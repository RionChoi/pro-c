#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

struct HighScore {
    std::string name;
    int score;
};

int main() {
    std::srand(std::time(nullptr));
    std::string scorefile = "highscores.txt";
    
    std::cout << "=== 숫자 맞추기 게임 ===\n";
    std::cout << "1. 새 게임 플레이\n";
    std::cout << "2. 최고 점수 보기\n";
    std::cout << "선택 (1 또는 2): ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    
    if (choice == 1) {
        // 새 게임
        int secret = (std::rand() % 100) + 1;
        int attempts = 0;
        int guess = 0;
        int score = 1000;
        
        std::cout << "\n1부터 100 사이의 숫자를 맞춰보세요!\n";
        
        while (guess != secret) {
            std::cout << "추측: ";
            std::cin >> guess;
            
            if (std::cin.fail()) {
                std::cerr << "숫자를 입력하세요.\n";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }
            
            attempts++;
            score -= 10;
            
            if (guess < secret) {
                std::cout << "↑ 더 큰 숫자입니다.\n";
            } else if (guess > secret) {
                std::cout << "↓ 더 작은 숫자입니다.\n";
            } else {
                std::cout << "\n축하합니다! " << attempts << "번 시도 끝에 맞혔습니다.\n";
                std::cout << "최종 점수: " << score << "\n";
            }
        }
        
        // 점수 파일에 추가
        std::cout << "\n이름을 입력하세요: ";
        std::string name;
        std::getline(std::cin, name);
        
        std::ofstream outfile(scorefile, std::ios::app);
        if (outfile.is_open()) {
            outfile << name << "," << score << "\n";
            outfile.close();
            std::cout << "점수가 저장되었습니다!\n";
        }
        
    } else if (choice == 2) {
        // 최고 점수 보기
        std::ifstream infile(scorefile);
        
        if (!infile.is_open()) {
            std::cout << "저장된 점수가 없습니다.\n";
            return 0;
        }
        
        std::cout << "\n=== 최고 점수 ===\n";
        std::cout << "이름            | 점수\n";
        std::cout << "-------------------\n";
        
        std::string line;
        int rank = 1;
        while (std::getline(infile, line)) {
            size_t commapos = line.find(',');
            if (commapos != std::string::npos) {
                std::string name = line.substr(0, commapos);
                std::string score_str = line.substr(commapos + 1);
                
                std::cout << name << " | " << score_str << "\n";
                rank++;
            }
        }
        
        infile.close();
    }
    
    return 0;
}
