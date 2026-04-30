#include <iostream>
#include <memory>
#include <vector>

class Employee {
public:
    virtual ~Employee() = default;
    virtual double monthlyPay() const = 0;
    virtual const char* type() const = 0;
};

class FullTimeEmployee : public Employee {
private:
    double baseSalary;

public:
    explicit FullTimeEmployee(double salary) : baseSalary(salary) {}

    double monthlyPay() const override {
        return baseSalary;
    }

    const char* type() const override {
        return "FullTime";
    }
};

class Freelancer : public Employee {
private:
    double hourlyRate;
    int hoursWorked;

public:
    Freelancer(double rate, int hours) : hourlyRate(rate), hoursWorked(hours) {}

    double monthlyPay() const override {
        return hourlyRate * static_cast<double>(hoursWorked);
    }

    const char* type() const override {
        return "Freelancer";
    }
};

int main() {
    std::vector<std::unique_ptr<Employee>> workers;
    workers.push_back(std::make_unique<FullTimeEmployee>(3500.0));
    workers.push_back(std::make_unique<Freelancer>(40.0, 60));

    double total = 0.0;
    for (const auto& worker : workers) {
        const double pay = worker->monthlyPay();
        total += pay;
        std::cout << worker->type() << " pay = " << pay << '\n';
    }

    std::cout << "Total payroll = " << total << '\n';
    return 0;
}
