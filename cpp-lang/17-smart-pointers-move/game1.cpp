#include <iostream>
#include <vector>
#include <utility> // for std::move

class DataPacket {
public:
    std::vector<int> data;
    DataPacket(std::vector<int> d) : data(std::move(d)) {
        std::cout << "DataPacket created. Size: " << data.size() << "\n";
    }
    // 이동 생성자 (Move Constructor)
    DataPacket(DataPacket&& other) noexcept : data(std::move(other.data)) {
        std::cout << "DataPacket MOVED. Size: " << data.size() << "\n";
        other.data.clear(); // Original object is now empty
    }
    ~DataPacket() {
        std::cout << "DataPacket destroyed. Size: " << data.size() << "\n";
    }
};

void run_game() {
    std::cout << "\n--- Game 1: Move Semantics ---\n";
    
    std::vector<int> raw_data = {1, 2, 3, 4, 5};
    // 여기서 move가 발생합니다. 자원 복사 대신 효율적으로 자원을 이전합니다.
    DataPacket packet(std::move(raw_data)); 
}

int main() {
    run_game();
    std::cout << "\nGame 1 finished.\n";
    return 0;
}