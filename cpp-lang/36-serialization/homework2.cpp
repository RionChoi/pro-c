#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>

class BinaryWriter {
private:
    std::vector<uint8_t> buffer;

public:
    void write_int(int32_t value) {
        buffer.push_back((value >> 0) & 0xFF);
        buffer.push_back((value >> 8) & 0xFF);
        buffer.push_back((value >> 16) & 0xFF);
        buffer.push_back((value >> 24) & 0xFF);
    }

    void write_string(const std::string& str) {
        int32_t len = static_cast<int32_t>(str.length());
        write_int(len);
        for (char c : str) {
            buffer.push_back(static_cast<uint8_t>(c));
        }
    }

    void write_short(int16_t value) {
        buffer.push_back((value >> 0) & 0xFF);
        buffer.push_back((value >> 8) & 0xFF);
    }

    void write_bytes(const uint8_t* data, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            buffer.push_back(data[i]);
        }
    }

    const std::vector<uint8_t>& get_buffer() const {
        return buffer;
    }

    void save_to_file(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "파일 열기 실패\n";
            return;
        }
        file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        file.close();
    }
};

class BinaryReader {
private:
    std::vector<uint8_t> buffer;
    size_t pos = 0;

public:
    BinaryReader(const std::vector<uint8_t>& buf) : buffer(buf) {}

    static BinaryReader load_from_file(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        std::vector<uint8_t> buf;
        if (file) {
            file.seekg(0, std::ios::end);
            size_t size = file.tellg();
            file.seekg(0);
            buf.resize(size);
            file.read(reinterpret_cast<char*>(buf.data()), size);
        }
        return BinaryReader(buf);
    }

    int32_t read_int() {
        if (pos + 4 > buffer.size()) return 0;
        int32_t value = (buffer[pos + 0] << 0) | (buffer[pos + 1] << 8) |
                        (buffer[pos + 2] << 16) | (buffer[pos + 3] << 24);
        pos += 4;
        return value;
    }

    int16_t read_short() {
        if (pos + 2 > buffer.size()) return 0;
        int16_t value = (buffer[pos + 0] << 0) | (buffer[pos + 1] << 8);
        pos += 2;
        return value;
    }

    std::string read_string() {
        int32_t len = read_int();
        if (len < 0 || pos + len > buffer.size()) return "";
        std::string str(buffer.begin() + pos, buffer.begin() + pos + len);
        pos += len;
        return str;
    }

    bool is_valid() const {
        return pos <= buffer.size();
    }
};

struct Point {
    int32_t x, y;

    Point(int32_t x = 0, int32_t y = 0) : x(x), y(y) {}

    void serialize(BinaryWriter& writer) const {
        writer.write_int(x);
        writer.write_int(y);
    }

    static Point deserialize(BinaryReader& reader) {
        return Point(reader.read_int(), reader.read_int());
    }
};

struct Person {
    std::string name;
    int32_t age;
    std::vector<int16_t> scores;

    Person(const std::string& n = "", int32_t a = 0)
        : name(n), age(a) {}

    void serialize(BinaryWriter& writer) const {
        writer.write_string(name);
        writer.write_int(age);
        writer.write_int(static_cast<int32_t>(scores.size()));
        for (int16_t score : scores) {
            writer.write_short(score);
        }
    }

    static Person deserialize(BinaryReader& reader) {
        Person p;
        p.name = reader.read_string();
        p.age = reader.read_int();
        int32_t count = reader.read_int();
        for (int32_t i = 0; i < count; ++i) {
            p.scores.push_back(reader.read_short());
        }
        return p;
    }
};

int main() {
    std::cout << "=== 이진 프로토콜 직렬화 테스트 ===" << std::endl;

    BinaryWriter writer;

    Point p1(10, 20);
    p1.serialize(writer);

    Person person("Alice", 20);
    person.scores = {85, 90, 78};
    person.serialize(writer);

    writer.save_to_file("data.bin");
    std::cout << "데이터 저장: data.bin (" << writer.get_buffer().size()
              << " bytes)" << std::endl;

    std::cout << "\n=== 이진 프로토콜 역직렬화 테스트 ===" << std::endl;

    BinaryReader reader = BinaryReader::load_from_file("data.bin");

    Point p2 = Point::deserialize(reader);
    std::cout << "Point: (" << p2.x << ", " << p2.y << ")" << std::endl;

    Person person2 = Person::deserialize(reader);
    std::cout << "Person: " << person2.name << ", age=" << person2.age
              << std::endl;
    std::cout << "Scores: ";
    for (int16_t score : person2.scores) {
        std::cout << score << " ";
    }
    std::cout << std::endl;

    return 0;
}
