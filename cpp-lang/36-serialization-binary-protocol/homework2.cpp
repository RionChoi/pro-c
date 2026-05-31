#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>

const uint32_t MAGIC_NUMBER = 0x50524F43;
const uint16_t VERSION = 2;

struct Grade {
    uint16_t subject_id;
    float score;
};

struct StudentRecord {
    char name[64];
    uint16_t age;
    std::vector<Grade> grades;
};

bool save_record(const std::string& filename, const StudentRecord& record) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "파일 열기 실패" << std::endl;
        return false;
    }

    file.write(reinterpret_cast<const char*>(&MAGIC_NUMBER), sizeof(MAGIC_NUMBER));
    file.write(reinterpret_cast<const char*>(&VERSION), sizeof(VERSION));

    file.write(record.name, sizeof(record.name));
    file.write(reinterpret_cast<const char*>(&record.age), sizeof(record.age));

    uint32_t grade_count = static_cast<uint32_t>(record.grades.size());
    file.write(reinterpret_cast<const char*>(&grade_count), sizeof(grade_count));

    for (const auto& grade : record.grades) {
        file.write(reinterpret_cast<const char*>(&grade.subject_id), sizeof(grade.subject_id));
        file.write(reinterpret_cast<const char*>(&grade.score), sizeof(grade.score));
    }

    if (!file) {
        std::cerr << "파일 쓰기 실패" << std::endl;
        return false;
    }

    file.close();
    return true;
}

bool load_record(const std::string& filename, StudentRecord& record) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "파일 열기 실패" << std::endl;
        return false;
    }

    uint32_t magic;
    uint16_t version;

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

    file.read(record.name, sizeof(record.name));
    file.read(reinterpret_cast<char*>(&record.age), sizeof(record.age));

    uint32_t grade_count;
    file.read(reinterpret_cast<char*>(&grade_count), sizeof(grade_count));

    record.grades.clear();
    for (uint32_t i = 0; i < grade_count; ++i) {
        Grade grade;
        file.read(reinterpret_cast<char*>(&grade.subject_id), sizeof(grade.subject_id));
        file.read(reinterpret_cast<char*>(&grade.score), sizeof(grade.score));
        record.grades.push_back(grade);
    }

    if (!file) {
        std::cerr << "파일 읽기 실패" << std::endl;
        return false;
    }

    file.close();
    return true;
}

int main() {
    StudentRecord record;
    std::strcpy(record.name, "박영희");
    record.age = 21;
    record.grades = {
        {1, 95.5f},
        {2, 88.0f},
        {3, 92.3f}
    };

    std::cout << "=== 복잡한 구조 직렬화 ===" << std::endl;
    if (save_record("record.bin", record)) {
        std::cout << "학생 레코드를 저장했습니다." << std::endl;
        std::cout << "이름: " << record.name << ", 나이: " << record.age << std::endl;
        std::cout << "과목 수: " << record.grades.size() << std::endl;
    }

    std::cout << "\n=== 복잡한 구조 역직렬화 ===" << std::endl;
    StudentRecord loaded_record;
    if (load_record("record.bin", loaded_record)) {
        std::cout << "학생 레코드를 로드했습니다." << std::endl;
        std::cout << "이름: " << loaded_record.name << ", 나이: " << loaded_record.age << std::endl;
        std::cout << "과목별 성적:" << std::endl;
        for (size_t i = 0; i < loaded_record.grades.size(); ++i) {
            std::cout << "  과목 " << loaded_record.grades[i].subject_id
                      << ": " << loaded_record.grades[i].score << std::endl;
        }
    }

    return 0;
}
