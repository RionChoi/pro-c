#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>

struct Message {
    uint8_t version;
    uint16_t msg_id;
    uint16_t length;
    std::vector<uint8_t> payload;
    uint8_t checksum;
};

uint8_t calculate_checksum(const std::vector<uint8_t>& data) {
    uint8_t sum = 0;
    for (uint8_t byte : data) {
        sum ^= byte;
    }
    return sum;
}

bool serialize_message(const char* filename, const Message& msg) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "파일 열기 실패\n";
        return false;
    }

    std::vector<uint8_t> header;
    header.push_back(msg.version);
    header.push_back(msg.msg_id >> 8);
    header.push_back(msg.msg_id & 0xFF);
    header.push_back(msg.length >> 8);
    header.push_back(msg.length & 0xFF);

    uint8_t checksum = calculate_checksum(header);
    for (uint8_t byte : msg.payload) {
        checksum ^= byte;
    }

    file.write(reinterpret_cast<const char*>(header.data()), header.size());
    file.write(reinterpret_cast<const char*>(msg.payload.data()), msg.payload.size());
    file.write(reinterpret_cast<const char*>(&checksum), 1);

    file.close();
    std::cout << "메시지 직렬화 완료: " << filename << " (길이: " << msg.length << ")\n";
    return true;
}

Message deserialize_message(const char* filename) {
    Message msg{};
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "파일 열기 실패\n";
        return msg;
    }

    std::vector<uint8_t> header(5);
    file.read(reinterpret_cast<char*>(header.data()), 5);

    msg.version = header[0];
    msg.msg_id = (header[1] << 8) | header[2];
    msg.length = (header[3] << 8) | header[4];

    msg.payload.resize(msg.length);
    file.read(reinterpret_cast<char*>(msg.payload.data()), msg.length);

    file.read(reinterpret_cast<char*>(&msg.checksum), 1);

    uint8_t computed = calculate_checksum(header);
    for (uint8_t byte : msg.payload) {
        computed ^= byte;
    }

    if (computed != msg.checksum) {
        std::cerr << "체크섬 오류: 기대값 " << int(computed) << ", 실제값 " << int(msg.checksum) << '\n';
    }

    file.close();
    return msg;
}

int main() {
    Message msg;
    msg.version = 1;
    msg.msg_id = 256;
    msg.payload = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};
    msg.length = msg.payload.size();

    serialize_message("message.bin", msg);

    Message loaded = deserialize_message("message.bin");
    std::cout << "복원된 메시지: 버전=" << int(loaded.version)
              << ", ID=" << loaded.msg_id
              << ", 길이=" << loaded.length << '\n';
    std::cout << "페이로드: ";
    for (uint8_t byte : loaded.payload) {
        std::cout << static_cast<char>(byte);
    }
    std::cout << '\n';

    return 0;
}
