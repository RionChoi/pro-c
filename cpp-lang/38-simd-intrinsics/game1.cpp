#include <iostream>
#include <immintrin.h>
#include <vector>
#include <cstring>
#include <cstdint>

class SimpleImage {
    int width, height;
    std::vector<uint8_t> pixels;

public:
    SimpleImage(int w, int h) : width(w), height(h), pixels(w * h, 128) {}

    void fillGradient() {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                pixels[i * width + j] = (i * 256 / height) % 256;
            }
        }
    }

    void printPixels(int rows = 5, int cols = 10) {
        for (int i = 0; i < rows && i < height; ++i) {
            for (int j = 0; j < cols && j < width; ++j) {
                printf("%3u ", pixels[i * width + j]);
            }
            std::cout << "..." << std::endl;
        }
    }

    void applyBrightnessNaive(int delta) {
        for (size_t i = 0; i < pixels.size(); ++i) {
            int val = pixels[i] + delta;
            pixels[i] = (val < 0) ? 0 : (val > 255) ? 255 : val;
        }
    }

    void applyBrightnessSIMD(int delta) {
        __m128i delta_vec = _mm_set1_epi16(delta);
        __m128i zero = _mm_setzero_si128();
        __m128i max_val = _mm_set1_epi16(255);

        for (size_t i = 0; i < pixels.size(); i += 16) {
            if (i + 16 > pixels.size()) break;

            __m128i data = _mm_loadu_si128((__m128i*)&pixels[i]);

            __m128i lo = _mm_cvtepu8_epi16(data);
            __m128i hi = _mm_cvtepu8_epi16(_mm_srli_si128(data, 8));

            lo = _mm_adds_epi16(lo, delta_vec);
            hi = _mm_adds_epi16(hi, delta_vec);

            lo = _mm_max_epi16(lo, zero);
            hi = _mm_max_epi16(hi, zero);
            lo = _mm_min_epi16(lo, max_val);
            hi = _mm_min_epi16(hi, max_val);

            __m128i result = _mm_packus_epi16(lo, hi);
            _mm_storeu_si128((__m128i*)&pixels[i], result);
        }
    }
};

int main() {
    std::cout << "=== 이미지 밝기 조절 게임 (SIMD) ===" << std::endl;

    SimpleImage img(20, 20);
    img.fillGradient();

    std::cout << "\n원본 이미지 (처음 5x10):" << std::endl;
    img.printPixels();

    std::cout << "\n밝기 +50 적용 (SIMD)..." << std::endl;
    img.applyBrightnessSIMD(50);
    img.printPixels();

    std::cout << "\n✓ 이미지 처리 완료!" << std::endl;
    std::cout << "SIMD를 사용하면 대량의 픽셀을 병렬로 처리할 수 있습니다." << std::endl;

    return 0;
}
