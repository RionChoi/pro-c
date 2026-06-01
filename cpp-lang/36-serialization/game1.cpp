#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdint>
#include <algorithm>

struct Item {
    std::string name;
    int quantity;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buf;
        uint32_t name_len = name.size();
        buf.push_back((name_len >> 0) & 0xFF);
        buf.push_back((name_len >> 8) & 0xFF);
        buf.insert(buf.end(), name.begin(), name.end());

        buf.push_back((quantity >> 0) & 0xFF);
        buf.push_back((quantity >> 8) & 0xFF);
        buf.push_back((quantity >> 16) & 0xFF);
        buf.push_back((quantity >> 24) & 0xFF);

        return buf;
    }

    static Item deserialize(const std::vector<uint8_t>& buf, size_t& pos) {
        Item item;
        if (pos + 2 > buf.size()) throw std::runtime_error("아이템 이름 길이 오버플로우");
        uint32_t name_len = buf[pos] | (buf[pos+1] << 8);
        pos += 2;

        if (pos + name_len > buf.size()) throw std::runtime_error("아이템 이름 오버플로우");
        item.name = std::string(buf.begin() + pos, buf.begin() + pos + name_len);
        pos += name_len;

        if (pos + 4 > buf.size()) throw std::runtime_error("아이템 수량 오버플로우");
        item.quantity = buf[pos] | (buf[pos+1] << 8) | (buf[pos+2] << 16) | (buf[pos+3] << 24);
        pos += 4;

        return item;
    }
};

struct Character {
    std::string name;
    int level;
    int hp;
    std::vector<Item> inventory;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buf;

        // 이름
        uint32_t name_len = name.size();
        buf.push_back((name_len >> 0) & 0xFF);
        buf.push_back((name_len >> 8) & 0xFF);
        buf.insert(buf.end(), name.begin(), name.end());

        // 레벨
        buf.push_back((level >> 0) & 0xFF);
        buf.push_back((level >> 8) & 0xFF);
        buf.push_back((level >> 16) & 0xFF);
        buf.push_back((level >> 24) & 0xFF);

        // HP
        buf.push_back((hp >> 0) & 0xFF);
        buf.push_back((hp >> 8) & 0xFF);
        buf.push_back((hp >> 16) & 0xFF);
        buf.push_back((hp >> 24) & 0xFF);

        // 인벤토리 아이템 개수
        uint32_t item_count = inventory.size();
        buf.push_back((item_count >> 0) & 0xFF);
        buf.push_back((item_count >> 8) & 0xFF);

        // 각 아이템 직렬화
        for (const auto& item : inventory) {
            std::vector<uint8_t> item_buf = item.serialize();
            buf.insert(buf.end(), item_buf.begin(), item_buf.end());
        }

        return buf;
    }

    static Character deserialize(const std::vector<uint8_t>& buf, size_t& pos) {
        Character ch;

        // 이름
        if (pos + 2 > buf.size()) throw std::runtime_error("캐릭터 이름 길이 오버플로우");
        uint32_t name_len = buf[pos] | (buf[pos+1] << 8);
        pos += 2;
        if (pos + name_len > buf.size()) throw std::runtime_error("캐릭터 이름 오버플로우");
        ch.name = std::string(buf.begin() + pos, buf.begin() + pos + name_len);
        pos += name_len;

        // 레벨
        if (pos + 4 > buf.size()) throw std::runtime_error("레벨 오버플로우");
        ch.level = buf[pos] | (buf[pos+1] << 8) | (buf[pos+2] << 16) | (buf[pos+3] << 24);
        pos += 4;

        // HP
        if (pos + 4 > buf.size()) throw std::runtime_error("HP 오버플로우");
        ch.hp = buf[pos] | (buf[pos+1] << 8) | (buf[pos+2] << 16) | (buf[pos+3] << 24);
        pos += 4;

        // 인벤토리 아이템 개수
        if (pos + 2 > buf.size()) throw std::runtime_error("아이템 개수 오버플로우");
        uint32_t item_count = buf[pos] | (buf[pos+1] << 8);
        pos += 2;

        for (uint32_t i = 0; i < item_count; ++i) {
            ch.inventory.push_back(Item::deserialize(buf, pos));
        }

        return ch;
    }

    void level_up() {
        level++;
        hp += 50;
    }

    void add_item(const std::string& item_name, int qty) {
        auto it = std::find_if(inventory.begin(), inventory.end(),
                               [&](const Item& i) { return i.name == item_name; });
        if (it != inventory.end()) {
            it->quantity += qty;
        } else {
            inventory.push_back({item_name, qty});
        }
    }
};

