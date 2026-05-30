#include <iostream>
#include <vector>
#include <string>
#include <memory_resource>
#include <cstdlib>

struct Texture {
    std::string name;
    int width;
    int height;
    int color_depth;

    int size() const {
        return width * height * (color_depth / 8);
    }

    Texture(const std::string& n, int w, int h, int cd)
        : name(n), width(w), height(h), color_depth(cd) {}

    void print() const {
        std::cout << "  [" << name << "] " << width << "x" << height
                  << " (" << color_depth << "bit) - " << (size() / 1024) << "KB\n";
    }
};

using TextureAllocator = std::pmr::polymorphic_allocator<Texture>;
using TexturePool = std::pmr::vector<Texture>;

int main() {
    std::cout << "=== 텍스처 메모리 관리 게임 ===\n";
    std::cout << "메모리 풀을 효율적으로 관리하여 최대한 많은 텍스처를 로드하세요.\n\n";

    unsigned char memory_pool[65536];
    std::pmr::monotonic_buffer_resource resource(memory_pool, sizeof(memory_pool));

    TextureAllocator alloc(&resource);
    TexturePool textures(alloc);

    int total_memory = 0;
    int max_memory = sizeof(memory_pool);
    int loaded_count = 0;

    std::vector<std::pair<std::string, std::tuple<int, int, int>>> texture_data = {
        {"grass", {512, 512, 32}},
        {"stone", {512, 512, 24}},
        {"water", {256, 256, 32}},
        {"sky", {1024, 1024, 24}},
        {"tree", {256, 512, 32}},
        {"sand", {512, 512, 24}},
        {"snow", {256, 256, 32}},
        {"brick", {256, 256, 24}},
        {"metal", {512, 512, 32}},
        {"wood", {512, 512, 24}}
    };

    std::cout << "가용 메모리: " << max_memory / 1024 << "KB\n\n";

    for (const auto& [name, dims] : texture_data) {
        auto [w, h, bpp] = dims;
        Texture tex(name, w, h, bpp);
        int tex_size = tex.size();

        if (total_memory + tex_size <= max_memory) {
            textures.push_back(tex);
            total_memory += tex_size;
            ++loaded_count;
            std::cout << "[로드 성공] ";
            tex.print();
            std::cout << "  현재 메모리: " << total_memory / 1024 << "KB / "
                      << max_memory / 1024 << "KB\n";
        } else {
            std::cout << "[로드 실패] " << name << " (필요: " << tex_size / 1024
                      << "KB, 가용: " << (max_memory - total_memory) / 1024 << "KB)\n";
        }
    }

    std::cout << "\n=== 최종 통계 ===\n";
    std::cout << "로드된 텍스처: " << loaded_count << "개\n";
    std::cout << "사용된 메모리: " << total_memory / 1024 << "KB\n";
    std::cout << "남은 메모리: " << (max_memory - total_memory) / 1024 << "KB\n";
    std::cout << "메모리 효율: " << (total_memory * 100 / max_memory) << "%\n";

    return 0;
}
