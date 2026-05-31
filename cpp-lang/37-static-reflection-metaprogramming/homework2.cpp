#include <iostream>
#include <tuple>
#include <string>
#include <type_traits>
#include <utility>

struct Student {
    std::string name;
    int age;
    float gpa;
};

struct Person {
    std::string first_name;
    std::string last_name;
    int id;
};

template <typename T>
class StructReflection {
public:
    template <typename Func>
    static void for_each_member(T& obj, Func func);

    static void print_type_info();
};

template <>
class StructReflection<Student> {
public:
    template <typename Func>
    static void for_each_member(Student& obj, Func func) {
        func("name", obj.name);
        func("age", obj.age);
        func("gpa", obj.gpa);
    }

    static void print_type_info() {
        std::cout << "=== Student 구조체 멤버 정보 ===" << std::endl;
        std::cout << "1. name (std::string)" << std::endl;
        std::cout << "2. age (int)" << std::endl;
        std::cout << "3. gpa (float)" << std::endl;
    }
};

template <>
class StructReflection<Person> {
public:
    template <typename Func>
    static void for_each_member(Person& obj, Func func) {
        func("first_name", obj.first_name);
        func("last_name", obj.last_name);
        func("id", obj.id);
    }

    static void print_type_info() {
        std::cout << "=== Person 구조체 멤버 정보 ===" << std::endl;
        std::cout << "1. first_name (std::string)" << std::endl;
        std::cout << "2. last_name (std::string)" << std::endl;
        std::cout << "3. id (int)" << std::endl;
    }
};

template <typename T>
void print_struct_members(T& obj) {
    std::cout << "\n=== 구조체 멤버 자동 출력 ===" << std::endl;
    auto printer = [](const char* name, const auto& value) {
        std::cout << "  " << name << " = ";
        if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<decltype(value)>>, std::string>) {
            std::cout << "\"" << value << "\"";
        } else {
            std::cout << value;
        }
        std::cout << std::endl;
    };

    StructReflection<T>::for_each_member(obj, printer);
}

template <typename T>
void print_member_count(T& obj) {
    std::cout << "\n=== 멤버 개수 ===" << std::endl;
    int count = 0;
    auto counter = [&count](const char*, const auto&) {
        count++;
    };
    StructReflection<T>::for_each_member(obj, counter);
    std::cout << "총 멤버 개수: " << count << std::endl;
}

int main() {
    Student student;
    student.name = "김철수";
    student.age = 22;
    student.gpa = 3.8f;

    Person person;
    person.first_name = "John";
    person.last_name = "Doe";
    person.id = 12345;

    std::cout << "=== 정적 리플렉션 데모 ===" << std::endl;

    StructReflection<Student>::print_type_info();
    print_struct_members(student);
    print_member_count(student);

    std::cout << "\n";
    StructReflection<Person>::print_type_info();
    print_struct_members(person);
    print_member_count(person);

    std::cout << "\n=== 멤버 값 수정 ===" << std::endl;
    auto modifier = [](const char* name, auto& value) {
        if constexpr (std::is_same_v<decltype(value), int>) {
            std::cout << "정수형 " << name << " 발견, 값에 10 추가" << std::endl;
            value += 10;
        }
    };

    std::cout << "수정 전: age=" << student.age << std::endl;
    StructReflection<Student>::for_each_member(student, modifier);
    std::cout << "수정 후: age=" << student.age << std::endl;

    return 0;
}
