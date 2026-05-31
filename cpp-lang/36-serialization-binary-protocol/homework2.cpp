#include <iostream>
#include <vector>
#include <map>
#include <cstring>
#include <iomanip>
#include <cstdint>

enum class MessageType : uint8_t {
    INT = 1,
    DOUBLE = 2,
    STRING = 3,
    VECTOR_INT = 4
};

class TypedBinaryBuffer {
private:
    std::vector<uint8_t> buffer;
    std::size_t read_pos = 0;

public:
    void write_type_byte(uint8_t val) {
        buffer.push_back(val);
    }

    void write_uint8(uint8_t val) {
        buffer.push_back(val);
    }

    void write_int32(int32_t val) {
        for (int i = 0; i < 4; ++i) {
            buffer.push_back(static_cast<uint8_t>((val >> (i * 8)) & 0xFF));
        }
    }

    void write_double(double val) {
        uint8_t *ptr = reinterpret_cast<uint8_t *>(&val);
        for (int i = 0; i < 8; ++i) {
            buffer.push_back(ptr[i]);
        }
    }

    void write_string(const std::string &str) {
        write_int32(static_cast<int32_t>(str.length()));
        for (char c : str) {
            buffer.push_back(static_cast<uint8_t>(c));
        }
    }

    void write_int_vector(const std::vector<int32_t> &vec) {
        write_int32(static_cast<int32_t>(vec.size()));
        for (int32_t val : vec) {
            write_int32(val);
        }
    }

    template<typename T>
    void write_value(MessageType type, const T &val) {
        write_uint8(static_cast<uint8_t>(type));
        if constexpr (std::is_same_v<T, int32_t>) {
            write_int32(val);
        } else if constexpr (std::is_same_v<T, double>) {
            write_double(val);
        } else if constexpr (std::is_same_v<T, std::string>) {
            write_string(val);
        }
    }

    uint8_t read_uint8() {
        if (read_pos >= buffer.size()) throw std::runtime_error("버퍼 오류");
        return buffer[read_pos++];
    }

    int32_t read_int32() {
        int32_t result = 0;
        for (int i = 0; i < 4; ++i) {
            result |= static_cast<int32_t>(read_uint8()) << (i * 8);
        }
        return result;
    }

    double read_double() {
        uint8_t bytes[8];
        for (int i = 0; i < 8; ++i) {
            bytes[i] = read_uint8();
        }
        return *reinterpret_cast<double *>(bytes);
    }

    std::string read_string() {
        int32_t len = read_int32();
        std::string result;
        for (int32_t i = 0; i < len; ++i) {
            result += static_cast<char>(read_uint8());
        }
        return result;
    }

    std::vector<int32_t> read_int_vector() {
        int32_t len = read_int32();
        std::vector<int32_t> result;
        for (int32_t i = 0; i < len; ++i) {
            result.push_back(read_int32());
        }
        return result;
    }

    MessageType read_type() {
        return static_cast<MessageType>(read_uint8());
    }

    const std::vector<uint8_t> &get_data() const { return buffer; }
    void reset_read() { read_pos = 0; }
    std::size_t size() const { return buffer.size(); }
};

void print_hex(const std::vector<uint8_t> &data, std::size_t limit = 16) {
    std::size_t count = 0;
    for (uint8_t b : data) {
        if (count >= limit) {
            std::cout << "...";
            break;
        }
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(b) << " ";
        count++;
    }
    std::cout << std::dec << "\n";
}

int main() {
    std::cout << "=== 타입 태그 직렬화 프로토콜 ===\n\n";

    std::cout << "1️⃣  타입 기반 메시지 직렬화:\n";
    TypedBinaryBuffer buf;
    buf.write_value(MessageType::INT, 42);
    buf.write_value(MessageType::DOUBLE, 3.14159);
    buf.write_value(MessageType::STRING, std::string("Protocol"));

    std::cout << "메시지 데이터 (Hex):\n";
    print_hex(buf.get_data());
    std::cout << "크기: " << buf.size() << " bytes\n\n";

    std::cout << "2️⃣  타입 기반 역직렬화:\n";
    buf.reset_read();

    MessageType type1 = buf.read_type();
    int32_t int_val = buf.read_int32();
    std::cout << "Type: INT, Value: " << int_val << "\n";

    MessageType type2 = buf.read_type();
    double double_val = buf.read_double();
    std::cout << "Type: DOUBLE, Value: " << std::fixed << std::setprecision(5)
              << double_val << "\n";

    MessageType type3 = buf.read_type();
    std::string string_val = buf.read_string();
    std::cout << "Type: STRING, Value: " << string_val << "\n\n";

    std::cout << "3️⃣  벡터 직렬화:\n";
    TypedBinaryBuffer vec_buf;
    vec_buf.write_uint8(static_cast<uint8_t>(MessageType::VECTOR_INT));
    std::vector<int32_t> numbers = {10, 20, 30, 40, 50};
    vec_buf.write_int_vector(numbers);

    std::cout << "벡터 직렬화 크기: " << vec_buf.size() << " bytes\n";
    std::cout << "데이터 (Hex): ";
    print_hex(vec_buf.get_data());

    std::cout << "\n4️⃣  벡터 역직렬화:\n";
    vec_buf.reset_read();
    MessageType vec_type = vec_buf.read_type();
    std::vector<int32_t> restored = vec_buf.read_int_vector();

    std::cout << "복원된 벡터: [";
    for (std::size_t i = 0; i < restored.size(); ++i) {
        std::cout << restored[i];
        if (i + 1 < restored.size()) std::cout << ", ";
    }
    std::cout << "]\n\n";

    std::cout << "5️⃣  다중 메시지 스트림:\n";
    TypedBinaryBuffer stream;
    stream.write_value(MessageType::INT, 100);
    stream.write_value(MessageType::STRING, std::string("Message1"));
    stream.write_value(MessageType::DOUBLE, 2.71828);

    std::cout << "스트림 크기: " << stream.size() << " bytes\n";
    std::cout << "스트림 데이터 (Hex): ";
    print_hex(stream.get_data(), 32);

    return 0;
}
