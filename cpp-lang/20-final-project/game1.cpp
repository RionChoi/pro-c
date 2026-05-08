// 최종 프로젝트 — 대화형 CLI 도서관 관리 시스템
// 적용 개념: Singleton, Observer, OOP, STL, 스마트 포인터, 파일 I/O, 예외 처리
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <limits>

// ── 예외 ──────────────────────────────────────────────────────────────────
class LibraryException : public std::runtime_error {
public:
    explicit LibraryException(const std::string& msg)
        : std::runtime_error(msg) {}
};

// ── Observer: 도서관 이벤트 알림 ──────────────────────────────────────────
class EventObserver {
public:
    virtual ~EventObserver() = default;
    virtual void onEvent(const std::string& event) = 0;
};

class ConsoleLogger : public EventObserver {
public:
    void onEvent(const std::string& event) override {
        std::cout << "  [LOG] " << event << "\n";
    }
};

// ── 도서 레코드 ───────────────────────────────────────────────────────────
struct Book {
    int         id;
    std::string title;
    std::string author;
    std::string genre;
    bool        borrowed  = false;
    int         borrowerId = 0;

    void print() const {
        std::cout << std::setw(4) << id
                  << " | " << std::setw(22) << std::left << title
                  << " | " << std::setw(12) << author
                  << " | " << std::setw(12) << genre
                  << " | " << (borrowed ? "대출중 (회원 " + std::to_string(borrowerId) + ")"
                                        : "대출가능") << "\n";
    }
};

// ── 회원 레코드 ───────────────────────────────────────────────────────────
struct Member {
    int         id;
    std::string name;
    std::string contact;

    void print() const {
        std::cout << "  ID:" << std::setw(4) << id
                  << " | " << std::setw(12) << std::left << name
                  << " | " << contact << "\n";
    }
};

// ── Singleton: 도서관 관리자 ──────────────────────────────────────────────
class LibraryManager {
public:
    static LibraryManager& instance() {
        static LibraryManager inst;
        return inst;
    }

    LibraryManager(const LibraryManager&)            = delete;
    LibraryManager& operator=(const LibraryManager&) = delete;

    // Observer 등록
    void addObserver(std::shared_ptr<EventObserver> obs) {
        observers_.push_back(std::move(obs));
    }

    // ── 도서 관리 ─────────────────────────────────────────────────────────
    void addBook(const std::string& title, const std::string& author,
                 const std::string& genre) {
        books_.push_back({nextBookId_++, title, author, genre, false, 0});
        notify("도서 등록: \"" + title + "\"");
    }

    void listBooks() const {
        std::cout << "\n=== 소장 도서 (" << books_.size() << "권) ===\n";
        if (books_.empty()) { std::cout << "  등록된 도서가 없습니다.\n"; return; }
        for (const auto& b : books_) b.print();
    }

    void searchBooks(const std::string& keyword) const {
        std::cout << "\n=== 검색: \"" << keyword << "\" ===\n";
        int cnt = 0;
        for (const auto& b : books_) {
            if (b.title.find(keyword)  != std::string::npos ||
                b.author.find(keyword) != std::string::npos ||
                b.genre.find(keyword)  != std::string::npos) {
                b.print(); ++cnt;
            }
        }
        if (cnt == 0) std::cout << "  검색 결과가 없습니다.\n";
    }

    // ── 회원 관리 ─────────────────────────────────────────────────────────
    void addMember(const std::string& name, const std::string& contact) {
        members_.push_back({nextMemberId_++, name, contact});
        notify("회원 등록: " + name);
    }

    void listMembers() const {
        std::cout << "\n=== 등록 회원 (" << members_.size() << "명) ===\n";
        if (members_.empty()) { std::cout << "  등록된 회원이 없습니다.\n"; return; }
        for (const auto& m : members_) m.print();
    }

