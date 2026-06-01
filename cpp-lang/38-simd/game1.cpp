#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;

struct Particle {
    float x, y;
    float vx, vy;
    float mass;
    uint32_t flags;  // 비트 플래그: [0] 활성화, [1] 충돌 중

    Particle() : x(0), y(0), vx(0), vy(0), mass(1.0f), flags(1) {}

    void set_active(bool active) {
        if (active) flags |= 1;
        else flags &= ~1;
    }

    bool is_active() const { return flags & 1; }

    void set_colliding(bool colliding) {
        if (colliding) flags |= 2;
        else flags &= ~2;
    }

    bool is_colliding() const { return (flags >> 1) & 1; }
};

// 간단한 입자계 (파티클 시스템)
class ParticleSystem {
private:
    vector<Particle> particles;
    float width, height;
    float gravity;
    int frame_count;

public:
    ParticleSystem(float w, float h)
        : width(w), height(h), gravity(9.81f), frame_count(0) {}

    void add_particle(float x, float y, float vx, float vy, float mass) {
        Particle p;
        p.x = x;
        p.y = y;
        p.vx = vx;
        p.vy = vy;
        p.mass = mass;
        particles.push_back(p);
    }

    void clear() {
        particles.clear();
    }

    // 최적화되지 않은 버전
    void update_naive(float dt) {
        for (auto& p : particles) {
            if (!p.is_active()) continue;

            // 속도 업데이트 (중력)
            p.vy += gravity * dt;

            // 위치 업데이트
            p.x += p.vx * dt;
            p.y += p.vy * dt;

            // 경계 충돌
            if (p.y > height) {
                p.y = height;
                p.vy = -p.vy * 0.8f;
            }
            if (p.x < 0) {
                p.x = 0;
                p.vx = -p.vx * 0.8f;
            }
            if (p.x > width) {
                p.x = width;
                p.vx = -p.vx * 0.8f;
            }
        }
    }

    // 루프 언롤링을 통한 최적화 버전
    void update_optimized(float dt) {
        int n = particles.size();
        int i = 0;

        // 4개씩 처리 (루프 언롤링)
        for (; i + 3 < n; i += 4) {
            // 파티클 0
            if (particles[i].is_active()) {
                particles[i].vy += gravity * dt;
                particles[i].x += particles[i].vx * dt;
                particles[i].y += particles[i].vy * dt;
                if (particles[i].y > height) {
                    particles[i].y = height;
                    particles[i].vy *= -0.8f;
                }
            }

            // 파티클 1
            if (particles[i + 1].is_active()) {
                particles[i + 1].vy += gravity * dt;
                particles[i + 1].x += particles[i + 1].vx * dt;
                particles[i + 1].y += particles[i + 1].vy * dt;
                if (particles[i + 1].y > height) {
                    particles[i + 1].y = height;
                    particles[i + 1].vy *= -0.8f;
                }
            }

            // 파티클 2
            if (particles[i + 2].is_active()) {
                particles[i + 2].vy += gravity * dt;
                particles[i + 2].x += particles[i + 2].vx * dt;
                particles[i + 2].y += particles[i + 2].vy * dt;
                if (particles[i + 2].y > height) {
                    particles[i + 2].y = height;
                    particles[i + 2].vy *= -0.8f;
                }
            }

            // 파티클 3
            if (particles[i + 3].is_active()) {
                particles[i + 3].vy += gravity * dt;
                particles[i + 3].x += particles[i + 3].vx * dt;
                particles[i + 3].y += particles[i + 3].vy * dt;
                if (particles[i + 3].y > height) {
                    particles[i + 3].y = height;
                    particles[i + 3].vy *= -0.8f;
                }
            }
        }

        // 남은 파티클 처리
        while (i < n) {
            if (particles[i].is_active()) {
                particles[i].vy += gravity * dt;
                particles[i].x += particles[i].vx * dt;
                particles[i].y += particles[i].vy * dt;
                if (particles[i].y > height) {
                    particles[i].y = height;
                    particles[i].vy *= -0.8f;
                }
            }
            i++;
        }
    }

