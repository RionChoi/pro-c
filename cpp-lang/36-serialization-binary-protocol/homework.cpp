#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <iomanip>

const uint32_t MAGIC_NUMBER = 0x50524F43; // "PROC"
const uint16_t VERSION = 1;

struct Student {
    char name[64];
    uint16_t age;
    float score;
};

uint32_t crc32_simple(const uint8_t* data, size_t len) {
    uint32_t crc = 0xFFFFFFFFU;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j) {
            crc = (crc >> 1) ^ (0xEDB88320U & -(crc & 1));
        }
    }
    return crc ^ 0xFFFFFFFFU;
}

bool save_student(const std::string& filename, const Student& student) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "파일 열기 실패: " << filename << std::endl;
        return false;
    }

    file.write(reinterpret_cast<const char*>(&MAGIC_NUMBER), sizeof(MAGIC_NUMBER));
    file.write(reinterpret_cast<const char*>(&VERSION), sizeof(VERSION));

    uint32_t data_size = sizeof(Student);
    file.write(reinterpret_cast<const char*>(&data_size), sizeof(data_size));

    file.write(reinterpret_cast<const char*>(&student), sizeof(Student));

    uint32_t checksum = crc32_simple(reinterpret_cast<const uint8_t*>(&student), sizeof(Student));
    file.write(reinterpret_cast<const char*>(&checksum), sizeof(checksum));

    if (!file) {
        std::cerr << "파일 쓰기 실패" << std::endl;
        return false;
    }

    file.close();
    return true;
}

bool load_student(const std::string& filename, Student& student) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "파일 열기 실패: " << filename << std::endl;
        return false;
    }

    uint32_t magic;
    uint16_t version;
    uint32_t data_size;
    uint32_t checksum;

    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (magic != MAGIC_NUMBER) {
        std::cerr << "유효하지 않은 매직 넘버" << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (version != VERSION) {
        std::cerr << "호환되지 않는 버전" << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(&data_size), sizeof(data_size));
    if (data_size != sizeof(Student)) {
        std::cerr << "데이터 크기 불일치" << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(&student), sizeof(Student));
    file.read(reinterpret_cast<char*>(&checksum), sizeof(checksum));

    uint32_t calculated = crc32_simple(reinterpret_cast<const uint8_t*>(&student), sizeof(Student));
    if (calculated != checksum) {
        std::cerr << "체크섬 검증 실패 (손상된 파일)" << std::endl;
        return false;
    }

    if (!file) {
        std::cerr << "파일 읽기 실패" << std::endl;
        return false;
    }

    file.close();
    return true;
}

int main() {
    Student s1;
    std::strcpy(s1.name, "김철수");
    s1.age = 22;
    s1.score = 95.5f;

    std::cout << "=== 이진 직렬화 저장 ===" << std::endl;
    if (save_student("student.bin", s1)) {
        std::cout << "학생 정보를 저장했습니다." << std::endl;
        std::cout << "이름: " << s1.name << ", 나이: " << s1.age << ", 점수: " << s1.score << std::endl;
    }

    std::cout << "\n=== 이진 역직렬화 로드 ===" << std::endl;
    Student s2;
    if (load_student("student.bin", s2)) {
        std::cout << "학생 정보를 로드했습니다." << std::endl;
        std::cout << "이름: " << s2.name << ", 나이: " << s2.age << ", 점수: " << s2.score << std::endl;
    }

    return 0;
}
