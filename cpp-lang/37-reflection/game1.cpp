#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>
#include <algorithm>

class Monster {
private:
    std::string name;
    int hp;
    int level;

public:
    Monster(const std::string& n, int h, int l) : name(n), hp(h), level(l) {}

    void display() const {
        std::cout << "  몬스터: " << name << " (HP: " << hp << ", 레벨: " << level << ")\n";
    }

    int get_hp() const { return hp; }
    int get_level() const { return level; }
    std::string get_name() const { return name; }
};

class Item {
private:
    std::string name;
    std::string type;
    int quantity;

public:
    Item(const std::string& n, const std::string& t, int q) : name(n), type(t), quantity(q) {}

    void display() const {
        std::cout << "  아이템: " << name << " (" << type << ") x" << quantity << "\n";
    }

    std::string get_name() const { return name; }
    std::string get_type() const { return type; }
    int get_quantity() const { return quantity; }
};

class Weapon {
private:
    std::string name;
    int attack;
    int durability;

public:
    Weapon(const std::string& n, int a, int d) : name(n), attack(a), durability(d) {}

    void display() const {
        std::cout << "  무기: " << name << " (공격력: " << attack << ", 내구도: " << durability << ")\n";
    }

    std::string get_name() const { return name; }
    int get_attack() const { return attack; }
    int get_durability() const { return durability; }
};

template <typename T>
class TypeSafeContainer {
private:
    std::vector<T> objects;
    std::string type_name;

public:
    TypeSafeContainer() {
        type_name = typeid(T).name();
    }

    void add(const T& obj) {
        objects.push_back(obj);
    }

    void add_multiple(const std::vector<T>& objs) {
        objects.insert(objects.end(), objs.begin(), objs.end());
    }

    size_t count() const {
        return objects.size();
    }

    void display_all() const {
        std::cout << "[" << get_type_display_name() << " - 총 " << objects.size() << "개]\n";
        for (const auto& obj : objects) {
            obj.display();
        }
    }

    const T& get(size_t index) const {
        return objects[index];
    }

    std::string get_type_display_name() const {
        if (type_name.find("Monster") != std::string::npos) return "몬스터";
        if (type_name.find("Item") != std::string::npos) return "아이템";
        if (type_name.find("Weapon") != std::string::npos) return "무기";
        return type_name;
    }

    bool is_type_of(const std::string& check_type) const {
        if (check_type == "Monster") return type_name.find("Monster") != std::string::npos;
        if (check_type == "Item") return type_name.find("Item") != std::string::npos;
        if (check_type == "Weapon") return type_name.find("Weapon") != std::string::npos;
        return false;
    }

    void clear() {
        objects.clear();
    }
};

int main() {
    std::cout << "=== 타입 안전 게임 오브젝트 관리 ===\n\n";

    TypeSafeContainer<Monster> monsters;
    TypeSafeContainer<Item> items;
    TypeSafeContainer<Weapon> weapons;

    std::cout << "[게임 오브젝트 생성]\n";

    monsters.add(Monster("골렘", 150, 5));
    monsters.add(Monster("드래곤", 500, 15));
    monsters.add(Monster("스켈레톤", 50, 3));

    items.add(Item("포션", "소비", 5));
    items.add(Item("스크롤", "소비", 3));
    items.add(Item("보석", "재료", 2));

    weapons.add(Weapon("대검", 45, 100));
    weapons.add(Weapon("활", 30, 80));
    weapons.add(Weapon("지팡이", 35, 50));

    std::cout << "\n[오브젝트 수량]\n";
    std::cout << "몬스터: " << monsters.count() << "개\n";
    std::cout << "아이템: " << items.count() << "개\n";
    std::cout << "무기: " << weapons.count() << "개\n";

    std::cout << "\n[오브젝트 목록]\n";
    monsters.display_all();
    std::cout << "\n";
    items.display_all();
    std::cout << "\n";
    weapons.display_all();

    std::cout << "\n[타입 검증]\n";
    std::cout << "몬스터 컨테이너가 Monster 타입: " << (monsters.is_type_of("Monster") ? "✓" : "✗") << "\n";
    std::cout << "몬스터 컨테이너가 Item 타입: " << (monsters.is_type_of("Item") ? "✓" : "✗") << "\n";
    std::cout << "아이템 컨테이너가 Item 타입: " << (items.is_type_of("Item") ? "✓" : "✗") << "\n";
    std::cout << "무기 컨테이너가 Weapon 타입: " << (weapons.is_type_of("Weapon") ? "✓" : "✗") << "\n";

    std::cout << "\n[개별 오브젝트 접근]\n";
    const auto& boss = monsters.get(1);
    std::cout << "2번째 몬스터:\n";
    boss.display();

    const auto& main_weapon = weapons.get(0);
    std::cout << "주무기:\n";
    main_weapon.display();

    std::cout << "\n[게임 진행 시뮬레이션]\n";
    std::cout << "전투 준비: 몬스터 1체 선택, 무기 1개 선택\n";
    const auto& enemy = monsters.get(0);
    const auto& weapon = weapons.get(0);

    std::cout << "적 - ";
    enemy.display();
    std::cout << "무기 - ";
    weapon.display();

    std::cout << "\n[타입 안전성 검증]\n";
    std::cout << "몬스터 컨테이너: " << monsters.count() << "개 몬스터 안전 저장\n";
    std::cout << "아이템 컨테이너: " << items.count() << "개 아이템 안전 저장\n";
    std::cout << "무기 컨테이너: " << weapons.count() << "개 무기 안전 저장\n";

    std::cout << "\n✓ 타입 안전 게임 오브젝트 관리 완료\n";
    return 0;
}
