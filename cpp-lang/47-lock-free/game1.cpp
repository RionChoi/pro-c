// Session 47: MPSC 태스크 큐 시뮬레이터 게임
// Multi-Producer Single-Consumer lock-free 큐로 에이전트 작업 처리
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <array>
#include <iostream>
#include <string_view>
#include <string>
#include <cassert>
#include <new>
#include <cstdio>

// ─────────────────────────────────────────────
// MPSC Lock-Free Queue
// Multiple Producer, Single Consumer
// ─────────────────────────────────────────────

template <typename T>
class MPSCQueue {
    struct Node {
        std::atomic<Node*> next{nullptr};
        T                  value{};
        explicit Node(T v) : value(std::move(v)) {}
        Node() = default;
    };

public:
    MPSCQueue() {
        Node* sentinel = new Node();
        head_.store(sentinel, std::memory_order_relaxed);
        tail_ = sentinel;  // tail은 consumer만 접근 — atomic 불필요
    }

    ~MPSCQueue() {
        Node* cur = tail_;
        while (cur) {
            Node* nxt = cur->next.load(std::memory_order_relaxed);
            delete cur;
            cur = nxt;
        }
    }

    // Producer 측 — 여러 스레드 안전 (lock-free)
    void enqueue(T val) {
        Node* node = new Node(std::move(val));
        // head에 원자적으로 연결
        Node* prev = head_.exchange(node, std::memory_order_acq_rel);
        prev->next.store(node, std::memory_order_release);
        ++enqueue_count_;
    }

    // Consumer 측 — 싱글 스레드에서만 호출
    std::optional<T> dequeue() {
        Node* next = tail_->next.load(std::memory_order_acquire);
        if (!next) return std::nullopt;
        T val = std::move(next->value);
        delete tail_;
        tail_ = next;
        ++dequeue_count_;
        return val;
    }

    bool empty() const noexcept {
        return tail_->next.load(std::memory_order_acquire) == nullptr;
    }

    std::size_t enqueue_count() const noexcept { return enqueue_count_.load(); }
    std::size_t dequeue_count() const noexcept { return dequeue_count_.load(); }
    std::size_t pending()       const noexcept {
        return enqueue_count_.load() - dequeue_count_.load();
    }

private:
    alignas(64) std::atomic<Node*> head_{};  // producers가 접근
    Node*                           tail_{};  // consumer만 접근
    std::atomic<std::size_t>        enqueue_count_{};
    std::atomic<std::size_t>        dequeue_count_{};
};

// ─────────────────────────────────────────────
// 게임 엔티티
// ─────────────────────────────────────────────

enum class TaskType { Move, Attack, Heal, Scout, Rest };

struct Task {
    int      agent_id;
    TaskType type;
    int      priority;
    int      payload;

    std::string type_str() const {
        switch (type) {
        case TaskType::Move:   return "Move";
        case TaskType::Attack: return "Attack";
        case TaskType::Heal:   return "Heal";
        case TaskType::Scout:  return "Scout";
        case TaskType::Rest:   return "Rest";
        }
        return "Unknown";
    }
};

struct Agent {
    int         id;
    std::string name;
    int         health{100};
    int         energy{100};
    int         tasks_done{};
    int         x{}, y{};
};

// ─────────────────────────────────────────────
// LCG 난수
// ─────────────────────────────────────────────

struct Rng {
    unsigned s;
    explicit Rng(unsigned seed) : s(seed) {}
    unsigned next() { return s = s * 1664525u + 1013904223u; }
    int      nexti(int lo, int hi) { return lo + int(next() % unsigned(hi - lo + 1)); }
    TaskType next_type() { return TaskType(next() % 5); }
};

// ─────────────────────────────────────────────
// 게임 시뮬레이터
// ─────────────────────────────────────────────

class GameSim {
public:
    static constexpr int NUM_AGENTS = 4;
    static constexpr int NUM_ROUNDS = 5;

    GameSim() {
        agents_[0] = {0, "알파", 100, 100, 0, 10, 10};
        agents_[1] = {1, "베타", 100, 100, 0, 50, 20};
        agents_[2] = {2, "감마", 100, 100, 0, 30, 60};
        agents_[3] = {3, "델타", 100, 100, 0, 70, 40};
    }

