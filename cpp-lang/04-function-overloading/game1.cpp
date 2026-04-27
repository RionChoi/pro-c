#include <iostream>
#include <cstdlib>
#include <ctime>

int generateProblem(int& a, int& b, char& op) {
    op = std::rand() % 3;
    a = std::rand() % 50 + 1;
    b = std::rand() % 50 + 1;

    if (op == 0) {
        return a + b;
    } else if (op == 1) {
        if (a < b) {
            int temp = a;
            a = b;
            b = temp;
        }
        return a - b;
    } else {
        return a * b;
    }
}

int main(void) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    const int totalQuestions = 5;
    int score = 0;

    std::cout << "=== 계산기 게임 ===\n";
    std::cout << totalQuestions << "문제作答してください!\n\n";

    for (int i = 1; i <= totalQuestions; ++i) {
        int a, b, correctAnswer, userAnswer;
        char op, opSymbol;

        correctAnswer = generateProblem(a, b, op);

        if (op == 0) {
            opSymbol = '+';
        } else if (op == 1) {
            opSymbol = '-';
        } else {
            opSymbol = '*';
        }

        std::cout << "문제 " << i << "/" << totalQuestions << ": ";
        std::cout << a << " " << opSymbol << " " << b << " = ?\n";
        std::cout << "정답: ";

        if (!(std::cin >> userAnswer)) {
            std::cerr << "입력 오류\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (userAnswer == correctAnswer) {
            std::cout << "정답!\n";
            ++score;
        } else {
            std::cout << "오답! 정답은 " << correctAnswer << "입니다.\n";
        }
        std::cout << "\n";
    }

    std::cout << "=== 결과 ===\n";
    std::cout << "점수: " << score << "/" << totalQuestions << "\n";

    if (score == totalQuestions) {
        std::cout << " Perfecct! \n";
    } else if (score >= totalQuestions * 0.6) {
        std::cout << "Good job!\n";
    } else {
        std::cout << "다시 연습하세요!\n";
    }

    return 0;
}
