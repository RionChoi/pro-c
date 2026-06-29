#include "contract.hpp"

#include <iostream>
#include <string>
#include <utility>

class BankAccount {
public:
    BankAccount(std::string owner, int openingCents)
        : owner_(std::move(owner)),
          balanceCents_(openingCents)
    {
        CONTRACT_PRE_MSG(!owner_.empty(), "owner name must not be empty");
        CONTRACT_PRE_MSG(openingCents >= 0, "opening balance must be non-negative");
        assertInvariant();
    }

    void deposit(int cents)
    {
        CONTRACT_PRE_MSG(cents > 0, "deposit amount must be positive");
        const int before = balanceCents_;

        balanceCents_ += cents;

        CONTRACT_POST(balanceCents_ == before + cents);
        assertInvariant();
    }

    void withdraw(int cents)
    {
        CONTRACT_PRE_MSG(cents > 0, "withdrawal amount must be positive");
        CONTRACT_PRE_MSG(cents <= balanceCents_, "withdrawal must not overdraw");
        const int before = balanceCents_;

        balanceCents_ -= cents;

        CONTRACT_POST(balanceCents_ == before - cents);
        assertInvariant();
    }

    void transferTo(BankAccount& target, int cents)
    {
        CONTRACT_PRE_MSG(&target != this, "cannot transfer to self");
        CONTRACT_PRE_MSG(cents > 0, "transfer amount must be positive");
        CONTRACT_PRE_MSG(cents <= balanceCents_, "transfer must not overdraw");
        const int sourceBefore = balanceCents_;
        const int targetBefore = target.balanceCents_;

        withdraw(cents);
        target.deposit(cents);

        CONTRACT_POST(balanceCents_ == sourceBefore - cents);
        CONTRACT_POST(target.balanceCents_ == targetBefore + cents);
    }

    const std::string& owner() const
    {
        return owner_;
    }

    int balanceCents() const
    {
        return balanceCents_;
    }

private:
    void assertInvariant() const
    {
        CONTRACT_ASSERT_MSG(balanceCents_ >= 0, "account balance invariant failed");
    }

    std::string owner_;
    int balanceCents_;
};

std::string formatMoney(int cents)
{
    const char sign = cents < 0 ? '-' : '+';
    const int absolute = cents < 0 ? -cents : cents;
    const int whole = absolute / 100;
    const int fraction = absolute % 100;

    std::string result;
    result += sign;
    result += '$';
    result += std::to_string(whole);
    result += '.';
    if (fraction < 10) {
        result += '0';
    }
    result += std::to_string(fraction);
    return result;
}

void printAccount(const BankAccount& account)
{
    std::cout << account.owner() << " balance: " << formatMoney(account.balanceCents()) << '\n';
}

void reportViolation(const lesson_contracts::ContractViolation& error)
{
    const lesson_contracts::ViolationInfo& info = error.info();
    std::cout << "위반 종류: " << lesson_contracts::kindName(info.kind) << '\n';
    std::cout << "표현식: " << info.expression << '\n';
    std::cout << "메시지: " << info.message << '\n';
}

int main()
{
    std::cout << "native contracts available: "
              << (lesson_contracts::nativeContractsAvailable ? "yes" : "no") << "\n\n";

    BankAccount operating("운영 계좌", 250000);
    BankAccount savings("저축 계좌", 50000);

    operating.deposit(30000);
    operating.transferTo(savings, 80000);
    savings.withdraw(10000);

    printAccount(operating);
    printAccount(savings);

    try {
        operating.withdraw(999999);
    } catch (const lesson_contracts::ContractViolation& error) {
        std::cout << "\n계약 위반 포착\n";
        reportViolation(error);
    }

    return 0;
}
