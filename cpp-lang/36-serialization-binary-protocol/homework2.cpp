#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cstdint>

struct StudentRecord {
    char name[64];
    uint32_t student_id;
    double gpa;
};

class BinarySerializer {
public:
    static constexpr uint32_t MAGIC = 0x53544401;  // "STD\1"
    static constexpr uint16_t VERSION = 2;

    std::vector<uint8_t> serialize(const StudentRecord& record) {
        std::vector<uint8_t> buffer;

        write_uint32(buffer, MAGIC);
        write_uint16(buffer, VERSION);
        write_uint16(buffer, 0);  // reserved

        size_t checksum_pos = buffer.size();
        write_uint32(buffer, 0);  // placeholder for checksum

        write_string(buffer, record.name);
        write_uint32(buffer, record.student_id);
        write_double(buffer, record.gpa);

        uint32_t checksum = calculate_checksum(buffer, checksum_pos + 4);
        write_uint32_at(buffer, checksum_pos, checksum);

        return buffer;
    }

    bool deserialize(const std::vector<uint8_t>& buffer, StudentRecord& record) {
        size_t pos = 0;

        uint32_t magic = read_uint32(buffer, pos);
        if (magic != MAGIC) {
            std::cerr << "오류: 잘못된 매직 번호\n";
            return false;
        }

        uint16_t version = read_uint16(buffer, pos);
        if (version != VERSION) {
            std::cerr << "오류: 지원하지 않는 버전\n";
            return false;
        }

        pos += 2;  // skip reserved
        uint32_t stored_checksum = read_uint32(buffer, pos);

        uint32_t calculated = calculate_checksum(buffer, 8);
        if (stored_checksum != calculated) {
            std::cerr << "오류: 체크섬 불일치 (손상된 데이터)\n";
            return false;
        }

        read_string(buffer, pos, record.name, sizeof(record.name));
        record.student_id = read_uint32(buffer, pos);
        record.gpa = read_double(buffer, pos);

        return true;
    }

    void save_to_file(const std::string& filename, const std::vector<StudentRecord>& records) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "파일을 열 수 없습니다\n";
            return;
        }

        uint32_t count = records.size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (const auto& record : records) {
            auto buffer = serialize(record);
            file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        }

        file.close();
        std::cout << "파일에 " << count << "개 레코드 저장\n";
    }

    bool load_from_file(const std::string& filename, std::vector<StudentRecord>& records) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "파일을 열 수 없습니다\n";
            return false;
        }

        uint32_t count = 0;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));

        records.clear();
        for (uint32_t i = 0; i < count; ++i) {
            std::vector<uint8_t> buffer(256);
            file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

            StudentRecord record;
            if (!deserialize(buffer, record)) {
                std::cerr << "레코드 " << i << " 역직렬화 실패\n";
                return false;
            }
            records.push_back(record);
        }

        file.close();
        return true;
    }

private:
    void write_uint32(std::vector<uint8_t>& buf, uint32_t val) {
        buf.push_back((val >> 24) & 0xFF);
        buf.push_back((val >> 16) & 0xFF);
        buf.push_back((val >> 8) & 0xFF);
        buf.push_back(val & 0xFF);
    }

    void write_uint32_at(std::vector<uint8_t>& buf, size_t pos, uint32_t val) {
        buf[pos] = (val >> 24) & 0xFF;
        buf[pos + 1] = (val >> 16) & 0xFF;
        buf[pos + 2] = (val >> 8) & 0xFF;
        buf[pos + 3] = val & 0xFF;
    }

    void write_uint16(std::vector<uint8_t>& buf, uint16_t val) {
        buf.push_back((val >> 8) & 0xFF);
        buf.push_back(val & 0xFF);
    }

    void write_double(std::vector<uint8_t>& buf, double val) {
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&val);
        for (int i = 0; i < 8; ++i) buf.push_back(ptr[i]);
    }

    void write_string(std::vector<uint8_t>& buf, const char* str) {
        size_t len = std::strlen(str);
        write_uint16(buf, len);
        for (size_t i = 0; i < len; ++i) buf.push_back(str[i]);
    }

    uint32_t read_uint32(const std::vector<uint8_t>& buf, size_t& pos) {
        uint32_t val = (buf[pos] << 24) | (buf[pos + 1] << 16) |
                       (buf[pos + 2] << 8) | buf[pos + 3];
        pos += 4;
        return val;
    }

    uint16_t read_uint16(const std::vector<uint8_t>& buf, size_t& pos) {
        uint16_t val = (buf[pos] << 8) | buf[pos + 1];
        pos += 2;
        return val;
    }

    double read_double(const std::vector<uint8_t>& buf, size_t& pos) {
        double val;
        std::memcpy(&val, buf.data() + pos, 8);
        pos += 8;
        return val;
    }

    void read_string(const std::vector<uint8_t>& buf, size_t& pos, char* str, size_t max_len) {
        uint16_t len = read_uint16(buf, pos);
        size_t to_copy = (len < max_len - 1) ? len : max_len - 1;
        std::memcpy(str, buf.data() + pos, to_copy);
        str[to_copy] = '\0';
        pos += len;
    }

    uint32_t calculate_checksum(const std::vector<uint8_t>& buf, size_t start) {
        uint32_t sum = 0;
        for (size_t i = start; i < buf.size(); ++i) {
            sum = (sum << 1) ^ buf[i];
        }
        return sum;
    }
};

int main() {
    std::cout << "=== 이진 직렬화 예제 ===\n\n";

    StudentRecord records[] = {
        {"김철수", 2024001, 3.85},
        {"이영희", 2024002, 4.00},
        {"박민준", 2024003, 3.62}
    };

    BinarySerializer serializer;

    std::cout << "1. 직렬화:\n";
    auto buffer = serializer.serialize(records[0]);
    std::cout << "버퍼 크기: " << buffer.size() << " 바이트\n";
    std::cout << "마직: 0x" << std::hex << *(uint32_t*)buffer.data() << std::dec << "\n\n";

    std::cout << "2. 파일 저장:\n";
    std::vector<StudentRecord> to_save(records, records + 3);
    serializer.save_to_file("students.bin", to_save);

    std::cout << "\n3. 파일 로드 및 역직렬화:\n";
    std::vector<StudentRecord> loaded;
    if (serializer.load_from_file("students.bin", loaded)) {
        for (const auto& s : loaded) {
            std::cout << "이름: " << s.name << ", 학번: " << s.student_id
                      << ", GPA: " << s.gpa << "\n";
        }
    }

    return 0;
}
