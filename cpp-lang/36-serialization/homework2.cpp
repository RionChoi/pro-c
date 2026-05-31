#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <vector>

const uint32_t MAGIC_NUMBER = 0x50524F50;
const uint8_t CURRENT_VERSION = 2;

#pragma pack(1)
struct MessageHeader {
    uint32_t magic;
    uint8_t version;
    uint16_t length;
    uint16_t checksum;
};
#pragma pack()

#pragma pack(1)
struct PersonV1 {
    uint32_t age;
    char name[64];
};
#pragma pack()

#pragma pack(1)
struct PersonV2 {
    uint32_t age;
    char name[64];
    double height;
    char email[128];
};
#pragma pack()

uint16_t computeChecksum(const uint8_t* data, size_t length) {
    uint16_t sum = 0;
    for (size_t i = 0; i < length; ++i) {
        sum += data[i];
    }
    return sum;
}

uint32_t hton32(uint32_t value) {
    return ((value & 0xFF) << 24) |
           ((value & 0xFF00) << 8) |
           ((value & 0xFF0000) >> 8) |
           ((value & 0xFF000000) >> 24);
}

uint32_t ntoh32(uint32_t value) {
    return hton32(value);
}

uint16_t hton16(uint16_t value) {
    return ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
}

uint16_t ntoh16(uint16_t value) {
    return hton16(value);
}

void serializePersonV2(const PersonV2& p, std::vector<uint8_t>& buffer) {
    PersonV2 network_p = p;
    uint32_t age_network = hton32(p.age);
    std::memcpy(&network_p.age, &age_network, sizeof(uint32_t));

    uint16_t payload_size = static_cast<uint16_t>(sizeof(PersonV2));
    uint8_t payload[sizeof(PersonV2)];
    std::memcpy(payload, &network_p, sizeof(PersonV2));

    uint16_t checksum = computeChecksum(payload, payload_size);

    MessageHeader header;
    header.magic = hton32(MAGIC_NUMBER);
    header.version = CURRENT_VERSION;
    header.length = hton16(payload_size);
    header.checksum = hton16(checksum);

    uint8_t header_bytes[sizeof(MessageHeader)];
    std::memcpy(header_bytes, &header, sizeof(MessageHeader));

    buffer.clear();
    buffer.insert(buffer.end(), header_bytes, header_bytes + sizeof(MessageHeader));
    buffer.insert(buffer.end(), payload, payload + payload_size);
}

bool deserializePersonV2(PersonV2& p, const std::vector<uint8_t>& buffer) {
    if (buffer.size() < sizeof(MessageHeader) + sizeof(PersonV2)) {
        std::cerr << "Buffer too small\n";
        return false;
    }

    MessageHeader header;
    std::memcpy(&header, buffer.data(), sizeof(MessageHeader));

    if (ntoh32(header.magic) != MAGIC_NUMBER) {
        std::cerr << "Invalid magic number\n";
        return false;
    }

    if (header.version > CURRENT_VERSION) {
        std::cerr << "Unsupported version: " << (int)header.version << "\n";
        return false;
    }

    uint16_t payload_length = ntoh16(header.length);
    uint16_t received_checksum = ntoh16(header.checksum);
    uint16_t computed_checksum = computeChecksum(buffer.data() + sizeof(MessageHeader), payload_length);

    if (received_checksum != computed_checksum) {
        std::cerr << "Checksum mismatch\n";
        return false;
    }

    if (header.version == 1) {
        PersonV1 p1;
        std::memcpy(&p1, buffer.data() + sizeof(MessageHeader), sizeof(PersonV1));
        uint32_t age = ntoh32(p1.age);
        p.age = age;
        std::strcpy(p.name, p1.name);
        p.height = 0.0;
        std::strcpy(p.email, "");
        std::cout << "Loaded from V1 format, upgrading to V2\n";
    } else {
        PersonV2 temp;
        std::memcpy(&temp, buffer.data() + sizeof(MessageHeader), sizeof(PersonV2));
        uint32_t age = ntoh32(temp.age);
        p.age = age;
        std::strcpy(p.name, temp.name);
        p.height = temp.height;
        std::strcpy(p.email, temp.email);
    }

    return true;
}

int main() {
    PersonV2 person;
    person.age = 28;
    std::strcpy(person.name, "Bob Smith");
    person.height = 6.1;
    std::strcpy(person.email, "bob@example.com");

    std::vector<uint8_t> buffer;
    serializePersonV2(person, buffer);

    std::cout << "Serialized (V2) byte count: " << buffer.size() << "\n";
    std::cout << "Original Person V2:\n";
    std::cout << "  Age: " << person.age << "\n";
    std::cout << "  Name: " << person.name << "\n";
    std::cout << "  Height: " << person.height << "\n";
    std::cout << "  Email: " << person.email << "\n";

    PersonV2 restored;
    if (!deserializePersonV2(restored, buffer)) {
        std::cerr << "Deserialization failed\n";
        return 1;
    }

    std::cout << "\nDeserialized Person V2:\n";
    std::cout << "  Age: " << restored.age << "\n";
    std::cout << "  Name: " << restored.name << "\n";
    std::cout << "  Height: " << restored.height << "\n";
    std::cout << "  Email: " << restored.email << "\n";

    bool match = (person.age == restored.age &&
                  std::strcmp(person.name, restored.name) == 0 &&
                  person.height == restored.height &&
                  std::strcmp(person.email, restored.email) == 0);

    std::cout << "\nMatch: " << (match ? "Yes" : "No") << "\n";

    return match ? 0 : 1;
}
