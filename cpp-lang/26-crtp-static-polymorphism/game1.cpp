// 세션 26: CRTP & 정적 다형성
// game1.cpp — CRTP 전사 계층을 이용한 텍스트 RPG 전투
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <limits>

// ==================================================
// CRTP 기반 전사 클래스
// 파생 클래스별 attack_impl / special_impl / job_impl
// defending_impl / clear_def_impl 을 정적으로 디스패치
// ==================================================

template <typename Derived>
class Warrior {
protected:
    std::string name_;
    int         hp_, max_hp_, atk_;

public:
    Warrior(std::string n, int hp, int atk)
        : name_(std::move(n)), hp_(hp), max_hp_(hp), atk_(atk) {}

    int         normal_attack()     { return static_cast<Derived*>(this)->attack_impl(); }
    int         special_action()    { return static_cast<Derived*>(this)->special_impl(); }
    const char* job()         const { return static_cast<const Derived*>(this)->job_impl(); }
    const char* special_desc()const { return static_cast<const Derived*>(this)->spec_desc_impl(); }
    bool        is_defending()const { return static_cast<const Derived*>(this)->defending_impl(); }
    void        clear_defense()     { static_cast<Derived*>(this)->clear_def_impl(); }

    void take_damage(int dmg) { hp_ = std::max(0, hp_ - dmg); }
    void restore(int amount)  { hp_ = std::min(max_hp_, hp_ + amount); }
    bool is_alive()     const { return hp_ > 0; }
    const std::string& name() const { return name_; }
    int  hp()           const { return hp_; }
    int  max_hp()       const { return max_hp_; }

    void print_status() const {
        int filled = (max_hp_ > 0) ? hp_ * 20 / max_hp_ : 0;
        std::cout << "[" << name_ << "/" << job() << "] HP:[";
        for (int i = 0; i < 20; ++i)
            std::cout << (i < filled ? '#' : '.');
        std::cout << "] " << hp_ << "/" << max_hp_ << '\n';
    }
};

// ==================================================
// 직업 클래스
// ==================================================

// 기사 — 방어 기술로 다음 공격 피해 절반
class Knight : public Warrior<Knight> {
    bool defending_ = false;
public:
    explicit Knight(const std::string& n) : Warrior(n, 150, 20) {}
    int         attack_impl()     { defending_ = false; return atk_ + std::rand() % 10; }
    int         special_impl()    { defending_ = true;  return 0; }
    const char* job_impl()  const { return "기사"; }
    const char* spec_desc_impl() const { return "방어 태세! (다음 피해 절반)"; }
    bool        defending_impl() const { return defending_; }
    void        clear_def_impl()       { defending_ = false; }
};

// 마법사 — 고화력, 마나 관리 필요
class Mage : public Warrior<Mage> {
    int mana_ = 100;
public:
    explicit Mage(const std::string& n) : Warrior(n, 80, 35) {}
    int attack_impl() {
        if (mana_ >= 20) { mana_ -= 20; return atk_ + std::rand() % 20; }
        return 8;
    }
    int special_impl() {
        mana_ = std::min(100, mana_ + 40);
        return 0;
    }
    const char* job_impl()       const { return "마법사"; }
    const char* spec_desc_impl() const { return "마나 충전! (+40 마나)"; }
    bool        defending_impl() const { return false; }
    void        clear_def_impl()       {}
    int mana() const { return mana_; }
};

// 도적 — 낮은 HP, 30% 확률 3배 치명타
class Rogue : public Warrior<Rogue> {
public:
    explicit Rogue(const std::string& n) : Warrior(n, 100, 25) {}
    int attack_impl() {
        if (std::rand() % 10 < 3) return atk_ * 3;
        return atk_ + std::rand() % 5;
    }
    int         special_impl()    { restore(15); return 0; }
    const char* job_impl()  const { return "도적"; }
    const char* spec_desc_impl() const { return "회복! (+15 HP)"; }
    bool        defending_impl() const { return false; }
    void        clear_def_impl()       {}
};

