#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <iomanip>

class Product {
private:
    uint32_t id;
    char name[64];
    double price;
    uint16_t quantity;

    static constexpr uint32_t MAGIC = 0x50524F44; // 'PROD'

public:
    Product() : id(0), price(0.0), quantity(0) {
        std::memset(name, 0, sizeof(name));
    }

    Product(uint32_t id_, const std::string& name_, double price_, uint16_t qty)
        : id(id_), price(price_), quantity(qty) {
        std::strncpy(this->name, name_.c_str(), sizeof(this->name) - 1);
        this->name[sizeof(this->name) - 1] = '\0';
    }

    // 객체를 이진 형식으로 변환
    std::vector<uint8_t> toBinary() const {
        std::vector<uint8_t> buffer;
        uint32_t nameLen = static_cast<uint32_t>(std::strlen(name));

        // 매직 넘버 (4바이트)
        buffer.push_back(static_cast<uint8_t>((MAGIC >> 24) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((MAGIC >> 16) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((MAGIC >> 8) & 0xFF));
        buffer.push_back(static_cast<uint8_t>(MAGIC & 0xFF));

        // ID (4바이트)
        buffer.push_back(static_cast<uint8_t>((id >> 24) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((id >> 16) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((id >> 8) & 0xFF));
        buffer.push_back(static_cast<uint8_t>(id & 0xFF));

        // 이름 길이 (1바이트)
        buffer.push_back(static_cast<uint8_t>(nameLen & 0xFF));

        // 이름 문자열
        for (uint32_t i = 0; i < nameLen; ++i) {
            buffer.push_back(static_cast<uint8_t>(name[i]));
        }

        // 가격 (8바이트, double)
        uint64_t priceBytes = *reinterpret_cast<const uint64_t*>(&price);
        buffer.push_back(static_cast<uint8_t>((priceBytes >> 56) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((priceBytes >> 48) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((priceBytes >> 40) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((priceBytes >> 32) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((priceBytes >> 24) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((priceBytes >> 16) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((priceBytes >> 8) & 0xFF));
        buffer.push_back(static_cast<uint8_t>(priceBytes & 0xFF));

        // 수량 (2바이트)
        buffer.push_back(static_cast<uint8_t>((quantity >> 8) & 0xFF));
        buffer.push_back(static_cast<uint8_t>(quantity & 0xFF));

        return buffer;
    }

    // 이진 형식에서 객체 복원
    static Product fromBinary(const std::vector<uint8_t>& buffer) {
        Product product;

        if (buffer.size() < 18) {
            std::cerr << "버퍼 크기 부족\n";
            return product;
        }

        size_t pos = 0;

        // 매직 넘버 검증
        uint32_t magic = (static_cast<uint32_t>(buffer[0]) << 24) |
                        (static_cast<uint32_t>(buffer[1]) << 16) |
                        (static_cast<uint32_t>(buffer[2]) << 8) |
                        static_cast<uint32_t>(buffer[3]);
        if (magic != MAGIC) {
            std::cerr << "잘못된 매직 넘버\n";
            return product;
        }
        pos = 4;

        // ID 복원
        product.id = (static_cast<uint32_t>(buffer[pos]) << 24) |
                    (static_cast<uint32_t>(buffer[pos + 1]) << 16) |
                    (static_cast<uint32_t>(buffer[pos + 2]) << 8) |
                    static_cast<uint32_t>(buffer[pos + 3]);
        pos += 4;

        // 이름 길이
        uint32_t nameLen = buffer[pos++];

        // 이름 복원
        std::memset(product.name, 0, sizeof(product.name));
        for (uint32_t i = 0; i < nameLen && pos < buffer.size(); ++i) {
            product.name[i] = static_cast<char>(buffer[pos++]);
        }

        // 가격 복원
        if (pos + 8 <= buffer.size()) {
            uint64_t priceBytes = (static_cast<uint64_t>(buffer[pos]) << 56) |
                                 (static_cast<uint64_t>(buffer[pos + 1]) << 48) |
                                 (static_cast<uint64_t>(buffer[pos + 2]) << 40) |
                                 (static_cast<uint64_t>(buffer[pos + 3]) << 32) |
                                 (static_cast<uint64_t>(buffer[pos + 4]) << 24) |
                                 (static_cast<uint64_t>(buffer[pos + 5]) << 16) |
                                 (static_cast<uint64_t>(buffer[pos + 6]) << 8) |
                                 static_cast<uint64_t>(buffer[pos + 7]);
            product.price = *reinterpret_cast<const double*>(&priceBytes);
            pos += 8;
        }

        // 수량 복원
        if (pos + 2 <= buffer.size()) {
            product.quantity = (static_cast<uint16_t>(buffer[pos]) << 8) |
                              static_cast<uint16_t>(buffer[pos + 1]);
        }

        return product;
    }

    void display() const {
        std::cout << "ID: " << id
                  << ", Name: " << name
                  << ", Price: $" << std::fixed << std::setprecision(2) << price
                  << ", Qty: " << quantity << "\n";
    }
};

int main() {
    std::cout << "=== 이진 프로토콜 직렬화 테스트 ===\n\n";

    // 제품 객체 생성
    Product p1(101, "Laptop", 999.99, 5);
    Product p2(102, "Mouse", 29.99, 50);
    Product p3(103, "Keyboard", 79.99, 30);

    std::vector<Product> products = {p1, p2, p3};

    // 이진 형식으로 파일에 저장
    {
        std::ofstream outfile("products.bin", std::ios::binary);
        for (const auto& prod : products) {
            auto binary = prod.toBinary();
            outfile.write(reinterpret_cast<const char*>(binary.data()), binary.size());
        }
        std::cout << "제품 데이터를 products.bin에 저장했습니다.\n";
        std::cout << "총 저장된 제품: " << products.size() << "\n\n";
    }

    // 파일에서 이진 형식으로 읽어 복원
    {
        std::ifstream infile("products.bin", std::ios::binary);
        std::cout << "복원된 제품 정보:\n";

        while (infile.good()) {
            std::vector<uint8_t> buffer(100);
            // 헤더 읽기: magic(4) + id(4) + nameLen(1) = 9 바이트
            infile.read(reinterpret_cast<char*>(buffer.data()), 9);
            if (infile.gcount() < 9) break;

            // 이름 길이 가져오기
            uint8_t nameLen = buffer[8];
            // 나머지 읽기: name(nameLen) + price(8) + quantity(2)
            uint32_t remainingSize = nameLen + 8 + 2;
            infile.read(reinterpret_cast<char*>(buffer.data() + 9), remainingSize);
            if (infile.gcount() < static_cast<std::streamsize>(remainingSize)) break;

            uint32_t totalSize = 9 + remainingSize;
            buffer.resize(totalSize);
            Product restored = Product::fromBinary(buffer);
            restored.display();
        }
    }

    return 0;
}
