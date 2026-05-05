#include <iostream>
#include <memory>
#include <string>

class Resource {
public:
    int id;
    Resource(int i) : id(i) { std::cout << "Resource " << id << " created.\n"; }
    ~Resource() { std::cout << "Resource " << id << " destroyed.\n"; }
    void work() { std::cout << "Resource " << id << " is working.\n"; }
};

// 과제 1: std::unique_ptr을 사용하여 Resource 객체를 안전하게 생성하고 사용하세요.
void task_unique_ptr() {
    std::cout << "\n--- Task 1: unique_ptr ---\n";
    std::unique_ptr<Resource> res = std::make_unique<Resource>(101);
    res->work();
    std::cout << "unique_ptr released automatically when scope ends.\n";
}

int main() {
    task_unique_ptr();
    std::cout << "\nProgram finished. unique_ptr handles cleanup automatically.\n";
    return 0;
}