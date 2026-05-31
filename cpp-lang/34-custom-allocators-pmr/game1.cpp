#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <ctime>

template <typename T>
class ObjectPool {
    std::vector<T> objects;
    std::vector<bool> active;
    std::size_t nextFreeIndex = 0;
    std::size_t allocationCount = 0;

public:
    explicit ObjectPool(std::size_t initialSize = 100)
        : objects(initialSize), active(initialSize, false) {}

    T* allocate() {
        for (std::size_t i = 0; i < active.size(); ++i) {
            if (!active[i]) {
                active[i] = true;
                allocationCount++;
                return &objects[i];
            }
        }

        objects.push_back(T());
        active.push_back(true);
        allocationCount++;
        return &objects.back();
    }

    void deallocate(T* ptr) {
        if (!ptr) return;
        for (std::size_t i = 0; i < objects.size(); ++i) {
            if (&objects[i] == ptr) {
                active[i] = false;
                return;
            }
        }
    }

    std::size_t getActiveCount() const {
        std::size_t count = 0;
        for (bool a : active) {
            if (a) count++;
        }
        return count;
    }

    std::size_t getAllocationCount() const {
        return allocationCount;
    }

    std::vector<T*> getActive() {
        std::vector<T*> result;
        for (std::size_t i = 0; i < objects.size(); ++i) {
            if (active[i]) {
                result.push_back(&objects[i]);
            }
        }
        return result;
    }
};

struct Particle {
    double x, y;
    double vx, vy;
    int lifetime;
    bool alive;

    Particle() : x(0), y(0), vx(0), vy(0), lifetime(0), alive(false) {}

    void init(double startX, double startY) {
        x = startX;
        y = startY;
        vx = (rand() % 100 - 50) * 0.01;
        vy = (rand() % 100 - 50) * 0.01 - 0.05;
        lifetime = 30 + (rand() % 20);
        alive = true;
    }

    void update() {
        if (!alive) return;
        x += vx;
        y += vy;
        vy -= 0.001;
        lifetime--;
        if (lifetime <= 0) {
            alive = false;
        }
    }

    char getChar() const {
        if (lifetime > 25) return '*';
        if (lifetime > 15) return '+';
        if (lifetime > 5) return '.';
        return '`';
    }
};

int main() {
    std::srand(std::time(nullptr));
    ObjectPool<Particle> pool(50);

    std::cout << "=== 파티클 시뮬레이션 (객체 풀 사용) ===\n\n";
    std::cout << "5초간 파티클 생성 및 시뮬레이션\n\n";

    int tick = 0;
    int maxTicks = 50;

    while (tick < maxTicks) {
        if (tick % 5 == 0) {
            for (int i = 0; i < 3; ++i) {
                Particle* p = pool.allocate();
                p->init(25.0, 10.0);
            }
        }

        auto particles = pool.getActive();
        for (auto p : particles) {
            p->update();
        }

        if (tick % 10 == 0) {
            std::cout << "Frame " << tick / 10 << ":\n";

            const int width = 50;
            const int height = 15;
            std::vector<std::vector<char>> canvas(height, std::vector<char>(width, ' '));

            for (auto p : particles) {
                if (p->alive) {
                    int ix = static_cast<int>(p->x);
                    int iy = static_cast<int>(p->y);
                    if (ix >= 0 && ix < width && iy >= 0 && iy < height) {
                        canvas[height - 1 - iy][ix] = p->getChar();
                    }
                }
            }

            for (const auto& row : canvas) {
                std::cout << "  |";
                for (char c : row) {
                    std::cout << c;
                }
                std::cout << "|\n";
            }

            std::cout << "  활성 파티클: " << pool.getActiveCount() << "\n\n";
        }

        tick++;
    }

    std::cout << "시뮬레이션 완료!\n";
    std::cout << "총 할당: " << pool.getAllocationCount() << "\n";
    std::cout << "최종 활성 파티클: " << pool.getActiveCount() << "\n";
    std::cout << "\n객체 풀을 통한 효율적 메모리 관리 성공!\n";

    return 0;
}