    // ── 대출 / 반납 ───────────────────────────────────────────────────────
    void borrowBook(int memberId, int bookId) {
        auto& book   = findBook(bookId);
        auto& member = findMember(memberId);
        if (book.borrowed)
            throw LibraryException("\"" + book.title + "\"은(는) 이미 대출 중입니다.");
        book.borrowed   = true;
        book.borrowerId = memberId;
        notify("대출: " + member.name + " → \"" + book.title + "\"");
    }

    void returnBook(int memberId, int bookId) {
        auto& book   = findBook(bookId);
        auto& member = findMember(memberId);
        if (!book.borrowed || book.borrowerId != memberId)
            throw LibraryException("해당 회원의 대출 기록이 없습니다.");
        book.borrowed   = false;
        book.borrowerId = 0;
        notify("반납: " + member.name + " → \"" + book.title + "\"");
    }

    // ── 파일 저장 / 불러오기 ──────────────────────────────────────────────
    void save(const std::string& path) const {
        std::ofstream f(path);
        if (!f.is_open()) throw LibraryException("파일 저장 실패: " + path);
        f << "BOOKS\n";
        for (const auto& b : books_)
            f << b.id << "," << b.title << "," << b.author << ","
              << b.genre << "," << b.borrowed << "," << b.borrowerId << "\n";
        f << "MEMBERS\n";
        for (const auto& m : members_)
            f << m.id << "," << m.name << "," << m.contact << "\n";
        f << "META\n" << nextBookId_ << "," << nextMemberId_ << "\n";
        notify("데이터 저장 완료: " + path);
    }

    void load(const std::string& path) {
        std::ifstream f(path);
        if (!f.is_open()) throw LibraryException("파일 불러오기 실패: " + path);
        books_.clear();
        members_.clear();
        std::string line, section;
        while (std::getline(f, line)) {
            if (line == "BOOKS" || line == "MEMBERS" || line == "META") {
                section = line; continue;
            }
            if (line.empty()) continue;
            std::istringstream ss(line);
            std::string tok;
            if (section == "BOOKS") {
                Book b{};
                std::getline(ss, tok, ','); b.id = std::stoi(tok);
                std::getline(ss, b.title,  ',');
                std::getline(ss, b.author, ',');
                std::getline(ss, b.genre,  ',');
                std::getline(ss, tok, ','); b.borrowed   = (tok == "1");
                std::getline(ss, tok, ','); b.borrowerId = std::stoi(tok);
                books_.push_back(b);
            } else if (section == "MEMBERS") {
                Member m{};
                std::getline(ss, tok, ','); m.id = std::stoi(tok);
                std::getline(ss, m.name,    ',');
                std::getline(ss, m.contact, ',');
                members_.push_back(m);
            } else if (section == "META") {
                std::getline(ss, tok, ','); nextBookId_   = std::stoi(tok);
                std::getline(ss, tok, ','); nextMemberId_ = std::stoi(tok);
            }
        }
        notify("데이터 불러오기 완료: " + path);
    }

    // ── 통계 ──────────────────────────────────────────────────────────────
    void statistics() const {
        int borrowed = static_cast<int>(
            std::count_if(books_.begin(), books_.end(),
                          [](const Book& b) { return b.borrowed; }));
        std::cout << "\n=== 통계 ===\n"
                  << "  전체 도서: " << books_.size() << "권\n"
                  << "  대출 중:   " << borrowed << "권\n"
                  << "  대출 가능: " << (books_.size() - static_cast<std::size_t>(borrowed)) << "권\n"
                  << "  등록 회원: " << members_.size() << "명\n";
    }

private:
    LibraryManager() = default;

    void notify(const std::string& event) const {
        for (const auto& obs : observers_) obs->onEvent(event);
    }

    Book& findBook(int id) {
        auto it = std::find_if(books_.begin(), books_.end(),
                               [id](const Book& b) { return b.id == id; });
        if (it == books_.end()) throw LibraryException("도서 ID " + std::to_string(id) + " 없음");
        return *it;
    }

