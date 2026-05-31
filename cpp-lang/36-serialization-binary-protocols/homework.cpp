#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>

struct Person {
    int age;
    char name[64];
    double salary;

    // 바이너리로 인코딩
    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buffer;

        // age (4 bytes)
        uint8_t *age_ptr = reinterpret_cast<uint8_t*>(const_cast<int*>(&age));
        buffer.insert(buffer.end(), age_ptr, age_ptr + sizeof(int));

        // name (64 bytes)
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(name),
                      reinterpret_cast<const uint8_t*>(name) + sizeof(name));

        // salary (8 bytes)
        uint8_t *salary_ptr = reinterpret_cast<uint8_t*>(const_cast<double*>(&salary));
        buffer.insert(buffer.end(), salary_ptr, salary_ptr + sizeof(double));

        return buffer;
    }

    // 바이너리에서 디코딩
    void deserialize(const std::vector<uint8_t>& buffer) {
        if (buffer.size() < sizeof(int) + sizeof(name) + sizeof(double)) {
            std::cerr << "버퍼 크기 부족\n";
            return;
        }

        size_t offset = 0;

        // age
        std::memcpy(&age, buffer.data() + offset, sizeof(int));
        offset += sizeof(int);

        // name
        std::memcpy(name, buffer.data() + offset, sizeof(name));
        offset += sizeof(name);

        // salary
        std::memcpy(&salary, buffer.data() + offset, sizeof(double));
    }
};

int main() {
    // Person 객체 생성
    Person p1{};
    p1.age = 30;
    std::strcpy(p1.name, "Alice");
    p1.salary = 50000.50;

    std::cout << "=== 원본 데이터 ===\n";
    std::cout << "나이: " << p1.age << "\n";
    std::cout << "이름: " << p1.name << "\n";
    std::cout << "급여: " << p1.salary << "\n\n";

    // 직렬화
    std::vector<uint8_t> serialized = p1.serialize();
    std::cout << "직렬화 크기: " << serialized.size() << " bytes\n\n";

    // 파일에 저장
    std::ofstream file("person.bin", std::ios::binary);
    file.write(reinterpret_cast<const char*>(serialized.data()), serialized.size());
    file.close();
    std::cout << "파일 저장: person.bin\n\n";

    // 파일에서 읽기
    std::ifstream in_file("person.bin", std::ios::binary);
    std::vector<uint8_t> file_buffer((std::istreambuf_iterator<char>(in_file)),
                                      std::istreambuf_iterator<char>());
    in_file.close();

    // 역직렬화
    Person p2{};
    p2.deserialize(file_buffer);

    std::cout << "=== 복원된 데이터 ===\n";
    std::cout << "나이: " << p2.age << "\n";
    std::cout << "이름: " << p2.name << "\n";
    std::cout << "급여: " << p2.salary << "\n";

    return 0;
}
