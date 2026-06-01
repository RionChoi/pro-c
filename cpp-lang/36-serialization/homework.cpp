#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

class Student {
private:
    int id;
    std::string name;
    double gpa;

public:
    Student() : id(0), name(""), gpa(0.0) {}

    Student(int id_, const std::string& name_, double gpa_)
        : id(id_), name(name_), gpa(gpa_) {}

    int getId() const { return id; }
    std::string getName() const { return name; }
    double getGpa() const { return gpa; }

    // 객체를 JSON 문자열로 변환
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\"id\":" << id
            << ",\"name\":\"" << name
            << "\",\"gpa\":" << std::fixed << std::setprecision(2) << gpa
            << "}";
        return oss.str();
    }

    // JSON 문자열에서 객체 복원
    static Student fromJson(const std::string& json) {
        Student student;
        size_t pos = 0;

        // id 추출
        pos = json.find("\"id\":");
        if (pos != std::string::npos) {
            student.id = std::stoi(json.substr(pos + 5));
        }

        // name 추출
        pos = json.find("\"name\":\"");
        if (pos != std::string::npos) {
            size_t start = pos + 8;
            size_t end = json.find("\"", start);
            if (end != std::string::npos) {
                student.name = json.substr(start, end - start);
            }
        }

        // gpa 추출
        pos = json.find("\"gpa\":");
        if (pos != std::string::npos) {
            student.gpa = std::stod(json.substr(pos + 6));
        }

        return student;
    }

    void display() const {
        std::cout << "ID: " << id
                  << ", Name: " << name
                  << ", GPA: " << std::fixed << std::setprecision(2) << gpa << "\n";
    }
};

int main() {
    std::cout << "=== JSON 직렬화 테스트 ===\n\n";

    // 학생 객체 생성
    Student s1(1, "Alice", 3.8);
    Student s2(2, "Bob", 3.5);
    Student s3(3, "Charlie", 4.0);

    std::vector<Student> students = {s1, s2, s3};

    // 학생들을 JSON으로 직렬화하여 파일에 저장
    {
        std::ofstream outfile("students.json");
        outfile << "[";
        for (size_t i = 0; i < students.size(); ++i) {
            outfile << students[i].toJson();
            if (i < students.size() - 1) {
                outfile << ",";
            }
        }
        outfile << "]";
        std::cout << "학생 데이터를 students.json에 저장했습니다.\n\n";
    }

    // 파일에서 읽어 JSON 복원
    {
        std::ifstream infile("students.json");
        std::string content((std::istreambuf_iterator<char>(infile)),
                           std::istreambuf_iterator<char>());

        std::cout << "저장된 JSON 내용:\n";
        std::cout << content << "\n\n";

        // JSON 파싱 (간단한 버전)
        std::cout << "복원된 학생 정보:\n";
        size_t start = content.find("{");
        while (start != std::string::npos) {
            size_t end = content.find("}", start) + 1;
            std::string jsonObj = content.substr(start, end - start);
            Student restored = Student::fromJson(jsonObj);
            restored.display();
            start = content.find("{", end);
        }
    }

    return 0;
}
