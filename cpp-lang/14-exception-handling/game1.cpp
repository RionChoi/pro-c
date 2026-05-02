#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// 커스텀 예외: 계좌 오류
class AccountException : public std::exception {
protected:
    std::string message;

public:
    AccountException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

class InsufficientFundsError : public AccountException {
public:
    InsufficientFundsError() : AccountException("잔액 부족") {}
};

class InvalidAmountError : public AccountException {
public:
    InvalidAmountError() : AccountException("잘못된 금액 (0 이상이어야 함)") {}
};

class InvalidAccountError : public AccountException {
public:
    InvalidAccountError() : AccountException("계정을 찾을 수 없음") {}
};

// 은행 계좌 클래스
class BankAccount {
private:
    std::string accountNumber;
    double balance;

public:
    BankAccount(const std::string& accNum, double initialBalance)
        : accountNumber(accNum), balance(initialBalance) {
        if (initialBalance < 0) {
            throw InvalidAmountError();
        }
    }

    void deposit(double amount) {
        if (amount <= 0) {
            throw InvalidAmountError();
        }
        balance += amount;
        std::cout << "입금: +" << amount << "원 | 잔액: " << balance << "원\n";
    }

    void withdraw(double amount) {
        if (amount <= 0) {
            throw InvalidAmountError();
        }
        if (amount > balance) {
            throw InsufficientFundsError();
        }
        balance -= amount;
        std::cout << "출금: -" << amount << "원 | 잔액: " << balance << "원\n";
    }

    double getBalance() const {
        return balance;
    }

    std::string getAccountNumber() const {
        return accountNumber;
    }
};

// ATM 시스템
class ATM {
private:
    std::vector<BankAccount> accounts;

public:
    void addAccount(const BankAccount& account) {
        accounts.push_back(account);
    }

    BankAccount* findAccount(const std::string& accNum) {
        for (auto& acc : accounts) {
            if (acc.getAccountNumber() == accNum) {
                return &acc;
            }
        }
        return nullptr;
    }

    void processTransaction(const std::string& accNum, 
                          const std::string& type, 
                          double amount) {
        try {
            BankAccount* account = findAccount(accNum);
            
            if (account == nullptr) {
                throw InvalidAccountError();
            }

            if (type == "deposit") {
                account->deposit(amount);
            } else if (type == "withdraw") {
                account->withdraw(amount);
            } else {
                throw std::invalid_argument("거래 유형 오류 (deposit/withdraw만 가능)");
            }

        } catch (const InsufficientFundsError& e) {
            std::cerr << "❌ " << e.what() << "\n";
        } catch (const InvalidAmountError& e) {
            std::cerr << "❌ " << e.what() << "\n";
        } catch (const InvalidAccountError& e) {
            std::cerr << "❌ " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cerr << "❌ 오류: " << e.what() << "\n";
        }
    }

    void displayAccount(const std::string& accNum) const {
        for (const auto& acc : accounts) {
            if (acc.getAccountNumber() == accNum) {
                std::cout << "계정: " << acc.getAccountNumber() 
                         << " | 잔액: " << acc.getBalance() << "원\n";
                return;
            }
        }
        std::cerr << "❌ 계정을 찾을 수 없음\n";
    }
};

int main() {
    std::cout << "=== 예외 처리를 활용한 ATM 시스템 ===\n\n";

    ATM atm;

    // 계좌 생성
    try {
        atm.addAccount(BankAccount("001", 50000));
        atm.addAccount(BankAccount("002", 100000));
        std::cout << "✓ 계정 생성 완료\n\n";
    } catch (const std::exception& e) {
        std::cerr << "계정 생성 오류: " << e.what() << "\n";
        return 1;
    }

    // 정상 거래
    std::cout << "--- 정상 거래 ---\n";
    atm.processTransaction("001", "deposit", 10000);
    atm.processTransaction("001", "withdraw", 5000);
    atm.displayAccount("001");
    std::cout << "\n";

    // 잔액 부족 거래
    std::cout << "--- 잔액 부족 거래 ---\n";
    atm.processTransaction("001", "withdraw", 100000);
    std::cout << "\n";

    // 잘못된 금액
    std::cout << "--- 잘못된 금액 거래 ---\n";
    atm.processTransaction("002", "deposit", -5000);
    atm.processTransaction("002", "withdraw", 0);
    std::cout << "\n";

    // 존재하지 않는 계정
    std::cout << "--- 존재하지 않는 계정 ---\n";
    atm.processTransaction("999", "deposit", 1000);
    std::cout << "\n";

    // 최종 계정 상태
    std::cout << "--- 최종 계정 상태 ---\n";
    atm.displayAccount("001");
    atm.displayAccount("002");

    std::cout << "\n✓ 프로그램 정상 종료\n";
    return 0;
}
