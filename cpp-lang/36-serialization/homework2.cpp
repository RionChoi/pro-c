#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <cstdint>

class ScoreRecord {
private:
    uint32_t studentId;
    std::vector<uint32_t> scores;

public:
    ScoreRecord() : studentId(0) {}

    ScoreRecord(uint32_t id, const std::vector<uint32_t>& scr)
        : studentId(id), scores(scr) {}

    void serialize(std::ofstream& file) const {
        file.write(reinterpret_cast<const char*>(&studentId), sizeof(studentId));

        uint32_t scoreCount = static_cast<uint32_t>(scores.size());
        file.write(reinterpret_cast<const char*>(&scoreCount), sizeof(scoreCount));

        for (uint32_t score : scores) {
            file.write(reinterpret_cast<const char*>(&score), sizeof(score));
        }
    }

    bool deserialize(std::ifstream& file) {
        file.read(reinterpret_cast<char*>(&studentId), sizeof(studentId));
        if (!file) return false;

        uint32_t scoreCount;
        file.read(reinterpret_cast<char*>(&scoreCount), sizeof(scoreCount));
        if (!file) return false;

        scores.clear();
        scores.reserve(scoreCount);

        for (uint32_t i = 0; i < scoreCount; i++) {
            uint32_t score;
            file.read(reinterpret_cast<char*>(&score), sizeof(score));
            if (!file) return false;
            scores.push_back(score);
        }

        return true;
    }

    uint32_t getStudentId() const { return studentId; }
    const std::vector<uint32_t>& getScores() const { return scores; }

    uint32_t getAverage() const {
        if (scores.empty()) return 0;
        uint32_t sum = 0;
        for (uint32_t score : scores) {
            sum += score;
        }
        return sum / static_cast<uint32_t>(scores.size());
    }
};

int main() {
    const char* filename = "scores.bin";

    // 저장
    {
        std::ofstream out(filename, std::ios::binary);
        if (!out) {
            std::cerr << "파일 열기 실패\n";
            return 1;
        }

        ScoreRecord r1(101, {85, 90, 88});
        ScoreRecord r2(102, {92, 87, 95, 91});

        r1.serialize(out);
        r2.serialize(out);
        out.close();

        std::cout << "저장 완료\n";
    }

    // 불러오기
    {
        std::ifstream in(filename, std::ios::binary);
        if (!in) {
            std::cerr << "파일 열기 실패\n";
            return 1;
        }

        ScoreRecord r;
        while (r.deserialize(in)) {
            std::cout << "학생 ID: " << r.getStudentId() << ", 평균: " << r.getAverage() << "\n";
        }

        in.close();
    }

    return 0;
}
