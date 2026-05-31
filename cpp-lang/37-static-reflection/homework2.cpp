#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

class Serializer {
public:
    Serializer() = default;

    template <typename T>
    void serialize(const T& value) {
        if constexpr (std::is_integral_v<T>) {
            buffer << value << " ";
        } else if constexpr (std::is_floating_point_v<T>) {
            buffer << value << " ";
        } else if constexpr (std::is_same_v<T, std::string>) {
            buffer << "\"" << value << "\" ";
        } else if constexpr (std::is_same_v<T, bool>) {
            buffer << (value ? "true" : "false") << " ";
        }
    }

    std::string get_buffer() const {
        return buffer.str();
    }

private:
    std::ostringstream buffer;
};

class Deserializer {
public:
    Deserializer(const std::string& data) : buffer(data), iss(buffer) {}

    template <typename T>
    bool deserialize(T& value) {
        if constexpr (std::is_integral_v<T>) {
            return (iss >> value).operator bool();
        } else if constexpr (std::is_floating_point_v<T>) {
            return (iss >> value).operator bool();
        } else if constexpr (std::is_same_v<T, std::string>) {
            char quote;
            if (!(iss >> quote) || quote != '"') return false;
            std::string result;
            std::getline(iss, result, '"');
            value = result;
            iss.peek();
            return true;
        } else if constexpr (std::is_same_v<T, bool>) {
            std::string token;
            if (!(iss >> token)) return false;
            value = (token == "true");
            return true;
        }
        return false;
    }

private:
    std::string buffer;
    std::istringstream iss;
};

struct Address {
    std::string city;
    std::string country;

    std::string serialize() const {
        Serializer s;
        s.serialize(city);
        s.serialize(country);
        return s.get_buffer();
    }

    bool deserialize(const std::string& data) {
        Deserializer d(data);
        return d.deserialize(city) && d.deserialize(country);
    }
};

struct Person {
    std::string name;
    int age;
    double height;
    Address address;
    bool is_active;

    std::string serialize() const {
        Serializer s;
        s.serialize(name);
        s.serialize(age);
        s.serialize(height);
        s.serialize(is_active);
        std::ostringstream oss;
        oss << s.get_buffer() << address.serialize();
        return oss.str();
    }

    bool deserialize(const std::string& data) {
        Deserializer d(data);
        std::string addr_part;
        if (!d.deserialize(name)) return false;
        if (!d.deserialize(age)) return false;
        if (!d.deserialize(height)) return false;
        if (!d.deserialize(is_active)) return false;

        std::istringstream iss(data);
        std::string token;
        for (int i = 0; i < 4; ++i) iss >> token;
        std::getline(iss, addr_part);

        return address.deserialize(addr_part);
    }

    void print() const {
        std::cout << "  이름: " << name << "\n";
        std::cout << "  나이: " << age << "\n";
        std::cout << "  키: " << height << "cm\n";
        std::cout << "  도시: " << address.city << "\n";
        std::cout << "  국가: " << address.country << "\n";
        std::cout << "  활성: " << (is_active ? "예" : "아니오") << "\n";
    }
};

int main() {
    std::cout << "=== 리플렉션 기반 제네릭 직렬화 ===\n\n";

    Person original{
        "Alice",
        28,
        165.5,
        {"Seoul", "Korea"},
        true
    };

    std::cout << "원본 데이터:\n";
    original.print();

    std::string serialized = original.serialize();
    std::cout << "\n직렬화된 데이터:\n";
    std::cout << serialized << "\n\n";

    Person loaded;
    if (loaded.deserialize(serialized)) {
        std::cout << "역직렬화된 데이터:\n";
        loaded.print();

        std::cout << "\n검증:\n";
        bool match = (original.name == loaded.name &&
                     original.age == loaded.age &&
                     original.height == loaded.height &&
                     original.is_active == loaded.is_active &&
                     original.address.city == loaded.address.city &&
                     original.address.country == loaded.address.country);
        std::cout << "  데이터 무결성: " << (match ? "✓ 일치" : "✗ 불일치") << "\n";
    } else {
        std::cerr << "역직렬화 실패\n";
    }

    return 0;
}