    // 단순 충돌 감지 (거리 기반)
    void check_collisions(float collision_dist) {
        int n = particles.size();
        for (int i = 0; i < n; i++) {
            if (!particles[i].is_active()) continue;
            particles[i].set_colliding(false);

            for (int j = i + 1; j < n; j++) {
                if (!particles[j].is_active()) continue;

                float dx = particles[j].x - particles[i].x;
                float dy = particles[j].y - particles[i].y;
                float dist_sq = dx * dx + dy * dy;

                if (dist_sq < collision_dist * collision_dist) {
                    particles[i].set_colliding(true);
                    particles[j].set_colliding(true);

                    // 간단한 충돌 반응
                    float angle = atan2(dy, dx);
                    float v = sqrt(dist_sq) / 10;
                    particles[i].vx -= v * cos(angle);
                    particles[i].vy -= v * sin(angle);
                    particles[j].vx += v * cos(angle);
                    particles[j].vy += v * sin(angle);
                }
            }
        }
    }

    void render() const {
        for (int y = 0; y < 20; y++) {
            for (int x = 0; x < 40; x++) {
                bool found = false;
                for (const auto& p : particles) {
                    if (p.is_active() &&
                        (int)(p.x / (width / 40)) == x &&
                        (int)(p.y / (height / 20)) == y) {
                        cout << (p.is_colliding() ? "*" : "o");
                        found = true;
                        break;
                    }
                }
                if (!found) cout << ".";
            }
            cout << "\n";
        }
    }

    int get_particle_count() const { return particles.size(); }

    void print_stats() const {
        int active = 0;
        int colliding = 0;
        for (const auto& p : particles) {
            if (p.is_active()) active++;
            if (p.is_colliding()) colliding++;
        }
        cout << "활성: " << active << " | 충돌 중: " << colliding << "\n";
    }
};

void benchmark_comparison() {
    cout << "\n=== 성능 비교: Naive vs Optimized ===\n";
    cout << "---\n";

    ParticleSystem sys_naive(100, 50);
    ParticleSystem sys_optimized(100, 50);

    // 1000개 입자 추가
    srand(time(nullptr));
    for (int i = 0; i < 1000; i++) {
        float x = (rand() % 100);
        float y = (rand() % 50);
        float vx = (rand() % 40 - 20) / 10.0f;
        float vy = (rand() % 40 - 20) / 10.0f;
        sys_naive.add_particle(x, y, vx, vy, 1.0f);
        sys_optimized.add_particle(x, y, vx, vy, 1.0f);
    }

    float dt = 0.016f;  // 60 FPS

    // Naive 벤치마크
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; i++) {
        sys_naive.update_naive(dt);
    }
    auto end = chrono::high_resolution_clock::now();
    auto naive_time = chrono::duration_cast<chrono::milliseconds>(end - start);

    // Optimized 벤치마크
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; i++) {
        sys_optimized.update_optimized(dt);
    }
    end = chrono::high_resolution_clock::now();
    auto optimized_time = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "Naive version:      " << naive_time.count() << "ms\n";
    cout << "Optimized version:  " << optimized_time.count() << "ms\n";
    cout << "향상도: " << fixed << setprecision(1)
         << (100.0 * (naive_time.count() - optimized_time.count()) / naive_time.count())
         << "%\n";
}

int main() {
    cout << "=== 입자 시스템 게임 (SIMD 최적화) ===\n\n";

    ParticleSystem system(100, 50);

    // 초기 입자 설정
    system.add_particle(50, 5, 0, 0, 1.0f);
    system.add_particle(20, 10, 2, 0, 1.0f);
    system.add_particle(80, 15, -2, 0, 1.0f);
    system.add_particle(50, 20, 1, -1, 1.0f);

    cout << "초기 상태:\n";
    system.render();
    system.print_stats();

    float dt = 0.05f;

    for (int frame = 0; frame < 5; frame++) {
        cout << "\n프레임 " << frame + 1 << ":\n";

        system.update_optimized(dt);
        system.check_collisions(5.0f);

        system.render();
        system.print_stats();
    }

    // 성능 비교
    benchmark_comparison();

    cout << "\n[범례]\n";
    cout << ". = 빈 공간\n";
    cout << "o = 입자\n";
    cout << "* = 충돌 중인 입자\n";

    return 0;
}
