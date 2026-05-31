#include <iostream>
#include <tuple>
#include <string>
#include <functional>

template <typename T, typename U>
struct Member {
    std::string name;
    U T::*ptr;

    Member(const std::string& n, U T::*p) : name(n), ptr(p) {}
};

template <typename T>
struct ClassInfo;

struct Student {
    std::string name;
    int student_id;
    double gpa;
};

template <>
struct ClassInfo<Student> {
    using Type = Student;
    using Members = std::tuple<
        Member<Student, std::string>,
        Member<Student, int>,
        Member<Student, double>
    >;

    static Members get_members() {
        return std::make_tuple(
            Member<Student, std::string>("name", &Student::name),
            Member<Student, int>("student_id", &Student::student_id),
            Member<Student, double>("gpa", &Student::gpa)
        );
    }
};

template <typename T, typename F, size_t... I>
void for_each_impl(T& obj, const typename ClassInfo<T>::Members& members, F func,
                   std::index_sequence<I...>) {
    (..., func(obj, std::get<I>(members)));
}

template <typename T, typename F>
void for_each_member(T& obj, F func) {
    auto members = ClassInfo<T>::get_members();
    for_each_impl(obj, members, func, std::make_index_sequence<3>{});
}

void print_member_values() {
    std::cout << "=== 구조체 멤버 리플렉션 ===\n\n";

    Student s{"김철수", 2024001, 3.85};

    std::cout << "학생 정보 (리플렉션으로 출력):\n";
    for_each_member(s, [](const Student& obj, const auto& member) {
        std::cout << member.name << ": ";
        if (member.name == "name") {
            std::cout << (obj.*member.ptr) << "\n";
        } else if (member.name == "student_id") {
            std::cout << (obj.*member.ptr) << "\n";
        } else if (member.name == "gpa") {
            std::cout << (obj.*member.ptr) << "\n";
        }
    });
}

struct Player {
    std::string name;
    int hp;
    int level;
};

template <>
struct ClassInfo<Player> {
    using Type = Player;
    using Members = std::tuple<
        Member<Player, std::string>,
        Member<Player, int>,
        Member<Player, int>
    >;

    static Members get_members() {
        return std::make_tuple(
            Member<Player, std::string>("name", &Player::name),
            Member<Player, int>("hp", &Player::hp),
            Member<Player, int>("level", &Player::level)
        );
    }
};

template <typename T>
T clone_object(const T& obj) {
    T copy = obj;
    return copy;
}

template <typename T>
bool objects_equal(const T& a, const T& b) {
    if (a.name != b.name) return false;
    if (a.hp != b.hp) return false;
    if (a.level != b.level) return false;
    return true;
}

void pattern_matching_example() {
    std::cout << "\n=== 패턴 매칭 예제 ===\n\n";

    Player p1{"용사", 100, 10};
    Player p2 = clone_object(p1);
    p2.hp = 50;

    std::cout << "플레이어 1 복제 완료\n";
    std::cout << "p1과 p2가 같은가? " << (objects_equal(p1, p2) ? "예" : "아니오") << "\n";
    std::cout << "p1: " << p1.name << ", HP: " << p1.hp << ", 레벨: " << p1.level << "\n";
    std::cout << "p2: " << p2.name << ", HP: " << p2.hp << ", 레벨: " << p2.level << "\n";
}

struct Monster {
    std::string name;
    int attack;
    int defense;
};

template <>
struct ClassInfo<Monster> {
    using Type = Monster;
    using Members = std::tuple<
        Member<Monster, std::string>,
        Member<Monster, int>,
        Member<Monster, int>
    >;

    static Members get_members() {
        return std::make_tuple(
            Member<Monster, std::string>("name", &Monster::name),
            Member<Monster, int>("attack", &Monster::attack),
            Member<Monster, int>("defense", &Monster::defense)
        );
    }
};

int main() {
    print_member_values();
    pattern_matching_example();

    std::cout << "\n=== 몬스터 정보 ===\n";
    Monster m{"고블린", 15, 5};
    std::cout << "몬스터: " << m.name << "\n";
    std::cout << "공격력: " << m.attack << "\n";
    std::cout << "방어력: " << m.defense << "\n";

    return 0;
}
