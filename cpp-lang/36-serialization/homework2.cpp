#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cstdint>

class BinaryWriter {
    std::ofstream out_;

public:
    BinaryWriter(const std::string& filename)
        : out_(filename, std::ios::binary) {
        if (!out_) {
            throw std::runtime_error("파일을 열 수 없음: " + filename);
        }
    }

    template <typename T>
    void write(const T& value) {
        out_.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    void write_string(const std::string& str) {
        uint32_t len = str.length();
        write(len);
        out_.write(str.data(), len);
    }

    ~BinaryWriter() {
        if (out_) out_.close();
    }
};

class BinaryReader {
    std::ifstream in_;

public:
    BinaryReader(const std::string& filename)
        : in_(filename, std::ios::binary) {
        if (!in_) {
            throw std::runtime_error("파일을 열 수 없음: " + filename);
        }
    }

    template <typename T>
    void read(T& value) {
        in_.read(reinterpret_cast<char*>(&value), sizeof(T));
    }

    void read_string(std::string& str) {
        uint32_t len = 0;
        read(len);
        char* buffer = new char[len + 1];
        in_.read(buffer, len);
        buffer[len] = '\0';
        str = buffer;
        delete[] buffer;
    }

    ~BinaryReader() {
        if (in_) in_.close();
    }
};

struct Person {
    uint32_t id;
    std::string name;
    std::string email;
    double salary;
};

void save_people(const std::string& filename, const std::vector<Person>& people) {
    BinaryWriter writer(filename);
    writer.write(static_cast<uint32_t>(people.size()));

    for (const auto& p : people) {
        writer.write(p.id);
        writer.write_string(p.name);
        writer.write_string(p.email);
        writer.write(p.salary);
    }

    std::cout << "저장 완료: " << people.size() << " 명\n";
}

std::vector<Person> load_people(const std::string& filename) {
    std::vector<Person> people;
    BinaryReader reader(filename);

    uint32_t count = 0;
    reader.read(count);

    for (uint32_t i = 0; i < count; ++i) {
        Person p;
        reader.read(p.id);
        reader.read_string(p.name);
        reader.read_string(p.email);
        reader.read(p.salary);
        people.push_back(p);
    }

    std::cout << "로드 완료: " << people.size() << " 명\n";
    return people;
}

void display_people(const std::vector<Person>& people) {
    std::cout << "\n=== 직원 목록 ===\n";
    for (const auto& p : people) {
        std::cout << "[" << p.id << "] " << p.name << " (" << p.email
                  << ") 연봉: $" << p.salary << '\n';
    }
}

int main() {
    std::vector<Person> people = {
        {101, "Alice", "alice@example.com", 75000.0},
        {102, "Bob", "bob@example.com", 68000.0},
        {103, "Charlie", "charlie@example.com", 82000.0}
    };

    save_people("people.bin", people);
    auto loaded = load_people("people.bin");
    display_people(loaded);

    return 0;
}