// 적 AI
class Enemy : public Warrior<Enemy> {
public:
    Enemy(const std::string& n, int hp, int atk) : Warrior(n, hp, atk) {}
    int         attack_impl()    { return atk_ + std::rand() % 8; }
    int         special_impl()   { return 0; }
    const char* job_impl()  const { return "몬스터"; }
    const char* spec_desc_impl() const { return ""; }
    bool        defending_impl() const { return false; }
    void        clear_def_impl()       {}
};

// ==================================================
// 입력 유틸리티
// ==================================================

static int prompt_int(const char* msg, int lo, int hi) {
    while (true) {
        std::cout << msg;
        int v = 0;
        if (std::cin >> v && v >= lo && v <= hi) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return v;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "잘못된 입력. " << lo << "~" << hi << " 사이로 입력하세요.\n";
    }
}

// ==================================================
// 전투 루프 (Player 타입은 CRTP로 컴파일 타임에 결정)
// ==================================================

template <typename Player>
void battle(Player& player) {
    Enemy enemy{"고블린 용사", 100, 15};

    std::cout << "\n=== 전투 시작! ===\n\n";

    int round = 1;
    while (player.is_alive() && enemy.is_alive()) {
        std::cout << "--- 라운드 " << round++ << " ---\n";
        player.print_status();
        enemy.print_status();

        std::cout << "\n행동 선택:\n";
        std::cout << "  1. 일반 공격\n";
        std::cout << "  2. 특수 행동 (" << player.special_desc() << ")\n";
        int act = prompt_int("선택 (1-2): ", 1, 2);

        int player_dmg = 0;
        if (act == 1) {
            player_dmg = player.normal_attack();
            std::cout << player.name() << " 공격 → " << player_dmg << " 데미지!\n";
        } else {
            player.special_action();
            std::cout << player.name() << " 특수 행동: " << player.special_desc() << '\n';
        }

        // 적 공격
        int enemy_dmg = enemy.normal_attack();

        // 방어 처리 (CRTP — 가상 함수 없이 컴파일 타임 분기)
        if (player.is_defending()) {
            enemy_dmg /= 2;
            std::cout << enemy.name() << " 공격 → " << enemy_dmg << " 데미지! (방어 절반)\n";
            player.clear_defense();
        } else {
            std::cout << enemy.name() << " 공격 → " << enemy_dmg << " 데미지!\n";
        }

        if (player_dmg > 0) enemy.take_damage(player_dmg);
        player.take_damage(enemy_dmg);

        std::cout << '\n';

        if (round > 30) {
            std::cout << "전투가 너무 길어졌습니다. 무승부!\n";
            return;
        }
    }

    if (player.is_alive())
        std::cout << "★★★ " << player.name() << " 승리! ★★★\n";
    else
        std::cout << "✕ " << player.name() << " 패배...\n";
}

// ==================================================
// main
// ==================================================

int main() {
    std::srand(static_cast<unsigned int>(
        static_cast<unsigned long long>(std::time(nullptr)) & 0xFFFFFFFFULL));

    std::cout << "╔════════════════════════════════╗\n";
    std::cout << "║   CRTP 정적 다형성 RPG 전투    ║\n";
    std::cout << "╚════════════════════════════════╝\n\n";

    std::cout << "직업 선택:\n";
    std::cout << "  1. 기사   (HP:150 ATK:20~29  특수:방어)\n";
    std::cout << "  2. 마법사  (HP:80  ATK:35~54  특수:마나충전)\n";
    std::cout << "  3. 도적   (HP:100 ATK:25~75  특수:체력회복)\n";

    int job = prompt_int("직업 (1-3): ", 1, 3);

    std::cout << "이름: ";
    std::string pname;
    std::getline(std::cin, pname);
    if (pname.empty()) pname = "용사";

    if (job == 1) {
        Knight p{pname};
        battle(p);
    } else if (job == 2) {
        Mage p{pname};
        battle(p);
    } else {
        Rogue p{pname};
        battle(p);
    }

    return 0;
}
