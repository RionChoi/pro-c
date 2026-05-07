// Observer 패턴: 주식 가격 알림 시스템
// Subject(StockMarket)의 상태가 바뀌면 등록된 모든 Observer에게 자동으로 통보한다.
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

// Observer 인터페이스 — 모든 구독자가 구현해야 하는 계약
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& ticker, double price) = 0;
    virtual std::string name() const = 0;
};

// Subject 인터페이스 — 구독 관리와 알림 발송 계약
class Subject {
public:
    virtual ~Subject() = default;
    virtual void subscribe(Observer* observer)   = 0;
    virtual void unsubscribe(Observer* observer) = 0;
    virtual void notify()                        = 0;
};

// 구체적인 Subject: 주식 시장
class StockMarket : public Subject {
public:
    void setPrice(const std::string& ticker, double price) {
        ticker_ = ticker;
        price_  = price;
        std::cout << "\n[시장] " << ticker_ << " 가격 변동 → "
                  << std::fixed << std::setprecision(2) << price_ << "원\n";
        notify();
    }

    void subscribe(Observer* observer) override {
        observers_.push_back(observer);
        std::cout << "[시장] '" << observer->name() << "' 구독 등록\n";
    }

    void unsubscribe(Observer* observer) override {
        observers_.erase(
            std::remove(observers_.begin(), observers_.end(), observer),
            observers_.end()
        );
        std::cout << "[시장] '" << observer->name() << "' 구독 해제\n";
    }

    void notify() override {
        for (Observer* obs : observers_) {
            obs->update(ticker_, price_);
        }
    }

private:
    std::string           ticker_;
    double                price_ = 0.0;
    std::vector<Observer*> observers_;
};

// 구체적인 Observer 1: 목표가 도달 시 매수 알림
class BuyAlert : public Observer {
public:
    BuyAlert(const std::string& name, double targetPrice)
        : name_(name), targetPrice_(targetPrice) {}

    void update(const std::string& ticker, double price) override {
        if (price <= targetPrice_) {
            std::cout << "  [" << name_ << "] 매수 신호! "
                      << ticker << " " << price << "원 ≤ 목표 " << targetPrice_ << "원\n";
        }
    }

    std::string name() const override { return name_; }

private:
    std::string name_;
    double      targetPrice_;
};

// 구체적인 Observer 2: 목표가 도달 시 매도 알림
class SellAlert : public Observer {
public:
    SellAlert(const std::string& name, double targetPrice)
        : name_(name), targetPrice_(targetPrice) {}

    void update(const std::string& ticker, double price) override {
        if (price >= targetPrice_) {
            std::cout << "  [" << name_ << "] 매도 신호! "
                      << ticker << " " << price << "원 ≥ 목표 " << targetPrice_ << "원\n";
        }
    }

    std::string name() const override { return name_; }

private:
    std::string name_;
    double      targetPrice_;
};

// 구체적인 Observer 3: 모든 변동을 기록하는 로거
class PriceLogger : public Observer {
public:
    void update(const std::string& ticker, double price) override {
        std::cout << "  [Logger] 기록: " << ticker << " @ "
                  << std::fixed << std::setprecision(2) << price << "원\n";
        history_.push_back(price);
    }

    std::string name() const override { return "Logger"; }

    void printHistory() const {
        std::cout << "\n[Logger] 가격 이력: ";
        for (double p : history_) {
            std::cout << std::fixed << std::setprecision(0) << p << " ";
        }
        std::cout << "\n";
    }

private:
    std::vector<double> history_;
};

int main() {
    std::cout << "--- Observer 패턴: 주식 가격 알림 시스템 ---\n";

    StockMarket market;
    BuyAlert    buyer("김철수", 48000.0);
    SellAlert   seller("이영희", 55000.0);
    PriceLogger logger;

    market.subscribe(&buyer);
    market.subscribe(&seller);
    market.subscribe(&logger);

    // 가격 변동 시뮬레이션
    market.setPrice("삼성전자", 52000.0);
    market.setPrice("삼성전자", 47500.0);
    market.setPrice("삼성전자", 56000.0);

    // 김철수 구독 해제 후 추가 변동
    std::cout << "\n";
    market.unsubscribe(&buyer);
    market.setPrice("삼성전자", 44000.0);

    logger.printHistory();

    return 0;
}
