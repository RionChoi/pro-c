// Session 48: C++ 그래픽스 & GPU 컴퓨팅 - GPGPU 커널 CPU 시뮬레이션
#include <algorithm>
#include <cmath>
#include <iostream>
#include <span>
#include <vector>

struct DispatchGrid {
    std::size_t global_size;
    std::size_t workgroup_size;

    std::size_t group_count() const {
        return (global_size + workgroup_size - 1) / workgroup_size;
    }
};

template <typename Kernel>
void dispatch(const DispatchGrid& grid, Kernel kernel) {
    for (std::size_t group = 0; group < grid.group_count(); ++group) {
        for (std::size_t local = 0; local < grid.workgroup_size; ++local) {
            const std::size_t global = group * grid.workgroup_size + local;
            if (global < grid.global_size) {
                kernel(global, group, local);
            }
        }
    }
}

void vector_add(std::span<const float> a, std::span<const float> b, std::span<float> out) {
    dispatch({out.size(), 4}, [&](std::size_t i, std::size_t, std::size_t) {
        out[i] = a[i] + b[i];
    });
}

void tone_map(std::span<float> pixels, float exposure) {
    dispatch({pixels.size(), 8}, [&](std::size_t i, std::size_t, std::size_t) {
        pixels[i] = 1.0f - std::exp(-pixels[i] * exposure);
    });
}

int main() {
    std::vector<float> a{1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<float> b{8, 7, 6, 5, 4, 3, 2, 1};
    std::vector<float> out(a.size());

    vector_add(a, b, out);
    std::cout << "vector_add: ";
    for (float value : out) {
        std::cout << value << ' ';
    }
    std::cout << '\n';

    std::vector<float> hdr{0.1f, 0.5f, 1.0f, 2.0f, 5.0f};
    tone_map(hdr, 1.2f);
    std::cout << "tone_map: ";
    for (float value : hdr) {
        std::cout << value << ' ';
    }
    std::cout << '\n';
}