    void run() {
        std::cout << "╔══════════════════════════════════════════════╗\n";
        std::cout << "║    MPSC 태스크 큐 — 멀티 에이전트 시뮬레이터  ║\n";
        std::cout << "╚══════════════════════════════════════════════╝\n\n";

        Rng rng(777u);

        for (int round = 1; round <= NUM_ROUNDS; ++round) {
            std::cout << "━━━ 라운드 " << round << " ━━━\n";

            // [Producer phase] 각 에이전트가 태스크를 큐에 제출
            for (int i = 0; i < NUM_AGENTS; ++i) {
                int n_tasks = rng.nexti(1, 3);
                for (int k = 0; k < n_tasks; ++k) {
                    Task t{i, rng.next_type(), rng.nexti(1, 10), rng.nexti(0, 100)};
                    queue_.enqueue(t);
                }
            }

            std::cout << "  제출된 태스크: " << queue_.pending() << "개\n";

            // [Consumer phase] 싱글 컨슈머가 모든 태스크 처리
            int processed = 0;
            while (!queue_.empty()) {
                auto t = queue_.dequeue();
                if (!t) break;
                process_task(*t, rng);
                ++processed;
            }

            // 라운드 통계
            std::cout << "  처리 완료: " << processed << "개\n";
            std::cout << "  누적 enqueue=" << queue_.enqueue_count()
                      << "  dequeue=" << queue_.dequeue_count()
                      << "  pending=" << queue_.pending() << "\n\n";
        }

        print_final_stats();
    }

private:
    void process_task(const Task& t, Rng& rng) {
        auto& a = agents_[t.agent_id];
        switch (t.type) {
        case TaskType::Move:
            a.x += rng.nexti(-5, 5);
            a.y += rng.nexti(-5, 5);
            a.energy -= 5;
            break;
        case TaskType::Attack:
            a.energy -= 10;
            break;
        case TaskType::Heal:
            a.health = std::min(100, a.health + t.payload / 5);
            a.energy -= 3;
            break;
        case TaskType::Scout:
            a.energy -= 8;
            break;
        case TaskType::Rest:
            a.energy = std::min(100, a.energy + 20);
            break;
        }
        a.energy  = std::max(0, a.energy);
        a.health  = std::max(0, a.health);
        ++a.tasks_done;
    }

    void print_final_stats() const {
        std::cout << "=== 최종 에이전트 상태 ===\n";
        std::printf("%-6s %-6s %-6s %-6s %-8s %-8s\n",
                    "ID", "이름", "HP", "에너지", "완료태스크", "위치");
        std::cout << std::string(50, '-') << "\n";
        for (const auto& a : agents_) {
            std::printf("  %-4d %-6s %3d  %5d     %5d     (%d,%d)\n",
                        a.id, a.name.c_str(), a.health, a.energy,
                        a.tasks_done, a.x, a.y);
        }

        std::cout << "\n=== MPSC 큐 통계 ===\n";
        std::cout << "  총 enqueue: " << queue_.enqueue_count() << "\n";
        std::cout << "  총 dequeue: " << queue_.dequeue_count() << "\n";
        std::cout << "  미처리:     " << queue_.pending()
                  << (queue_.pending() == 0 ? " ✅" : " ❌") << "\n";

        std::cout << "\n=== 핵심 관찰 ===\n";
        std::cout << "  ✅ Producers(에이전트들): lock-free enqueue — 뮤텍스 없음\n";
        std::cout << "  ✅ Consumer(처리기): wait-free dequeue — 경쟁 없음\n";
        std::cout << "  ✅ 전체 시뮬레이션 heap 외 추가 동기화 없음\n";
    }

    MPSCQueue<Task>                     queue_;
    std::array<Agent, NUM_AGENTS>       agents_{};
};

// ─────────────────────────────────────────────
// main
// ─────────────────────────────────────────────

int main() {
    GameSim sim;
    sim.run();

    std::cout << "\n=== 게임 완료 ===\n";
    return 0;
}