int main() {
    std::cout << "=== RPG 세이브/로드 시뮬레이션 ===\n\n";

    // 캐릭터 생성
    std::vector<Character> party;
    party.push_back({"전사", 10, 100, {{"검", 1}, {"방패", 1}}});
    party.push_back({"마법사", 8, 60, {{"지팡이", 1}, {"마나 포션", 5}}});

    std::cout << "[세이브 전 캐릭터]\n";
    for (const auto& ch : party) {
        std::cout << "이름: " << ch.name << ", 레벨: " << ch.level << ", HP: " << ch.hp << "\n";
        std::cout << "  인벤토리: ";
        for (const auto& item : ch.inventory) {
            std::cout << item.name << "(" << item.quantity << ") ";
        }
        std::cout << "\n";
    }

    // 게임 진행 (상태 변경)
    std::cout << "\n[게임 진행 중]\n";
    party[0].level_up();
    party[0].add_item("경험치 물약", 2);
    party[1].level_up();
    party[1].add_item("마나 포션", 3);

    std::cout << "[세이브 전 최종 상태]\n";
    for (const auto& ch : party) {
        std::cout << "이름: " << ch.name << ", 레벨: " << ch.level << ", HP: " << ch.hp << "\n";
        std::cout << "  인벤토리: ";
        for (const auto& item : ch.inventory) {
            std::cout << item.name << "(" << item.quantity << ") ";
        }
        std::cout << "\n";
    }

    // 세이브
    std::cout << "\n[게임 세이브]\n";
    std::ofstream out("savegame.bin", std::ios::binary);
    if (!out) {
        std::cerr << "저장 실패\n";
        return 1;
    }

    uint32_t party_size = party.size();
    std::vector<uint8_t> header;
    header.push_back((party_size >> 0) & 0xFF);
    header.push_back((party_size >> 8) & 0xFF);
    out.write(reinterpret_cast<const char*>(header.data()), header.size());

    for (const auto& ch : party) {
        std::vector<uint8_t> buf = ch.serialize();
        out.write(reinterpret_cast<const char*>(buf.data()), buf.size());
    }
    out.close();
    std::cout << "✓ " << party_size << "명의 캐릭터를 저장했습니다.\n";

    // 로드
    std::cout << "\n[게임 로드]\n";
    std::ifstream in("savegame.bin", std::ios::binary);
    if (!in) {
        std::cerr << "로드 실패\n";
        return 1;
    }

    std::vector<uint8_t> loaded((std::istreambuf_iterator<char>(in)),
                                 std::istreambuf_iterator<char>());
    in.close();

    std::vector<Character> loaded_party;
    size_t pos = 0;
    if (pos + 2 > loaded.size()) {
        std::cerr << "파일 형식 오류\n";
        return 1;
    }
    uint32_t loaded_size = loaded[pos] | (loaded[pos+1] << 8);
    pos += 2;

    for (uint32_t i = 0; i < loaded_size; ++i) {
        loaded_party.push_back(Character::deserialize(loaded, pos));
    }

    std::cout << "✓ " << loaded_party.size() << "명의 캐릭터를 복원했습니다.\n";

    // 검증
    std::cout << "\n[로드 후 캐릭터]\n";
    bool all_match = true;
    for (size_t i = 0; i < party.size(); ++i) {
        const auto& orig = party[i];
        const auto& load = loaded_party[i];

        bool match = (orig.name == load.name &&
                      orig.level == load.level &&
                      orig.hp == load.hp &&
                      orig.inventory.size() == load.inventory.size());

        if (match) {
            for (size_t j = 0; j < orig.inventory.size(); ++j) {
                if (orig.inventory[j].name != load.inventory[j].name ||
                    orig.inventory[j].quantity != load.inventory[j].quantity) {
                    match = false;
                    break;
                }
            }
        }

        std::cout << load.name << " - ";
        if (match) {
            std::cout << "✓ 데이터 일치";
        } else {
            std::cout << "✗ 데이터 불일치";
            all_match = false;
        }
        std::cout << "\n";
    }

    std::cout << "\n" << (all_match ? "✓ 모든 데이터가 올바르게 저장/복원되었습니다." : "✗ 일부 데이터가 손실되었습니다.") << "\n";
    return all_match ? 0 : 1;
}
