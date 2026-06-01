#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <string>

struct Student {
    char name[64];
    int studentID;
    std::vector<double> scores;
};

const uint32_t MAGIC_VERSION = 1;

void serialize(const std::string& filename, const Student& s) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "파일 생성 실패\n";
        return;
    }

    uint32_t version = MAGIC_VERSION;
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));

    file.write(s.name, sizeof(s.name));
    file.write(reinterpret_cast<const char*>(&s.studentID), sizeof(s.studentID));

    size_t scoreCount = s.scores.size();
    file.write(reinterpret_cast<const char*>(&scoreCount), sizeof(scoreCount));

    for (double score : s.scores) {
        file.write(reinterpret_cast<const char*>(&score), sizeof(score));
    }

    if (file.fail()) {
        std::cerr << "파일 쓰기 실패\n";
    }
    file.close();
}

bool deserialize(const std::string& filename, Student& s) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "파일 열기 실패\n";
        return false;
    }

    uint32_t version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));

    if (version != MAGIC_VERSION) {
        std::cerr << "버전 불일치: " << version << "\n";
        return false;
    }

    file.read(s.name, sizeof(s.name));
    file.read(reinterpret_cast<char*>(&s.studentID), sizeof(s.studentID));

    size_t scoreCount;
    file.read(reinterpret_cast<char*>(&scoreCount), sizeof(scoreCount));

    s.scores.clear();
    for (size_t i = 0; i < scoreCount; ++i) {
        double score;
        file.read(reinterpret_cast<char*>(&score), sizeof(score));
        if (file.fail()) {
            std::cerr << "점수 읽기 실패\n";
            return false;
        }
        s.scores.push_back(score);
    }

    if (file.fail() && !file.eof()) {
        std::cerr << "파일 읽기 실패\n";
        return false;
    }
    file.close();
    return true;
}

int main() {
    Student original{"Charlie", 20241001, {85.0, 92.5, 78.0, 95.5}};

    std::cout << "=== 직렬화 ===\n";
    std::cout << "저장할 학생: " << original.name << " (ID: " << original.studentID << ")\n";
    std::cout << "점수: ";
    for (double s : original.scores) {
        std::cout << s << " ";
    }
    std::cout << "\n";
    serialize("student.bin", original);
    std::cout << "파일에 저장 완료\n\n";

    std::cout << "=== 역직렬화 ===\n";
    Student loaded{};
    if (deserialize("student.bin", loaded)) {
        std::cout << "복원된 학생: " << loaded.name << " (ID: " << loaded.studentID << ")\n";
        std::cout << "점수: ";
        for (double s : loaded.scores) {
            std::cout << s << " ";
        }
        std::cout << "\n";

        bool matches = (std::string(original.name) == loaded.name &&
                       original.studentID == loaded.studentID &&
                       original.scores.size() == loaded.scores.size());

        if (matches) {
            for (size_t i = 0; i < original.scores.size(); ++i) {
                if (original.scores[i] != loaded.scores[i]) {
                    matches = false;
                    break;
                }
            }
        }

        std::cout << "\n데이터 일치: " << (matches ? "성공 ✓" : "실패 ✗") << "\n";
    }

    return 0;
}
