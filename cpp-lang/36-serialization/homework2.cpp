#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

struct VersionedData {
    int version;
    std::string name;
    int age;
    // version 2에서 추가
    double salary;
    std::string department;

    std::vector<uint8_t> serialize(int v) const {
        std::vector<uint8_t> buf;

        // 버전 헤더 (1바이트)
        buf.push_back(v & 0xFF);

        // 버전 1 필드
        uint32_t name_len = name.size();
        buf.push_back((name_len >> 0) & 0xFF);
        buf.push_back((name_len >> 8) & 0xFF);
        buf.insert(buf.end(), name.begin(), name.end());

        buf.push_back((age >> 0) & 0xFF);
        buf.push_back((age >> 8) & 0xFF);
        buf.push_back((age >> 16) & 0xFF);
        buf.push_back((age >> 24) & 0xFF);

        // 버전 2 필드
        if (v >= 2) {
            const uint8_t* salary_ptr = reinterpret_cast<const uint8_t*>(&salary);
            buf.insert(buf.end(), salary_ptr, salary_ptr + sizeof(double));

            uint32_t dept_len = department.size();
            buf.push_back((dept_len >> 0) & 0xFF);
            buf.push_back((dept_len >> 8) & 0xFF);
            buf.insert(buf.end(), department.begin(), department.end());
        }

        return buf;
    }

    static VersionedData deserialize(const std::vector<uint8_t>& buf, size_t& pos) {
        VersionedData d{0, "", 0, 0.0, ""};

        if (pos >= buf.size()) throw std::runtime_error("버퍼 오버플로우: 버전");
        d.version = buf[pos];
        pos++;

        // 버전 1 필드
        if (pos + 2 > buf.size()) throw std::runtime_error("버퍼 오버플로우: name_len");
        uint32_t name_len = buf[pos] | (buf[pos+1] << 8);
        pos += 2;

        if (pos + name_len > buf.size()) throw std::runtime_error("버퍼 오버플로우: name");
        d.name = std::string(buf.begin() + pos, buf.begin() + pos + name_len);
        pos += name_len;

        if (pos + 4 > buf.size()) throw std::runtime_error("버퍼 오버플로우: age");
        d.age = buf[pos] | (buf[pos+1] << 8) | (buf[pos+2] << 16) | (buf[pos+3] << 24);
        pos += 4;

        // 버전 2 필드
        if (d.version >= 2) {
            if (pos + 8 > buf.size()) throw std::runtime_error("버퍼 오버플로우: salary");
            d.salary = *reinterpret_cast<const double*>(&buf[pos]);
            pos += 8;

            if (pos + 2 > buf.size()) throw std::runtime_error("버퍼 오버플로우: dept_len");
            uint32_t dept_len = buf[pos] | (buf[pos+1] << 8);
            pos += 2;

            if (pos + dept_len > buf.size()) throw std::runtime_error("버퍼 오버플로우: department");
            d.department = std::string(buf.begin() + pos, buf.begin() + pos + dept_len);
            pos += dept_len;
        }

        return d;
    }
};

int main() {
    std::cout << "=== 프로토콜 버전 관리 테스트 ===\n\n";

    // 버전 1 데이터
    std::cout << "[버전 1 데이터]\n";
    VersionedData v1{1, "철수", 25, 0.0, ""};
    std::vector<uint8_t> v1_buf = v1.serialize(1);
    std::cout << "크기: " << v1_buf.size() << " 바이트\n";

    size_t pos = 0;
    VersionedData v1_restored = VersionedData::deserialize(v1_buf, pos);
    std::cout << "복원: " << v1_restored.name << ", 나이: " << v1_restored.age << "\n\n";

    // 버전 2 데이터
    std::cout << "[버전 2 데이터]\n";
    VersionedData v2{2, "영희", 30, 5500000.0, "엔지니어링"};
    std::vector<uint8_t> v2_buf = v2.serialize(2);
    std::cout << "크기: " << v2_buf.size() << " 바이트\n";

    pos = 0;
    VersionedData v2_restored = VersionedData::deserialize(v2_buf, pos);
    std::cout << "복원: " << v2_restored.name << ", 나이: " << v2_restored.age;
    std::cout << ", 급여: " << v2_restored.salary << ", 부서: " << v2_restored.department << "\n\n";

    // 버전 2 데이터를 버전 1처럼 읽기 (하위호환성)
    std::cout << "[버전 호환성 테스트]\n";
    std::cout << "버전 2 버퍼를 버전 1로 읽으면:\n";
    pos = 0;
    VersionedData v2_as_v1 = VersionedData::deserialize(v2_buf, pos);
    std::cout << "이름: " << v2_as_v1.name << ", 나이: " << v2_as_v1.age << "\n";
    std::cout << "버전 필드: " << v2_as_v1.version << "\n";

    if (v2_as_v1.version == 2) {
        std::cout << "✓ 버전 인식: 버전 2 필드 읽음\n";
        std::cout << "  급여: " << v2_as_v1.salary << ", 부서: " << v2_as_v1.department << "\n";
    } else {
        std::cout << "⚠ 버전 1로 인식: 추가 필드는 무시됨\n";
    }

    std::cout << "\n✓ 프로토콜 버전 관리 완료\n";
    return 0;
}
