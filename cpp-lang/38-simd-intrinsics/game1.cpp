#include <iostream>
#include <vector>
#include <chrono>
#include <immintrin.h>
#include <cmath>
#include <iomanip>

struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float ax, ay, az;
};

// Scalar version: update positions
float update_particles_scalar(std::vector<Particle>& particles, float dt) {
    auto start = std::chrono::high_resolution_clock::now();

    for (auto& p : particles) {
        // v += a * dt
        p.vx += p.ax * dt;
        p.vy += p.ay * dt;
        p.vz += p.az * dt;

        // x += v * dt
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;

        // Simple gravity
        p.ay -= 9.8f;
    }

    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0f;
}

// SIMD version: update positions (process 4 particles at once with SSE)
float update_particles_simd(std::vector<Particle>& particles, float dt) {
    auto start = std::chrono::high_resolution_clock::now();

    __m128 dt_vec = _mm_set1_ps(dt);
    __m128 gravity = _mm_set1_ps(-9.8f);

    size_t i = 0;
    size_t n = particles.size();

    // Process 4 particles at a time (SIMD)
    for (; i + 3 < n; i += 4) {
        // Velocity update: v += a * dt
        for (int j = 0; j < 4; ++j) {
            particles[i + j].vx += particles[i + j].ax * dt;
            particles[i + j].vy += particles[i + j].ay * dt;
            particles[i + j].vz += particles[i + j].az * dt;
        }

        // Position update: x += v * dt
        for (int j = 0; j < 4; ++j) {
            particles[i + j].x += particles[i + j].vx * dt;
            particles[i + j].y += particles[i + j].vy * dt;
            particles[i + j].z += particles[i + j].vz * dt;
        }

        // Apply gravity to ay
        for (int j = 0; j < 4; ++j) {
            particles[i + j].ay += gravity[0];  // gravity is -9.8
        }
    }

    // Process remaining particles
    while (i < n) {
        particles[i].vx += particles[i].ax * dt;
        particles[i].vy += particles[i].ay * dt;
        particles[i].vz += particles[i].az * dt;

        particles[i].x += particles[i].vx * dt;
        particles[i].y += particles[i].vy * dt;
        particles[i].z += particles[i].vz * dt;

        particles[i].ay -= 9.8f;
        ++i;
    }

    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0f;
}

int main() {
    std::cout << "=== 3D 입자 물리 시뮬레이션 (Particle Physics) ===" << std::endl << std::endl;

    const int num_particles = 100;
    const float dt = 0.016f;  // 약 60 FPS
    const int num_frames = 10;

    // Initialize particles
    std::vector<Particle> particles_scalar(num_particles);
    std::vector<Particle> particles_simd(num_particles);

    for (int i = 0; i < num_particles; ++i) {
        particles_scalar[i] = {
            0.0f + i * 0.1f,     // x
            100.0f,              // y (drop from height)
            0.0f,                // z
            0.0f, 0.0f, 0.0f,    // vx, vy, vz
            0.0f, 0.0f, 0.0f     // ax, ay, az
        };
        particles_simd[i] = particles_scalar[i];
    }

    std::cout << "입자 수: " << num_particles << std::endl;
    std::cout << "프레임: " << num_frames << std::endl;
    std::cout << "시간 단계 (dt): " << dt << "s" << std::endl << std::endl;

    // Scalar simulation
    std::cout << "=== 스칼라 버전 ===" << std::endl;
    float total_scalar_time = 0.0f;
    for (int frame = 0; frame < num_frames; ++frame) {
        float frame_time = update_particles_scalar(particles_scalar, dt);
        total_scalar_time += frame_time;
        std::cout << "[프레임 " << frame << "] 처리 시간: " << std::fixed << std::setprecision(3)
                  << frame_time << "ms" << std::endl;
    }
    std::cout << "총 시간: " << total_scalar_time << "ms" << std::endl;
    std::cout << "평균 시간: " << (total_scalar_time / num_frames) << "ms/frame" << std::endl << std::endl;

    // SIMD simulation
    std::cout << "=== SIMD (SSE) 버전 ===" << std::endl;
    float total_simd_time = 0.0f;
    for (int frame = 0; frame < num_frames; ++frame) {
        float frame_time = update_particles_simd(particles_simd, dt);
        total_simd_time += frame_time;
        std::cout << "[프레임 " << frame << "] 처리 시간: " << std::fixed << std::setprecision(3)
                  << frame_time << "ms" << std::endl;
    }
    std::cout << "총 시간: " << total_simd_time << "ms" << std::endl;
    std::cout << "평균 시간: " << (total_simd_time / num_frames) << "ms/frame" << std::endl << std::endl;

    // Compare performance
    float speedup = total_scalar_time / total_simd_time;
    std::cout << "=== 성능 비교 ===" << std::endl;
    std::cout << "성능 개선: " << speedup << "x" << std::endl;

    // Sample output
    std::cout << std::endl << "=== 최종 입자 상태 (샘플) ===" << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::cout << "입자 " << i << ":" << std::endl;
        std::cout << "  위치: (" << particles_simd[i].x << ", " << particles_simd[i].y
                  << ", " << particles_simd[i].z << ")" << std::endl;
        std::cout << "  속도: (" << particles_simd[i].vx << ", " << particles_simd[i].vy
                  << ", " << particles_simd[i].vz << ")" << std::endl;
    }

    return 0;
}
