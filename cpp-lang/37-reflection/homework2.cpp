#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

struct MemberInfo {
    std::string name;
    std::string type;
};

struct ReflectedType {
    std::string type_name;
    std::vector<MemberInfo> members;
};

class Reflection {
private:
    static std::unordered_map<std::string, ReflectedType>& registry() {
        static std::unordered_map<std::string, ReflectedType> reg;
        return reg;
    }

public:
    static void register_type(const std::string& type_name, const std::vector<MemberInfo>& members) {
        registry()[type_name] = {type_name, members};
    }

    static const ReflectedType* get_type(const std::string& type_name) {
        auto it = registry().find(type_name);
        if (it != registry().end()) {
            return &it->second;
        }
        return nullptr;
    }

    static void print_type_info(const std::string& type_name) {
        const auto* info = get_type(type_name);
        if (!info) {
            std::cout << "타입 '" << type_name << "'은(는) 등록되지 않았습니다.\n";
            return;
        }

        std::cout << "[" << info->type_name << "]\n";
        std::cout << "  멤버 수: " << info->members.size() << "\n";
        for (const auto& member : info->members) {
            std::cout << "    - " << member.name << " (" << member.type << ")\n";
        }
    }

    static void list_all_types() {
        const auto& reg = registry();
        std::cout << "등록된 타입 개수: " << reg.size() << "\n";
        for (const auto& [name, _] : reg) {
            std::cout << "  - " << name << "\n";
        }
    }
};

struct Person {
    std::string name;
    int age;
    double height;
};

struct Book {
    std::string title;
    std::string author;
    int year;
    double price;
};

struct Student {
    std::string id;
    std::string name;
    int semester;
    double gpa;
    std::string major;
};

int main() {
    std::cout << "=== 런타임 리플렉션 시스템 ===\n\n";

    Reflection::register_type("Person", {
        {"name", "std::string"},
        {"age", "int"},
        {"height", "double"}
    });

    Reflection::register_type("Book", {
        {"title", "std::string"},
        {"author", "std::string"},
        {"year", "int"},
        {"price", "double"}
    });

    Reflection::register_type("Student", {
        {"id", "std::string"},
        {"name", "std::string"},
        {"semester", "int"},
        {"gpa", "double"},
        {"major", "std::string"}
    });

    std::cout << "[등록된 모든 타입]\n";
    Reflection::list_all_types();

    std::cout << "\n[Person 타입 정보]\n";
    Reflection::print_type_info("Person");

    std::cout << "\n[Book 타입 정보]\n";
    Reflection::print_type_info("Book");

    std::cout << "\n[Student 타입 정보]\n";
    Reflection::print_type_info("Student");

    std::cout << "\n[등록되지 않은 타입 조회]\n";
    Reflection::print_type_info("Animal");

    std::cout << "\n[리플렉션 데이터 활용]\n";
    const auto* person_info = Reflection::get_type("Person");
    if (person_info) {
        Person p{"철수", 25, 1.75};
        std::cout << person_info->type_name << " 인스턴스 생성:\n";
        std::cout << "  name: " << p.name << "\n";
        std::cout << "  age: " << p.age << "\n";
        std::cout << "  height: " << p.height << "\n";

        std::cout << "\n멤버 정보로부터 검증:\n";
        for (const auto& member : person_info->members) {
            std::cout << "  - " << member.name << " 타입 " << member.type << " 존재함\n";
        }
    }

    std::cout << "\n✓ 런타임 리플렉션 완료\n";
    return 0;
}
