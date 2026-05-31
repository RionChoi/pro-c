#include <iostream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <cstdint>

class BinaryBuffer {
private:
    std::vector<uint8_t> buffer;
    std::size_t read_pos = 0;

public:
    void write_byte(uint8_t val) {
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

    uint8_t read_byte() {
        if (read_pos >= buffer.size()) throw std::runtime_error("버퍼 읽기 오류");
        return buffer[read_pos++];
    }

    int32_t read_int32() {
        int32_t result = 0;
        for (int i = 0; i < 4; ++i) {
            result |= static_cast<int32_t>(read_byte()) << (i * 8);
        }
        return result;
    }

    double read_double() {
        uint8_t bytes[8];
        for (int i = 0; i < 8; ++i) {
            bytes[i] = read_byte();
        }
        return *reinterpret_cast<double *>(bytes);
    }

    std::string read_string() {
        int32_t len = read_int32();
        std::string result;
        for (int32_t i = 0; i < len; ++i) {
            result += static_cast<char>(read_byte());
        }
        return result;
    }

    const std::vector<uint8_t> &get_data() const { return buffer; }
    void reset_read() { read_pos = 0; }
    std::size_t size() const { return buffer.size(); }
};

struct Person {
    int32_t id;
    std::string name;
    double height;

    void serialize(BinaryBuffer &buf) const {
        buf.write_int32(id);
        buf.write_string(name);
        buf.write_double(height);
    }

    void deserialize(BinaryBuffer &buf) {
        id = buf.read_int32();
        name = buf.read_string();
        height = buf.read_double();
    }

    void print() const {
        std::cout << "ID: " << id << ", Name: " << name
                  << ", Height: " << std::fixed << std::setprecision(2)
                  << height << "cm\n";
    }
};

void print_hex(const std::vector<uint8_t> &data) {
    for (uint8_t b : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(b) << " ";
    }
    std::cout << std::dec << "\n";
}

int main() {
    std::cout << "=== 이진 직렬화 프로토콜 ===\n\n";

    std::cout << "1️⃣  기본 타입 직렬화:\n";
    BinaryBuffer buf;
    buf.write_byte(42);
    buf.write_int32(123456);
    buf.write_double(3.14159);
    buf.write_string("Hello");

    std::cout << "직렬화된 데이터 (Hex):\n";
    print_hex(buf.get_data());
    std::cout << "바이트 크기: " << buf.size() << "\n\n";

    std::cout << "2️⃣  역직렬화:\n";
    buf.reset_read();
    uint8_t b = buf.read_byte();
    int32_t i = buf.read_int32();
    double d = buf.read_double();
    std::string s = buf.read_string();

    std::cout << "byte: " << static_cast<int>(b) << "\n";
    std::cout << "int32: " << i << "\n";
    std::cout << "double: " << std::fixed << std::setprecision(5) << d << "\n";
    std::cout << "string: " << s << "\n\n";

    std::cout << "3️⃣  구조체 직렬화:\n";
    Person person1{101, "Alice", 165.5};
    Person person2{102, "Bob", 178.3};

    BinaryBuffer person_buf;
    person1.serialize(person_buf);
    person2.serialize(person_buf);

    std::cout << "Person 데이터 (Hex):\n";
    print_hex(person_buf.get_data());
    std::cout << "총 바이트: " << person_buf.size() << "\n\n";

    std::cout << "4️⃣  구조체 역직렬화:\n";
    person_buf.reset_read();
    Person restored1, restored2;
    restored1.deserialize(person_buf);
    restored2.deserialize(person_buf);

    std::cout << "복원된 Person 1: ";
    restored1.print();
    std::cout << "복원된 Person 2: ";
    restored2.print();

    return 0;
}
