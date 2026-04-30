#include <iostream>
#include <cstring>

class BankAccount {
private:
    char accountNumber[32];
    double balance;
    char ownerName[64];

public:
    void setInfo(const char* accNum, const char* owner, double bal) {
        strncpy(accountNumber, accNum, sizeof(accountNumber) - 1);
        accountNumber[sizeof(accountNumber) - 1] = '\0';
        strncpy(ownerName, owner, sizeof(ownerName) - 1);
        ownerName[sizeof(ownerName) - 1] = '\0';
        balance = bal;
    }

    void deposit(double amount) {
        if (amount <= 0) {
            std::cerr << "입금 오류: 금액은 0보다 커야 합니다.\n";
            return;
        }
        balance += amount;
        std::cout << ownerName << " 계좌에 " << amount << "원이 입금되었습니다. (잔액: " << balance << "원)\n";
    }

    void withdraw(double amount) {
        if (amount <= 0) {
            std::cerr << "출금 오류: 금액은 0보다 커야 합니다.\n";
            return;
        }
        if (amount > balance) {
            std::cout << "출금 거절: 잔액이 부족합니다. (현재 잔액: " << balance << "원)\n";
            return;
        }
        balance -= amount;
        std::cout << ownerName << " 계좌에서 " << amount << "원이 출금되었습니다. (잔액: " << balance << "원)\n";
    }

    void display() const {
        std::cout << "--- 계좌 정보 ---\n";
        std::cout << "계좌번호: " << accountNumber << "\n";
        std::cout << "예금주: " << ownerName << "\n";
        std::cout << "잔액: " << balance << "원\n";
    }
};

int main() {
    std::cout << "=== 은행 계좌 시뮬레이션 ===\n\n";

    BankAccount acc1, acc2;
    acc1.setInfo("1001-2001", "김철수", 100000.0);
    acc2.setInfo("1002-2002", "이영희", 50000.0);

    // 계좌 1: 김철수
    acc1.display();
    acc1.deposit(30000.0);
    acc1.withdraw(50000.0);
    acc1.withdraw(100000.0); // 잔액 부족
    acc1.display();

    std::cout << "\n";

    // 계좌 2: 이영희
    acc2.display();
    acc2.deposit(20000.0);
    acc2.withdraw(30000.0);
    acc2.withdraw(50000.0); // 잔액 부족
    acc2.display();

    return 0;
}
