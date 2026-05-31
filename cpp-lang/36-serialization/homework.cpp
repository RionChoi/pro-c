#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

struct Student {
    std::string name;
    int age;
    double gpa;
    std::vector<int> scores;
};

std::string studentToJSON(const Student& s) {
    std::ostringstream oss;
    oss << "{\n"
        << "  \"name\": \"" << s.name << "\",\n"
        << "  \"age\": " << s.age << ",\n"
        << "  \"gpa\": " << std::fixed << std::setprecision(2) << s.gpa << ",\n"
        << "  \"scores\": [";

    for (size_t i = 0; i < s.scores.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << s.scores[i];
    }
    oss << "]\n}";
    return oss.str();
}

bool studentFromJSON(const std::string& json, Student& s) {
    std::istringstream iss(json);
    std::string line;
    s.scores.clear();

    while (std::getline(iss, line)) {
        if (line.find("\"name\":") != std::string::npos) {
            size_t start = line.find('"', line.find('"') + 1) + 1;
            size_t end = line.find('"', start);
            s.name = line.substr(start, end - start);
        } else if (line.find("\"age\":") != std::string::npos) {
            sscanf(line.c_str(), "  \"age\": %d", &s.age);
        } else if (line.find("\"gpa\":") != std::string::npos) {
            sscanf(line.c_str(), "  \"gpa\": %lf", &s.gpa);
        } else if (line.find("\"scores\":") != std::string::npos) {
            size_t start = line.find('[') + 1;
            size_t end = line.find(']');
            std::string scores_str = line.substr(start, end - start);
            std::istringstream scores_iss(scores_str);
            int score;
            while (scores_iss >> score) {
                s.scores.push_back(score);
                if (scores_iss.peek() == ',') scores_iss.ignore();
            }
        }
    }
    return true;
}

int main() {
    std::cout << "=== JSON 직렬화 데모 ===\n\n";

    Student original{"Alice", 20, 3.85, {85, 90, 88}};
    std::cout << "원본 학생:\n";
    std::cout << "이름: " << original.name << ", 나이: " << original.age
              << ", GPA: " << original.gpa << "\n";
    std::cout << "점수: ";
    for (int score : original.scores) std::cout << score << " ";
    std::cout << "\n\n";

    std::string json = studentToJSON(original);
    std::cout << "JSON 형식:\n" << json << "\n\n";

    std::ofstream out("student.json");
    out << json;
    out.close();
    std::cout << "student.json 파일에 저장되었습니다.\n\n";

    std::ifstream in("student.json");
    std::ostringstream buffer;
    buffer << in.rdbuf();
    in.close();

    Student loaded;
    studentFromJSON(buffer.str(), loaded);
    std::cout << "로드한 학생:\n";
    std::cout << "이름: " << loaded.name << ", 나이: " << loaded.age
              << ", GPA: " << loaded.gpa << "\n";
    std::cout << "점수: ";
    for (int score : loaded.scores) std::cout << score << " ";
    std::cout << "\n";

    return 0;
}
