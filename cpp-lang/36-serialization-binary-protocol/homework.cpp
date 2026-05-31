#include <iostream>
#include <string>
#include <sstream>
#include <vector>

struct Student {
    std::string name;
    int student_id;
    double gpa;
};

class SerializerJSON {
public:
    std::string serialize(const Student& s) {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"name\": \"" << escape_json(s.name) << "\",\n";
        ss << "  \"student_id\": " << s.student_id << ",\n";
        ss << "  \"gpa\": " << s.gpa << "\n";
        ss << "}";
        return ss.str();
    }

    std::string serialize_array(const std::vector<Student>& students) {
        std::stringstream ss;
        ss << "[\n";
        for (size_t i = 0; i < students.size(); ++i) {
            ss << "  {\n";
            ss << "    \"name\": \"" << escape_json(students[i].name) << "\",\n";
            ss << "    \"student_id\": " << students[i].student_id << ",\n";
            ss << "    \"gpa\": " << students[i].gpa << "\n";
            ss << "  }";
            if (i < students.size() - 1) ss << ",";
            ss << "\n";
        }
        ss << "]";
        return ss.str();
    }

private:
    std::string escape_json(const std::string& s) {
        std::string result;
        for (char c : s) {
            if (c == '\"') result += "\\\"";
            else if (c == '\\') result += "\\\\";
            else if (c == '\n') result += "\\n";
            else if (c == '\r') result += "\\r";
            else if (c == '\t') result += "\\t";
            else result += c;
        }
        return result;
    }
};

class DeserializerJSON {
public:
    Student deserialize(const std::string& json) {
        Student s;

        size_t name_start = json.find("\"name\"") + 9;
        size_t name_end = json.find("\"", name_start + 1);
        s.name = json.substr(name_start + 1, name_end - name_start - 1);

        size_t id_start = json.find("\"student_id\"") + 14;
        s.student_id = std::stoi(json.substr(id_start));

        size_t gpa_start = json.find("\"gpa\"") + 7;
        s.gpa = std::stod(json.substr(gpa_start));

        return s;
    }
};

int main() {
    std::cout << "=== JSON Serialization 예제 ===\n\n";

    Student s1{"김철수", 2024001, 3.85};
    Student s2{"이영희", 2024002, 4.00};
    Student s3{"박민준", 2024003, 3.62};
    std::vector<Student> students{s1, s2, s3};

    SerializerJSON serializer;
    std::cout << "단일 학생 직렬화:\n";
    std::string json1 = serializer.serialize(s1);
    std::cout << json1 << "\n\n";

    std::cout << "배열 직렬화:\n";
    std::string json_array = serializer.serialize_array(students);
    std::cout << json_array << "\n\n";

    std::cout << "역직렬화 테스트:\n";
    DeserializerJSON deserializer;
    Student loaded = deserializer.deserialize(json1);
    std::cout << "이름: " << loaded.name << ", 학번: " << loaded.student_id
              << ", GPA: " << loaded.gpa << "\n";

    return 0;
}
