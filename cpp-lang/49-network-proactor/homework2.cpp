// Session 49: C++ 네트워크 & Proactor 패턴 - HTTP/2 프레임 직렬화
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

struct Frame {
    std::uint32_t length;
    std::uint8_t type;
    std::uint8_t flags;
    std::uint32_t stream_id;
    std::vector<std::uint8_t> payload;
};

std::vector<std::uint8_t> serialize(const Frame& frame) {
    if (frame.payload.size() != frame.length) {
        throw std::runtime_error("payload length mismatch");
    }

    std::vector<std::uint8_t> out(9);
    out[0] = static_cast<std::uint8_t>((frame.length >> 16) & 0xffU);
    out[1] = static_cast<std::uint8_t>((frame.length >> 8) & 0xffU);
    out[2] = static_cast<std::uint8_t>(frame.length & 0xffU);
    out[3] = frame.type;
    out[4] = frame.flags;
    out[5] = static_cast<std::uint8_t>((frame.stream_id >> 24) & 0x7fU);
    out[6] = static_cast<std::uint8_t>((frame.stream_id >> 16) & 0xffU);
    out[7] = static_cast<std::uint8_t>((frame.stream_id >> 8) & 0xffU);
    out[8] = static_cast<std::uint8_t>(frame.stream_id & 0xffU);
    out.insert(out.end(), frame.payload.begin(), frame.payload.end());
    return out;
}

Frame parse(const std::vector<std::uint8_t>& bytes) {
    if (bytes.size() < 9) {
        throw std::runtime_error("frame too short");
    }

    Frame frame{};
    frame.length = (static_cast<std::uint32_t>(bytes[0]) << 16)
                 | (static_cast<std::uint32_t>(bytes[1]) << 8)
                 | static_cast<std::uint32_t>(bytes[2]);
    frame.type = bytes[3];
    frame.flags = bytes[4];
    frame.stream_id = ((static_cast<std::uint32_t>(bytes[5]) & 0x7fU) << 24)
                    | (static_cast<std::uint32_t>(bytes[6]) << 16)
                    | (static_cast<std::uint32_t>(bytes[7]) << 8)
                    | static_cast<std::uint32_t>(bytes[8]);

    if (bytes.size() != frame.length + 9U) {
        throw std::runtime_error("invalid frame length");
    }
    frame.payload.assign(bytes.begin() + 9, bytes.end());
    return frame;
}

int main() {
    std::string text = "hello";
    Frame frame{
        static_cast<std::uint32_t>(text.size()),
        0x0U,
        0x1U,
        1U,
        std::vector<std::uint8_t>(text.begin(), text.end())
    };

    auto bytes = serialize(frame);
    std::cout << "serialized:";
    for (std::uint8_t byte : bytes) {
        std::cout << ' ' << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(byte);
    }
    std::cout << std::dec << '\n';

    Frame parsed = parse(bytes);
    std::string payload(parsed.payload.begin(), parsed.payload.end());
    std::cout << "type=" << static_cast<int>(parsed.type)
              << " flags=" << static_cast<int>(parsed.flags)
              << " stream=" << parsed.stream_id
              << " payload=" << payload << '\n';
}
