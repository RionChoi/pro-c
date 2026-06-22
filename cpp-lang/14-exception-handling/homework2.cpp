#include <iostream>
#include <stdexcept>
#include <string>

// 커스텀 예외 클래스 1: NegativeNumberException
class NegativeNumberException : public std::exception {
private:
    std::string message;

public:
    NegativeNumberException(const std::string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

// 커스텀 예외 클래스 2: InsufficientFundsException
class InsufficientFundsException : public std::exception {
private:
    double balance;
    double amount;

public:
    InsufficientFundsException(double b, double a)
        : balance(b), amount(a) {}

    const char* what() const noexcept override {
        return "잔액 부족 오류";
    }

    double getBalance() const { return balance; }
    double getAmount() const { return amount; }
};

int main() {
    // 학습: 커스텀 예외 클래스, 중첩된 예외 처리

    std::cout << "=== 커스텀 예외 처리 ===\n\n";

    // 1. NegativeNumberException 사용
    try {
        std::cout << "시도 1: 음수 검증\n";
        int number = -5;

        if (number < 0) {
            throw NegativeNumberException("음수는 허용되지 않습니다!");
        }

        std::cout << "유효한 수: " << number << "\n";

    } catch (const NegativeNumberException& e) {
        std::cerr << "커스텀 예외: " << e.what() << "\n\n";
    }

    // 2. InsufficientFundsException 사용
    try {
        std::cout << "시도 2: 계좌 인출\n";
        double balance = 1000.0;
        double withdraw = 1500.0;

        if (withdraw > balance) {
            throw InsufficientFundsException(balance, withdraw);
        }

        balance -= withdraw;
        std::cout << "인출 후 잔액: " << balance << "\n";

    } catch (const InsufficientFundsException& e) {
        std::cerr << "오류: " << e.what() << "\n";
        std::cerr << "현재 잔액: " << e.getBalance() << "\n";
        std::cerr << "요청 금액: " << e.getAmount() << "\n\n";
    }

    // 3. 중첩된 try-catch
    try {
        std::cout << "시도 3: 중첩된 예외 처리\n";

        try {
            throw NegativeNumberException("내부 오류");
        } catch (const NegativeNumberException& e) {
            std::cout << "내부에서 잡은 예외: " << e.what() << "\n";

            // 조건에 따라 예외 변환
            if (true) {
                throw std::runtime_error("외부 예외로 변환됨");
            }
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "변환된 예외: " << e.what() << "\n\n";
    }

    // 4. 예외 처리 순서 (구체적 → 추상적)
    try {
        std::cout << "시도 4: 예외 처리 계층\n";
        throw NegativeNumberException("계층 테스트");

    } catch (const NegativeNumberException& e) {
        std::cerr << "구체적 예외: " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "일반 예외: " << e.what() << "\n";
    }
    std::cout << "\n";

    // 5. 함수에서 예외 처리
    auto validateAge = [](int age) {
        if (age < 0 || age > 150) {
            throw std::invalid_argument("나이는 0~150 사이여야 합니다");
        }
        return age;
    };

    try {
        std::cout << "시도 5: 함수 내 예외\n";
        int age = validateAge(25);
        std::cout << "유효한 나이: " << age << "\n";
    } catch (const std::invalid_argument& e) {
        std::cerr << "유효성 검사 오류: " << e.what() << "\n";
    }
    std::cout << "\n";

    std::cout << "프로그램 정상 종료.\n";
    return 0;
}
