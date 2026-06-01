#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>

class Person {
public:
    std::string name;
    int age;

    Person(const std::string& n = "", int a = 0) : name(n), age(a) {}
    virtual ~Person() = default;

    virtual std::string serialize() const = 0;
};

class Student : public Person {
public:
    std::string student_id;
    std::vector<int> scores;

    Student(const std::string& n = "", int a = 0, const std::string& id = "",
            const std::vector<int>& s = {})
        : Person(n, a), student_id(id), scores(s) {}

    std::string serialize() const override {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"type\": \"Student\",\n";
        oss << "  \"name\": \"" << name << "\",\n";
        oss << "  \"age\": " << age << ",\n";
        oss << "  \"student_id\": \"" << student_id << "\",\n";
        oss << "  \"scores\": [";
        for (size_t i = 0; i < scores.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << scores[i];
        }
        oss << "]\n";
        oss << "}";
        return oss.str();
    }
};

class SimpleStudent : public Person {
public:
    SimpleStudent(const std::string& n = "", int a = 0)
        : Person(n, a) {}

    std::string serialize() const override {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"type\": \"Person\",\n";
        oss << "  \"name\": \"" << name << "\",\n";
        oss << "  \"age\": " << age << "\n";
        oss << "}";
        return oss.str();
    }
};

Student deserialize_student(const std::string& json) {
    std::string name, student_id;
    int age = 0;
    std::vector<int> scores;

    size_t name_pos = json.find("\"name\": \"");
    if (name_pos != std::string::npos) {
        size_t start = name_pos + 10;
        size_t end = json.find("\"", start);
        name = json.substr(start, end - start);
    }

    size_t age_pos = json.find("\"age\": ");
    if (age_pos != std::string::npos) {
        age = std::stoi(json.substr(age_pos + 7));
    }

    size_t id_pos = json.find("\"student_id\": \"");
    if (id_pos != std::string::npos) {
        size_t start = id_pos + 15;
        size_t end = json.find("\"", start);
        student_id = json.substr(start, end - start);
    }

    size_t scores_pos = json.find("\"scores\": [");
    if (scores_pos != std::string::npos) {
        size_t start = scores_pos + 11;
        size_t end = json.find("]", start);
        std::string scores_str = json.substr(start, end - start);
        std::istringstream iss(scores_str);
        int score;
        char comma;
        while (iss >> score) {
            scores.push_back(score);
            iss >> comma;
        }
    }

    return Student(name, age, student_id, scores);
}

int main() {
    std::vector<std::unique_ptr<Person>> people;

    people.push_back(std::make_unique<Student>("Alice", 20, "S001",
                                               std::vector<int>{85, 90, 78}));
    people.push_back(std::make_unique<Student>("Bob", 21, "S002",
                                               std::vector<int>{92, 88, 95}));
    people.push_back(std::make_unique<SimpleStudent>("Charlie", 19));

    std::cout << "=== 직렬화된 데이터 ===" << std::endl;
    for (const auto& person : people) {
        std::cout << person->serialize() << std::endl << std::endl;
    }

    std::cout << "=== 역직렬화 테스트 ===" << std::endl;
    Student s1("Alice", 20, "S001", std::vector<int>{85, 90, 78});
    std::string json_data = s1.serialize();
    std::cout << "원본 JSON:\n" << json_data << std::endl << std::endl;

    Student s2 = deserialize_student(json_data);
    std::cout << "역직렬화된 Student:" << std::endl;
    std::cout << "  이름: " << s2.name << std::endl;
    std::cout << "  나이: " << s2.age << std::endl;
    std::cout << "  학번: " << s2.student_id << std::endl;
    std::cout << "  성적: ";
    for (int score : s2.scores) {
        std::cout << score << " ";
    }
    std::cout << std::endl;

    return 0;
}
