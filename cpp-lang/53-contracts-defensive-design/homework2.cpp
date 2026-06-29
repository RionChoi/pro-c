#include "contract.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>

template <typename T, std::size_t Capacity>
class BoundedQueue {
public:
    BoundedQueue()
    {
        CONTRACT_PRE_MSG(Capacity > 0, "queue capacity must be positive");
        assertInvariant();
    }

    void push(T value)
    {
        CONTRACT_PRE_MSG(!full(), "push requires free capacity");
        const std::size_t before = size_;

        storage_[tail_] = std::move(value);
        tail_ = next(tail_);
        ++size_;

        CONTRACT_POST(size_ == before + 1);
        assertInvariant();
    }

    T pop()
    {
        CONTRACT_PRE_MSG(!empty(), "pop requires at least one element");
        const std::size_t before = size_;

        T value = std::move(storage_[head_]);
        head_ = next(head_);
        --size_;

        CONTRACT_POST(size_ == before - 1);
        assertInvariant();
        return value;
    }

    const T& front() const
    {
        CONTRACT_PRE_MSG(!empty(), "front requires at least one element");
        return storage_[head_];
    }

    bool empty() const
    {
        return size_ == 0;
    }

    bool full() const
    {
        return size_ == Capacity;
    }

    std::size_t size() const
    {
        return size_;
    }

private:
    std::size_t next(std::size_t index) const
    {
        return (index + 1) % Capacity;
    }

    void assertInvariant() const
    {
        CONTRACT_ASSERT(size_ >= 0);
        CONTRACT_ASSERT(size_ <= Capacity);
        CONTRACT_ASSERT(head_ >= 0 && head_ < Capacity);
        CONTRACT_ASSERT(tail_ >= 0 && tail_ < Capacity);
    }

    std::array<T, Capacity> storage_{};
    std::size_t head_ = 0;
    std::size_t tail_ = 0;
    std::size_t size_ = 0;
};

void reportViolation(const lesson_contracts::ContractViolation& error)
{
    const lesson_contracts::ViolationInfo& info = error.info();
    std::cout << "계약 위반: " << lesson_contracts::kindName(info.kind) << " -> "
              << info.expression << '\n';
}

int main()
{
    BoundedQueue<std::string, 3> jobs;

    jobs.push("parse");
    jobs.push("validate");
    jobs.push("persist");

    std::cout << "front: " << jobs.front() << '\n';
    while (!jobs.empty()) {
        std::cout << "run: " << jobs.pop() << " (remaining " << jobs.size() << ")\n";
    }

    try {
        jobs.pop();
    } catch (const lesson_contracts::ContractViolation& error) {
        reportViolation(error);
    }

    return 0;
}
