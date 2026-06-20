// Session 48: C++ 그래픽스 & GPU 컴퓨팅 - 입자 렌더링 미니 시뮬레이션
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

struct Particle {
    float x;
    float y;
    float vx;
    float vy;
};

void simulate(std::vector<Particle>& particles, float dt) {
    for (Particle& p : particles) {
        p.vy += 9.8f * dt;
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        if (p.y > 1.0f) {
            p.y = 1.0f;
            p.vy *= -0.55f;
        }
    }
}

void render(const std::vector<Particle>& particles) {
    constexpr int width = 36;
    constexpr int height = 12;
    std::vector<std::string> frame(height, std::string(width, '.'));

    for (const Particle& p : particles) {
        const int x = std::clamp(static_cast<int>(p.x * static_cast<float>(width - 1)), 0, width - 1);
        const int y = std::clamp(static_cast<int>(p.y * static_cast<float>(height - 1)), 0, height - 1);
        frame[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = '*';
    }

    for (const auto& row : frame) {
        std::cout << row << '\n';
    }
}

int main() {
    std::vector<Particle> particles{
        {0.10f, 0.10f, 0.25f, -0.10f},
        {0.25f, 0.20f, 0.18f, -0.20f},
        {0.45f, 0.05f, 0.08f, -0.15f},
        {0.65f, 0.15f, -0.05f, -0.12f},
        {0.80f, 0.08f, -0.12f, -0.10f}
    };

    for (int frame = 0; frame < 3; ++frame) {
        std::cout << "=== frame " << frame << " ===\n";
        render(particles);
        simulate(particles, 0.12f);
    }
}
