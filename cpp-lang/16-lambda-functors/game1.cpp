#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <cstdlib>
#include <ctime>

struct Player {
    std::string name;
    int score = 0;
    int health = 100;
};

int main() {
    std::srand(std::time(nullptr));
    
    std::cout << "=== 람다 기반 게임 시스템 ===\n";
    std::cout << "플레이어 3명이 번갈아가며 주사위를 던지고 점수를 획득합니다.\n\n";
    
    // 플레이어 생성
    std::vector<Player> players = {
        {"플레이어1", 0, 100},
        {"플레이어2", 0, 100},
        {"플레이어3", 0, 100}
    };
    
    // 람다: 주사위 던지기 (1-6)
    auto rollDice = []() -> int {
        return (std::rand() % 6) + 1;
    };
    
    // 람다: 점수 획득
    auto addScore = [](Player& p, int points) -> void {
        p.score += points;
    };
    
    // 람다: 데미지
    auto takeDamage = [](Player& p, int damage) -> void {
        p.health -= damage;
        if (p.health < 0) p.health = 0;
    };
    
    // 람다: 플레이어 정보 출력
    auto displayPlayer = [](const Player& p) -> void {
        std::cout << "  " << p.name << ": 점수=" << p.score 
                  << ", 체력=" << p.health << "\n";
    };
    
    // 게임 루프 (3라운드)
    for (int round = 1; round <= 3; round++) {
        std::cout << "=== 라운드 " << round << " ===\n";
        
        for (Player& p : players) {
            int dice1 = rollDice();
            int dice2 = rollDice();
            int roll_sum = dice1 + dice2;
            
            std::cout << "\n" << p.name << "의 턴: " 
                      << dice1 << " + " << dice2 << " = " << roll_sum << "\n";
            
            // 람다: 조건부 보너스
            auto applyBonus = [](int score) -> int {
                return (score > 10) ? static_cast<int>(score * 1.5) : score;
            };
            
            int final_score = applyBonus(roll_sum);
            addScore(p, final_score);
            
            if (roll_sum > 10) {
                std::cout << "  보너스! 점수: " << final_score << "\n";
            } else {
                std::cout << "  점수: " << final_score << "\n";
            }
            
            // 람다: 랜덤 이벤트
            int event = std::rand() % 4;
            auto handleEvent = [&takeDamage](Player& player, int evt) -> void {
                if (evt == 0) {
                    takeDamage(player, 10);
                    std::cout << "  악운! -10 체력\n";
                } else if (evt == 1) {
                    std::cout << "  행운! 아무것도 발생하지 않았습니다.\n";
                }
            };
            
            handleEvent(p, event);
        }
        
        // 현재 상태 출력
        std::cout << "\n라운드 " << round << " 현황:\n";
        std::for_each(players.begin(), players.end(), displayPlayer);
    }
    
    // 최종 순위 (람다로 정렬)
    std::cout << "\n=== 최종 순위 ===\n";
    std::sort(players.begin(), players.end(),
              [](const Player& a, const Player& b) {
                  return a.score > b.score;
              });
    
    for (size_t i = 0; i < players.size(); i++) {
        std::cout << (i + 1) << "등: " << players[i].name 
                  << " (점수: " << players[i].score << ")\n";
    }
    
    std::cout << "\n게임 종료!\n";
    
    return 0;
}