    Member& findMember(int id) {
        auto it = std::find_if(members_.begin(), members_.end(),
                               [id](const Member& m) { return m.id == id; });
        if (it == members_.end()) throw LibraryException("회원 ID " + std::to_string(id) + " 없음");
        return *it;
    }

    std::vector<Book>   books_;
    std::vector<Member> members_;
    int nextBookId_   = 1;
    int nextMemberId_ = 101;
    std::vector<std::shared_ptr<EventObserver>> observers_;
};

// ── 유틸리티 ──────────────────────────────────────────────────────────────
static int inputInt(const std::string& prompt) {
    int v;
    std::cout << prompt;
    while (!(std::cin >> v)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  숫자를 입력하세요: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return v;
}

static std::string inputStr(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

// ── 메뉴 ──────────────────────────────────────────────────────────────────
static void printMenu() {
    std::cout << "\n╔═══════════════════════════════╗\n"
              << "║   📚 도서관 관리 시스템 v1.0  ║\n"
              << "╠═══════════════════════════════╣\n"
              << "║  1. 도서 목록 보기            ║\n"
              << "║  2. 도서 등록                 ║\n"
              << "║  3. 도서 검색                 ║\n"
              << "║  4. 대출                      ║\n"
              << "║  5. 반납                      ║\n"
              << "║  6. 회원 목록 보기            ║\n"
              << "║  7. 회원 등록                 ║\n"
              << "║  8. 통계                      ║\n"
              << "║  9. 저장                      ║\n"
              << "║ 10. 불러오기                  ║\n"
              << "║  0. 종료                      ║\n"
              << "╚═══════════════════════════════╝\n"
              << "선택: ";
}

// ── main ───────────────────────────────────────────────────────────────────
int main() {
    auto& lib = LibraryManager::instance();
    lib.addObserver(std::make_shared<ConsoleLogger>());

    // 샘플 데이터
    lib.addBook("클린 코드",    "로버트 마틴", "프로그래밍");
    lib.addBook("C++ 프라이머", "리포먼",      "프로그래밍");
    lib.addBook("디자인 패턴",  "GoF",         "소프트웨어");
    lib.addBook("운영체제",     "실버샤츠",    "컴퓨터공학");
    lib.addMember("김철수", "010-1234-5678");
    lib.addMember("이영희", "010-9876-5432");

    const std::string saveFile = "/tmp/library_save.dat";
    int choice = -1;

    while (choice != 0) {
        printMenu();
        choice = inputInt("");

        try {
            switch (choice) {
            case 1:
                lib.listBooks();
                break;
            case 2: {
                auto title  = inputStr("  제목: ");
                auto author = inputStr("  저자: ");
                auto genre  = inputStr("  장르: ");
                lib.addBook(title, author, genre);
                break;
            }
            case 3: {
                auto kw = inputStr("  검색어 (제목/저자/장르): ");
                lib.searchBooks(kw);
                break;
            }
            case 4: {
                lib.listMembers();
                int mid = inputInt("  회원 ID: ");
                lib.listBooks();
                int bid = inputInt("  도서 ID: ");
                lib.borrowBook(mid, bid);
                break;
            }
            case 5: {
                int mid = inputInt("  회원 ID: ");
                int bid = inputInt("  도서 ID: ");
                lib.returnBook(mid, bid);
                break;
            }
            case 6:
                lib.listMembers();
                break;
            case 7: {
                auto name    = inputStr("  이름: ");
                auto contact = inputStr("  연락처: ");
                lib.addMember(name, contact);
                break;
            }
            case 8:
                lib.statistics();
                break;
            case 9:
                lib.save(saveFile);
                break;
            case 10:
                lib.load(saveFile);
                break;
            case 0:
                std::cout << "\n  프로그램을 종료합니다. 안녕히 가세요!\n";
                break;
            default:
                std::cout << "  올바른 번호를 입력하세요.\n";
                break;
            }
        } catch (const LibraryException& e) {
            std::cerr << "  [오류] " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cerr << "  [시스템 오류] " << e.what() << "\n";
        }
    }

    return 0;
}
