// Singleton 패턴: 전역 설정 관리자
// 프로그램 전체에서 단 하나의 Config 인스턴스만 존재하도록 보장한다.
#include <iostream>
#include <string>
#include <map>

class Config {
public:
    // 유일한 인스턴스 반환 (Meyers' Singleton — thread-safe in C++11)
    static Config& getInstance() {
        static Config instance;
        return instance;
    }

    void set(const std::string& key, const std::string& value) {
        settings_[key] = value;
    }

    std::string get(const std::string& key, const std::string& fallback = "") const {
        auto it = settings_.find(key);
        return (it != settings_.end()) ? it->second : fallback;
    }

    void print() const {
        std::cout << "=== 현재 설정 ===\n";
        for (const auto& [key, value] : settings_) {
            std::cout << "  " << key << " = " << value << "\n";
        }
    }

private:
    Config() {
        // 기본값 초기화
        settings_["language"] = "ko";
        settings_["theme"]    = "light";
        settings_["volume"]   = "80";
    }

    // 복사 및 대입 금지 — 두 번째 인스턴스 생성을 차단한다.
    Config(const Config&)            = delete;
    Config& operator=(const Config&) = delete;

    std::map<std::string, std::string> settings_;
};

int main() {
    std::cout << "--- Singleton 패턴: 설정 관리자 ---\n\n";

    // 두 참조가 동일한 인스턴스를 가리키는지 확인
    Config& cfg1 = Config::getInstance();
    Config& cfg2 = Config::getInstance();

    std::cout << "cfg1 주소: " << static_cast<void*>(&cfg1) << "\n";
    std::cout << "cfg2 주소: " << static_cast<void*>(&cfg2) << "\n";
    std::cout << ((&cfg1 == &cfg2) ? "✅ 동일한 인스턴스\n" : "❌ 다른 인스턴스\n");
    std::cout << "\n";

    cfg1.print();
    std::cout << "\n";

    // cfg1을 통해 변경하면 cfg2에도 반영된다.
    cfg1.set("theme", "dark");
    cfg1.set("language", "en");

    std::cout << "cfg1에서 theme 변경 후 cfg2로 조회:\n";
    std::cout << "  theme    = " << cfg2.get("theme") << "\n";
    std::cout << "  language = " << cfg2.get("language") << "\n";
    std::cout << "  volume   = " << cfg2.get("volume", "기본값 없음") << "\n";

    return 0;
}
