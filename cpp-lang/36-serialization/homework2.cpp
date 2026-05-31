#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>

struct Message {
    static constexpr uint8_t TYPE_TEXT = 1;
    static constexpr uint8_t TYPE_DATA = 2;

    uint8_t type;
    uint32_t length;
    std::vector<uint8_t> payload;
    uint8_t checksum;

    uint8_t calculate_checksum() const {
        uint8_t sum = type;
        for (uint8_t byte : payload) {
            sum = (sum + byte) % 256;
        }
        return sum;
    }

    bool validate_checksum() const {
        return checksum == calculate_checksum();
    }

    bool serialize(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&type), sizeof(type));
        out.write(reinterpret_cast<const char*>(&length), sizeof(length));
        out.write(reinterpret_cast<const char*>(payload.data()), length);
        out.write(reinterpret_cast<const char*>(&checksum), sizeof(checksum));
        return out.good();
    }

    bool deserialize(std::ifstream& in) {
        if (!in.read(reinterpret_cast<char*>(&type), sizeof(type))) return false;
        if (!in.read(reinterpret_cast<char*>(&length), sizeof(length))) return false;

        payload.resize(length);
        if (!in.read(reinterpret_cast<char*>(payload.data()), length)) return false;
        if (!in.read(reinterpret_cast<char*>(&checksum), sizeof(checksum))) return false;

        return true;
    }

    void display() const {
        std::cout << "Type: " << static_cast<int>(type)
                  << ", Length: " << length
                  << ", Checksum: " << static_cast<int>(checksum)
                  << ", Valid: " << (validate_checksum() ? "Yes" : "No") << '\n';
        if (type == TYPE_TEXT && payload.size() > 0) {
            std::cout << "Text: ";
            for (uint8_t c : payload) std::cout << static_cast<char>(c);
            std::cout << '\n';
        }
    }
};

int main() {
    std::vector<Message> messages;

    Message msg1;
    msg1.type = Message::TYPE_TEXT;
    msg1.payload = {'H', 'e', 'l', 'l', 'o'};
    msg1.length = msg1.payload.size();
    msg1.checksum = msg1.calculate_checksum();
    messages.push_back(msg1);

    Message msg2;
    msg2.type = Message::TYPE_TEXT;
    msg2.payload = {'W', 'o', 'r', 'l', 'd'};
    msg2.length = msg2.payload.size();
    msg2.checksum = msg2.calculate_checksum();
    messages.push_back(msg2);

    std::cout << "=== 메시지 직렬화 ===\n";
    std::ofstream out("messages.bin", std::ios::binary);
    if (!out) {
        std::cerr << "파일 열기 실패\n";
        return 1;
    }

    for (const auto& msg : messages) {
        if (!msg.serialize(out)) {
            std::cerr << "직렬화 실패\n";
            return 1;
        }
    }
    out.close();
    std::cout << "저장 완료: messages.bin\n\n";

    std::cout << "=== 메시지 역직렬화 ===\n";
    std::ifstream in("messages.bin", std::ios::binary);
    if (!in) {
        std::cerr << "파일 열기 실패\n";
        return 1;
    }

    Message loaded;
    int count = 0;
    while (loaded.deserialize(in)) {
        std::cout << "메시지 " << (++count) << ": ";
        loaded.display();
    }
    in.close();

    return 0;
}
