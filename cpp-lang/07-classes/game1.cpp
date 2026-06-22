#include <iostream>
#include <cstring>
#include <ctime>
#include <cstdlib>

class Character {
private:
    char name[32];
    int  hp;
    int  maxHp;
    int  attack;

public:
    Character() : hp(0), maxHp(0), attack(0) { name[0] = '\0'; }

    void init(const char* n, int h, int atk) {
        strncpy(name, n, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        maxHp = h;
        hp    = h;
        attack = atk;
    }

    void attackEnemy(Character& enemy) {
        int dmg = attack + (rand() % 5); // 0~4 랜덤 보정치
        enemy.hp -= dmg;
        if (enemy.hp < 0) enemy.hp = 0;
        std::cout << name << "의 공격! " << enemy.name << "에게 " << dmg << " 데미지!\n";
    }

    void defend() {
        std::cout << name << "이(가) 방어 자세로 들어갑니다. (이번 턴 받는 데미지 감소)\n";
    }

    void heal() {
        int recover = 10 + (rand() % 10);
        hp += recover;
        if (hp > maxHp) hp = maxHp;
        std::cout << name << "이(가) 회복 마법을 사용! HP " << recover << " 회복! (현재 HP: " << hp << ")\n";
    }

    bool isAlive() const { return hp > 0; }

    void printStatus() const {
        std::cout << "[" << name << "] HP: " << hp << "/" << maxHp
                  << " | 공격력: " << attack << "\n";
    }

    const char* getName() const { return name; }
    int getHp() const { return hp; }
};

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::cout << "========================================\n";
    std::cout << "         클래스 RPG 전투\n";
    std::cout << "========================================\n\n";

    Character player, monster;
    player.init("영웅", 100, 15);
    monster.init(" goblin", 80, 10);

    player.printStatus();
    monster.printStatus();
    std::cout << "\n";

    int turn = 1;
    bool playerDefending = false;

    while (player.isAlive() && monster.isAlive()) {
        std::cout << "--- 턴 " << turn << " ---\n";

        std::cout << "행동을 선택하세요 (A: 공격, D: 방어, S: 회복): ";
        char input;
        std::cin >> input;

        playerDefending = false;

        switch (input) {
            case 'A':
            case 'a':
                player.attackEnemy(monster);
                break;
            case 'D':
            case 'd':
                player.defend();
                playerDefending = true;
                break;
            case 'S':
            case 's':
                player.heal();
                break;
            default:
                std::cout << "잘못된 입력! 공격으로 대체합니다.\n";
                player.attackEnemy(monster);
        }

        if (!monster.isAlive()) {
            std::cout << "\n" << monster.getName() << "이(가) 쓰러졌습니다! 전투 승리!\n";
            break;
        }

        // 몬스터 턴
        std::cout << monster.getName() << "의 공격!\n";
        int dmg = monster.getHp() > 0 ? 10 + (rand() % 5) : 0;
        if (playerDefending) {
            dmg /= 2;
            std::cout << "방어 효과! 데미지 감소! ";
        }
        std::cout << "영웅이 " << dmg << " 데미지를 입었습니다.\n";

        std::cout << "\n[현재 상태]\n";
        player.printStatus();
        monster.printStatus();
        std::cout << "\n";

        if (!player.isAlive()) {
            std::cout << "영웅이 쓰러졌습니다... 게임 오버!\n";
            break;
        }

        if (turn >= 10) {
            std::cout << "10턴이 지났습니다! 몬스터가 도망쳤습니다...\n";
            break;
        }

        ++turn;
    }

    std::cout << "\n========================================\n";
    std::cout << "           전투 종료\n";
    std::cout << "========================================\n";
    player.printStatus();
    monster.printStatus();

    return 0;
}
