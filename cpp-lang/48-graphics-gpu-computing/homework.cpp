// Session 48: C++ 그래픽스 & GPU 컴퓨팅 - 소프트웨어 파이프라인
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

struct Vec2 {
    float x;
    float y;
};

struct Color {
    float r;
    float g;
    float b;
};

struct Vertex {
    Vec2 position;
    Color color;
};

struct VertexOut {
    Vec2 screen;
    Color color;
};

template <typename T>
concept ShaderOutput = requires(T value) {
    { value.screen.x } -> std::convertible_to<float>;
    { value.color.r } -> std::convertible_to<float>;
};

static_assert(ShaderOutput<VertexOut>);

struct PushConstants {
    float scale;
    Vec2 offset;
};

VertexOut vertex_shader(const Vertex& vertex, const PushConstants& pc) {
    return {
        {vertex.position.x * pc.scale + pc.offset.x,
         vertex.position.y * pc.scale + pc.offset.y},
        vertex.color
    };
}

char shade_pixel(const Color& color) {
    const float luminance = color.r * 0.2126f + color.g * 0.7152f + color.b * 0.0722f;
    if (luminance > 0.75f) return '#';
    if (luminance > 0.45f) return '*';
    if (luminance > 0.20f) return '+';
    return '.';
}

int main() {
    constexpr int width = 32;
    constexpr int height = 12;
    std::vector<std::string> framebuffer(height, std::string(width, ' '));

    std::array<Vertex, 4> vertices{{
        {{-0.8f, -0.6f}, {1.0f, 0.2f, 0.2f}},
        {{-0.2f,  0.7f}, {0.2f, 1.0f, 0.2f}},
        {{ 0.5f,  0.3f}, {0.2f, 0.4f, 1.0f}},
        {{ 0.9f, -0.5f}, {1.0f, 1.0f, 0.2f}}
    }};

    PushConstants pc{1.0f, {0.0f, 0.0f}};
    for (const Vertex& vertex : vertices) {
        VertexOut out = vertex_shader(vertex, pc);
        const int sx = static_cast<int>((out.screen.x * 0.5f + 0.5f) * static_cast<float>(width - 1));
        const int sy = static_cast<int>((1.0f - (out.screen.y * 0.5f + 0.5f)) * static_cast<float>(height - 1));
        if (sx >= 0 && sx < width && sy >= 0 && sy < height) {
            framebuffer[static_cast<std::size_t>(sy)][static_cast<std::size_t>(sx)] = shade_pixel(out.color);
        }
    }

    std::cout << "=== software vertex pipeline ===\n";
    for (const auto& row : framebuffer) {
        std::cout << row << '\n';
    }
}
