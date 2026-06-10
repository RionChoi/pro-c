#include <iostream>
#include <string>
#include <vector>

class Pet {
private:
    std::string name;
    int hunger;
    int happiness;

public:
    // 생성자: 펫 탄생 및 초기 상태 설정
    Pet(std::string n) : name(n), hunger(50), happiness(50) {
        std::cout << "\n✨ " << name << "(이)가 태어났습니다!\n";
        std::cout << "  (배고픔: " << hunger << ", 행복도: " << happiness << ")\n";
    }

    // 소멸자: 펫과의 이별
    ~Pet() {
        std::cout << "\n🌈 " << name << "(이)가 무지개 다리를 건넜습니다...\n";
        std::cout << "  최종 상태 - 배고픔: " << hunger << ", 행복도: " << happiness << "\n";
        std::cout << "  " << name << "와(과) 함께한 시간은 즐거웠습니다. 안녕!\n";
    }

    void feed() {
        std::cout << "🍗 " << name << "에게 밥을 주었습니다.\n";
        hunger = (hunger > 10) ? hunger - 10 : 0;
        happiness += 5;
    }

    void play() {
        std::cout << "🎾 " << name << "와(과) 함께 놀았습니다.\n";
        happiness += 10;
        hunger += 5;
    }

    void status() const {
        std::cout << "\n[" << name << "의 현재 상태]\n";
        std::cout << "  배고픔: " << hunger << " (낮을수록 좋음)\n";
        std::cout << "  행복도: " << happiness << " (높을수록 좋음)\n";
    }
};

int main() {
    std::string petName;
    std::cout << "=== 펫 키우기 시뮬레이션 ===\n";
    std::cout << "펫의 이름을 지어주세요: ";
    std::getline(std::cin, petName);

    {
        Pet myPet(petName);
        int choice;
        bool running = true;

        while (running) {
            myPet.status();
            std::cout << "\n1. 밥 주기  2. 같이 놀기  3. 작별 인사하기\n선택: ";
            if (!(std::cin >> choice)) {
                break;
            }

            switch (choice) {
                case 1: myPet.feed(); break;
                case 2: myPet.play(); break;
                case 3: running = false; break;
                default: std::cout << "잘못된 선택입니다.\n";
            }
        }
        std::cout << "\n펫과의 시간을 종료합니다.\n";
    } // 블록이 끝나며 myPet 객체의 소멸자 호출

    std::cout << "\n프로그램이 완전히 종료되었습니다.\n";
    return 0;
}
