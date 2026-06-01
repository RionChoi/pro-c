#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdint>
#include <cstring>

struct Person {
    int age;
    double score;
    std::string name;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buf;

        // age (4바이트)
        buf.push_back((age >> 0) & 0xFF);
        buf.push_back((age >> 8) & 0xFF);
        buf.push_back((age >> 16) & 0xFF);
        buf.push_back((age >> 24) & 0xFF);

        // score (8바이트 double)
        const uint8_t* score_ptr = reinterpret_cast<const uint8_t*>(&score);
        buf.insert(buf.end(), score_ptr, score_ptr + sizeof(double));

        // name (길이 + 문자열)
        uint32_t name_len = name.size();
        buf.push_back((name_len >> 0) & 0xFF);
        buf.push_back((name_len >> 8) & 0xFF);
        buf.push_back((name_len >> 16) & 0xFF);
        buf.push_back((name_len >> 24) & 0xFF);
        buf.insert(buf.end(), name.begin(), name.end());

        return buf;
    }

    static Person deserialize(const std::vector<uint8_t>& buf, size_t& pos) {
        Person p;

        if (pos + 4 > buf.size()) throw std::runtime_error("버퍼 오버플로우: age");
        p.age = buf[pos] | (buf[pos+1] << 8) | (buf[pos+2] << 16) | (buf[pos+3] << 24);
        pos += 4;

        if (pos + 8 > buf.size()) throw std::runtime_error("버퍼 오버플로우: score");
        p.score = *reinterpret_cast<const double*>(&buf[pos]);
        pos += 8;

        if (pos + 4 > buf.size()) throw std::runtime_error("버퍼 오버플로우: name_len");
        uint32_t name_len = buf[pos] | (buf[pos+1] << 8) | (buf[pos+2] << 16) | (buf[pos+3] << 24);
        pos += 4;

        if (pos + name_len > buf.size()) throw std::runtime_error("버퍼 오버플로우: name");
        p.name = std::string(buf.begin() + pos, buf.begin() + pos + name_len);
        pos += name_len;

        return p;
    }
};

int main() {
    std::cout << "=== 직렬화 및 역직렬화 테스트 ===\n\n";

    // 원본 데이터
    Person original{42, 95.5, "철수"};
    std::cout << "원본 데이터:\n";
    std::cout << "  이름: " << original.name << "\n";
    std::cout << "  나이: " << original.age << "\n";
    std::cout << "  점수: " << original.score << "\n\n";

    // 직렬화
    std::vector<uint8_t> serialized = original.serialize();
    std::cout << "직렬화된 바이트 수: " << serialized.size() << "\n";
    std::cout << "바이트 데이터 (16진수): ";
    for (size_t i = 0; i < serialized.size() && i < 32; ++i) {
        printf("%02X ", serialized[i]);
    }
    std::cout << "\n\n";

    // 파일에 저장
    std::ofstream out("person.bin", std::ios::binary);
    if (!out) {
        std::cerr << "파일 저장 실패\n";
        return 1;
    }
    out.write(reinterpret_cast<const char*>(serialized.data()), serialized.size());
    out.close();
    std::cout << "파일에 저장됨: person.bin\n\n";

    // 파일에서 읽기
    std::ifstream in("person.bin", std::ios::binary);
    if (!in) {
        std::cerr << "파일 읽기 실패\n";
        return 1;
    }
    std::vector<uint8_t> loaded((std::istreambuf_iterator<char>(in)),
                                 std::istreambuf_iterator<char>());
    in.close();

    // 역직렬화
    size_t pos = 0;
    Person restored = Person::deserialize(loaded, pos);

    std::cout << "복원된 데이터:\n";
    std::cout << "  이름: " << restored.name << "\n";
    std::cout << "  나이: " << restored.age << "\n";
    std::cout << "  점수: " << restored.score << "\n\n";

    // 검증
    bool match = (original.name == restored.name &&
                  original.age == restored.age &&
                  original.score == restored.score);

    if (match) {
        std::cout << "✓ 직렬화/역직렬화 성공: 데이터 일치\n";
        return 0;
    } else {
        std::cout << "✗ 직렬화/역직렬화 실패: 데이터 불일치\n";
        return 1;
    }
}
