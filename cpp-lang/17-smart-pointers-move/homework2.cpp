#include <iostream>
#include <memory>
#include <string>

class SharedResource {
public:
    int id;
    SharedResource(int i) : id(i) { std::cout << "SharedResource " << id << " created.\n"; }
    ~SharedResource() { std::cout << "SharedResource " << id << " destroyed.\n"; }
};

// 과제 2: std::shared_ptr을 사용하여 두 개 이상의 포인터가 하나의 자원을 공유하게 하고, 참조 카운트가 0이 될 때 파괴되는지 확인하세요.
void task_shared_ptr() {
    std::cout << "\n--- Task 2: shared_ptr ---\n";
    std::shared_ptr<SharedResource> ptr1 = std::make_shared<SharedResource>(202);
    std::cout << "Initial count: " << ptr1.use_count() << "\n";
    
    std::shared_ptr<SharedResource> ptr2 = ptr1; // Count increases
    std::cout << "After copy, count: " << ptr1.use_count() << "\n";
    
    // ptr1 범위를 벗어나도 ptr2가 살아있기 때문에 파괴되지 않습니다.
    {
        std::shared_ptr<SharedResource> ptr3 = ptr1; // Count increases again
        std::cout << "Inside scope, count: " << ptr1.use_count() << "\n";
    } // ptr3 is destroyed, count decreases
    
    std::cout << "After inner scope, count: " << ptr1.use_count() << "\n";
    
}

int main() {
    task_shared_ptr();
    std::cout << "\nProgram finished. shared_ptr handles cleanup safely.\n";
    return 0;
}