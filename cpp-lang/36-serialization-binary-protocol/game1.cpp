#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <cstdint>
#include <string>
#include <cctype>

enum class DataType : uint8_t {
    BYTE = 1,
    SHORT = 2,
    INT = 3,
    LONG = 4
};

class BinaryGame {
private:
    struct Challenge {
        DataType type;
        std::vector<uint8_t> expected_bytes;
        std::string description;
        int32_t original_value;
    };

    std::vector<Challenge> challenges;
    int score = 0;

    void serialize_int16(int16_t val, std::vector<uint8_t> &buf) {
        buf.push_back(static_cast<uint8_t>(val & 0xFF));
        buf.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));
    }

    void serialize_int32(int32_t val, std::vector<uint8_t> &buf) {
        for (int i = 0; i < 4; ++i) {
            buf.push_back(static_cast<uint8_t>((val >> (i * 8)) & 0xFF));
        }
    }

    void serialize_int64(int64_t val, std::vector<uint8_t> &buf) {
        for (int i = 0; i < 8; ++i) {
            buf.push_back(static_cast<uint8_t>((val >> (i * 8)) & 0xFF));
        }
    }

    void create_challenges() {
        Challenge c1;
        c1.type = DataType::BYTE;
        c1.original_value = 42;
        c1.expected_bytes = {42};
        c1.description = "바이트 42를 직렬화하면?";
        challenges.push_back(c1);

        Challenge c2;
        c2.type = DataType::SHORT;
        c2.original_value = 256;
        c2.expected_bytes = {0, 1};
        serialize_int16(256, c2.expected_bytes);
        c2.description = "short 256을 Little-Endian으로 직렬화하면?";
        challenges.push_back(c2);

        Challenge c3;
        c3.type = DataType::INT;
        c3.original_value = 1000;
        c3.expected_bytes.clear();
        serialize_int32(1000, c3.expected_bytes);
        c3.description = "int 1000을 직렬화하면? (4 바이트)";
        challenges.push_back(c3);

        Challenge c4;
        c4.type = DataType::INT;
        c4.original_value = 0x12345678;
        c4.expected_bytes.clear();
        serialize_int32(0x12345678, c4.expected_bytes);
        c4.description = "int 0x12345678을 Little-Endian으로?";
        challenges.push_back(c4);
    }

    void print_hex(const std::vector<uint8_t> &data) {
        std::cout << "[";
        for (std::size_t i = 0; i < data.size(); ++i) {
            std::cout << "0x" << std::hex << std::setw(2)
                      << std::setfill('0')
                      << static_cast<int>(data[i]);
            if (i + 1 < data.size()) std::cout << ", ";
        }
        std::cout << std::dec << "]\n";
    }

public:
    void play() {
        std::cout << "=== 바이너리 직렬화 게임 ===\n\n";
        std::cout << "각 문제에 대해 직렬화된 바이트를 맞혀봅시다!\n";
        std::cout << "Little-Endian 바이트 순서를 사용합니다.\n\n";

        create_challenges();

        for (std::size_t i = 0; i < challenges.size(); ++i) {
            play_challenge(challenges[i], i + 1);
        }

        std::cout << "\n🎮 게임 종료!\n";
        std::cout << "최종 점수: " << score << "/" << challenges.size() << "\n";

        if (score == static_cast<int>(challenges.size())) {
            std::cout << "🏆 완벽한 점수!\n";
        } else if (score >= static_cast<int>(challenges.size()) / 2) {
            std::cout << "🌟 좋은 성적!\n";
        } else {
            std::cout << "💪 더 연습해봅시다!\n";
        }
    }

private:
    void play_challenge(const Challenge &c, std::size_t num) {
        std::cout << "문제 " << num << ": " << c.description << "\n";
        std::cout << "정답의 바이트 개수: " << c.expected_bytes.size() << "\n\n";

        std::cout << "정답은? (바이트를 16진수로 입력, 공백으로 구분)\n";
        std::cout << "예: 0x2a 또는 42\n> ";

        std::vector<uint8_t> user_input;
        std::string input_line;
        std::getline(std::cin, input_line);

        if (!parse_input(input_line, user_input)) {
            std::cout << "❌ 입력 형식 오류!\n";
            show_answer(c);
            return;
        }

        if (user_input.size() != c.expected_bytes.size()) {
            std::cout << "❌ 바이트 개수가 맞지 않습니다!\n";
            std::cout << "   입력: " << user_input.size() << "개\n";
            show_answer(c);
            return;
        }

        bool correct = true;
        for (std::size_t i = 0; i < user_input.size(); ++i) {
            if (user_input[i] != c.expected_bytes[i]) {
                correct = false;
                break;
            }
        }

        if (correct) {
            std::cout << "✅ 정답!\n";
            score++;
        } else {
            std::cout << "❌ 틀렸습니다!\n";
            show_answer(c);
        }
        std::cout << "\n";
    }

    bool parse_input(const std::string &line, std::vector<uint8_t> &result) {
        std::size_t pos = 0;
        while (pos < line.length()) {
            while (pos < line.length() && std::isspace(line[pos])) {
                pos++;
            }
            if (pos >= line.length()) break;

            std::size_t start = pos;
            while (pos < line.length() && !std::isspace(line[pos])) {
                pos++;
            }

            std::string token = line.substr(start, pos - start);
            int val;

            if (token.substr(0, 2) == "0x" || token.substr(0, 2) == "0X") {
                val = std::stoi(token, nullptr, 16);
            } else {
                val = std::stoi(token);
            }

            if (val < 0 || val > 255) return false;
            result.push_back(static_cast<uint8_t>(val));
        }
        return !result.empty();
    }

    void show_answer(const Challenge &c) {
        std::cout << "정답: ";
        print_hex(c.expected_bytes);
        std::cout << "원본 값: " << c.original_value << "\n";
    }
};

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    BinaryGame game;
    game.play();

    return 0;
}
