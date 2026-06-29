#ifndef RECLAIM_HPP
#define RECLAIM_HPP

#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace reclaim {

class HazardDomain {
public:
    static constexpr std::size_t maxSlots = 8;

    class Guard {
    public:
        explicit Guard(HazardDomain& domain)
            : domain_(&domain),
              index_(domain.acquireSlot())
        {
        }

        Guard(const Guard&) = delete;
        Guard& operator=(const Guard&) = delete;

        Guard(Guard&& other) noexcept
            : domain_(other.domain_),
              index_(other.index_)
        {
            other.domain_ = nullptr;
        }

        ~Guard()
        {
            if (domain_ != nullptr) {
                domain_->clear(index_);
                domain_->releaseSlot(index_);
            }
        }

        template <typename T>
        T* protect(const std::atomic<T*>& target)
        {
            while (true) {
                T* observed = target.load(std::memory_order_acquire);
                domain_->slots_[index_].ptr.store(observed, std::memory_order_release);
                if (observed == target.load(std::memory_order_acquire)) {
                    return observed;
                }
            }
        }

        void clear()
        {
            if (domain_ != nullptr) {
                domain_->clear(index_);
            }
        }

    private:
        HazardDomain* domain_;
        std::size_t index_;
    };

    bool isProtected(void* pointer) const
    {
        for (const Slot& slot : slots_) {
            if (slot.active.load(std::memory_order_acquire) &&
                slot.ptr.load(std::memory_order_acquire) == pointer) {
                return true;
            }
        }
        return false;
    }

private:
    struct Slot {
        std::atomic<bool> active{false};
        std::atomic<void*> ptr{nullptr};
    };

    std::size_t acquireSlot()
    {
        for (std::size_t index = 0; index < slots_.size(); ++index) {
            bool expected = false;
            if (slots_[index].active.compare_exchange_strong(expected,
                                                             true,
                                                             std::memory_order_acq_rel,
                                                             std::memory_order_relaxed)) {
                return index;
            }
        }

        throw std::runtime_error("no hazard pointer slots left");
    }

    void releaseSlot(std::size_t index)
    {
        slots_[index].active.store(false, std::memory_order_release);
    }

    void clear(std::size_t index)
    {
        slots_[index].ptr.store(nullptr, std::memory_order_release);
    }

    std::array<Slot, maxSlots> slots_{};

    friend class Guard;
};

class RetireList {
public:
    template <typename T>
    void retire(T* pointer)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        items_.push_back(Entry{pointer, [](void* value) { delete static_cast<T*>(value); }});
    }

    std::size_t scan(const HazardDomain& hazards)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        std::size_t reclaimed = 0;
        std::vector<Entry> keep;
        keep.reserve(items_.size());

        for (Entry& entry : items_) {
            if (hazards.isProtected(entry.pointer)) {
                keep.push_back(entry);
            } else {
                entry.deleter(entry.pointer);
                ++reclaimed;
            }
        }

        items_ = std::move(keep);
        return reclaimed;
    }

    std::size_t pending() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return items_.size();
    }

    void drainUnsafe()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (Entry& entry : items_) {
            entry.deleter(entry.pointer);
        }
        items_.clear();
    }

private:
    struct Entry {
        void* pointer;
        void (*deleter)(void*);
    };

    mutable std::mutex mutex_;
    std::vector<Entry> items_;
};

template <typename T>
class AtomicStack {
public:
    AtomicStack() = default;

    ~AtomicStack()
    {
        while (pop().has_value()) {
        }
        retired_.scan(hazards_);
        retired_.drainUnsafe();
    }

    void push(T value)
    {
        Node* node = new Node{std::move(value), nullptr};
        node->next = head_.load(std::memory_order_relaxed);
        while (!head_.compare_exchange_weak(node->next,
                                            node,
                                            std::memory_order_release,
                                            std::memory_order_relaxed)) {
        }
    }

    std::optional<T> pop()
    {
        HazardDomain::Guard guard(hazards_);

        while (true) {
            Node* current = guard.protect(head_);
            if (current == nullptr) {
                return std::nullopt;
            }

            Node* next = current->next;
            if (head_.compare_exchange_weak(current,
                                            next,
                                            std::memory_order_acq_rel,
                                            std::memory_order_relaxed)) {
                guard.clear();
                T value = std::move(current->value);
                retired_.retire(current);
                if (retired_.pending() >= retireThreshold) {
                    retired_.scan(hazards_);
                }
                return value;
            }
        }
    }

    std::size_t reclaimNow()
    {
        return retired_.scan(hazards_);
    }

    std::size_t pendingRetired() const
    {
        return retired_.pending();
    }

private:
    struct Node {
        T value;
        Node* next;
    };

    static constexpr std::size_t retireThreshold = 4;

    std::atomic<Node*> head_{nullptr};
    HazardDomain hazards_;
    RetireList retired_;
};

class EpochDomain {
public:
    class Guard {
    public:
        explicit Guard(EpochDomain& domain)
            : domain_(&domain),
              epoch_(domain.enter())
        {
        }

        Guard(const Guard&) = delete;
        Guard& operator=(const Guard&) = delete;

        ~Guard()
        {
            if (domain_ != nullptr) {
                domain_->leave(epoch_);
            }
        }

    private:
        EpochDomain* domain_;
        std::size_t epoch_;
    };

    template <typename T>
    void retire(T* pointer)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        // ponytail: single retired vector is enough here; sharding only matters for real contention.
        retired_.push_back(Entry{
            pointer,
            [](void* value) { delete static_cast<T*>(value); },
            globalEpoch_.load(std::memory_order_acquire),
        });
    }

    std::size_t synchronize()
    {
        const std::size_t nextEpoch =
            globalEpoch_.fetch_add(1, std::memory_order_acq_rel) + 1;

        if (nextEpoch < 2) {
            return 0;
        }

        const std::size_t safeEpoch = nextEpoch - 2;
        if (readers_[safeEpoch % readers_.size()].load(std::memory_order_acquire) != 0) {
            return 0;
        }

        std::lock_guard<std::mutex> lock(mutex_);

        std::size_t reclaimed = 0;
        std::vector<Entry> keep;
        keep.reserve(retired_.size());

        for (Entry& entry : retired_) {
            if (entry.epoch <= safeEpoch) {
                entry.deleter(entry.pointer);
                ++reclaimed;
            } else {
                keep.push_back(entry);
            }
        }

        retired_ = std::move(keep);
        return reclaimed;
    }

    std::size_t pending() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return retired_.size();
    }

    void drainUnsafe()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (Entry& entry : retired_) {
            entry.deleter(entry.pointer);
        }
        retired_.clear();
    }

private:
    struct Entry {
        void* pointer;
        void (*deleter)(void*);
        std::size_t epoch;
    };

    std::size_t enter()
    {
        const std::size_t epoch = globalEpoch_.load(std::memory_order_acquire);
        readers_[epoch % readers_.size()].fetch_add(1, std::memory_order_acq_rel);
        return epoch;
    }

    void leave(std::size_t epoch)
    {
        readers_[epoch % readers_.size()].fetch_sub(1, std::memory_order_acq_rel);
    }

    std::atomic<std::size_t> globalEpoch_{0};
    std::array<std::atomic<std::size_t>, 3> readers_{};
    mutable std::mutex mutex_;
    std::vector<Entry> retired_;

    friend class Guard;
};

}  // namespace reclaim

#endif
