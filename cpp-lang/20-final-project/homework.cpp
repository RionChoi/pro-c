// 최종 프로젝트 — 핵심 클래스 설계
// 적용 개념: 상속/다형성, 연산자 오버로딩, STL 컨테이너/알고리즘, 스마트 포인터
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iomanip>

// ── 추상 기반 클래스: 도서관 소장 아이템 ──────────────────────────────────
class Item {
public:
    Item(int id, std::string title, std::string author)
        : id_(id), title_(std::move(title)), author_(std::move(author)), borrowed_(false) {}
    virtual ~Item() = default;

    int               id()       const { return id_; }
    const std::string& title()   const { return title_; }
    const std::string& author()  const { return author_; }
    bool              borrowed() const { return borrowed_; }
    void              borrow()         { borrowed_ = true; }
    void              returnItem()     { borrowed_ = false; }

    virtual std::string type()  const = 0;
    virtual void        print() const = 0;

    // 출력 연산자 오버로딩
    friend std::ostream& operator<<(std::ostream& os, const Item& item) {
        item.print();
        return os;
    }

protected:
    int         id_;
    std::string title_;
    std::string author_;
    bool        borrowed_;
};

// ── 구체 클래스: 도서 ──────────────────────────────────────────────────────
class Book : public Item {
public:
    Book(int id, std::string title, std::string author, std::string genre)
        : Item(id, std::move(title), std::move(author)), genre_(std::move(genre)) {}

    std::string type() const override { return "도서"; }

    void print() const override {
        std::cout << std::setw(4) << id_
                  << " | [도서] " << std::setw(20) << std::left << title_
                  << " | 저자: " << std::setw(12) << author_
                  << " | 장르: " << std::setw(10) << genre_
                  << " | " << (borrowed_ ? "대출중" : "대출가능") << "\n";
    }

    const std::string& genre() const { return genre_; }

private:
    std::string genre_;
};

// ── 구체 클래스: 잡지 ──────────────────────────────────────────────────────
class Magazine : public Item {
public:
    Magazine(int id, std::string title, std::string publisher, int issue)
        : Item(id, std::move(title), std::move(publisher)), issue_(issue) {}

    std::string type() const override { return "잡지"; }

    void print() const override {
        std::cout << std::setw(4) << id_
                  << " | [잡지] " << std::setw(20) << std::left << title_
                  << " | 출판사: " << std::setw(11) << author_
                  << " | 호: " << std::setw(5) << issue_
                  << " | " << (borrowed_ ? "대출중" : "대출가능") << "\n";
    }

private:
    int issue_;
};

// ── 회원 클래스 ────────────────────────────────────────────────────────────
class Member {
public:
    Member(int id, std::string name, std::string contact)
        : id_(id), name_(std::move(name)), contact_(std::move(contact)) {}

    int               id()      const { return id_; }
    const std::string& name()   const { return name_; }
    const std::string& contact()const { return contact_; }

    void borrowItem(int itemId)  { borrowedIds_.push_back(itemId); }
    void returnItem(int itemId) {
        borrowedIds_.erase(
            std::remove(borrowedIds_.begin(), borrowedIds_.end(), itemId),
            borrowedIds_.end()
        );
    }
    const std::vector<int>& borrowedIds() const { return borrowedIds_; }

    void print() const {
        std::cout << "  ID:" << id_ << " | " << name_
                  << " | 연락처: " << contact_
                  << " | 대출 중: " << borrowedIds_.size() << "권\n";
    }

private:
    int              id_;
    std::string      name_;
    std::string      contact_;
    std::vector<int> borrowedIds_;
};

// ── 도서관 클래스 ──────────────────────────────────────────────────────────
class Library {
public:
    // 아이템 추가
    void addItem(std::unique_ptr<Item> item) {
        items_.push_back(std::move(item));
    }

    // 회원 추가
    void addMember(std::shared_ptr<Member> member) {
        members_.push_back(std::move(member));
    }

    // 전체 소장 목록 출력
    void listItems() const {
        std::cout << "\n=== 소장 목록 (" << items_.size() << "건) ===\n";
        for (const auto& item : items_) {
            item->print();
        }
    }

    // 제목으로 검색
    void searchByTitle(const std::string& keyword) const {
        std::cout << "\n=== 검색: \"" << keyword << "\" ===\n";
        int found = 0;
        for (const auto& item : items_) {
            if (item->title().find(keyword) != std::string::npos) {
                item->print();
                ++found;
            }
        }
        if (found == 0) std::cout << "  검색 결과 없음\n";
    }

    // 대출
    bool borrowItem(int memberId, int itemId) {
        auto* member = findMember(memberId);
        auto* item   = findItem(itemId);
        if (!member || !item) return false;
        if (item->borrowed()) {
            std::cout << "  [오류] 이미 대출 중인 아이템입니다.\n";
            return false;
        }
        item->borrow();
        member->borrowItem(itemId);
        std::cout << "  ✅ " << member->name() << " → \"" << item->title() << "\" 대출 완료\n";
        return true;
    }

    // 반납
    bool returnItem(int memberId, int itemId) {
        auto* member = findMember(memberId);
        auto* item   = findItem(itemId);
        if (!member || !item) return false;
        item->returnItem();
        member->returnItem(itemId);
        std::cout << "  ✅ " << member->name() << " → \"" << item->title() << "\" 반납 완료\n";
        return true;
    }

    // 통계
    void statistics() const {
        int total    = static_cast<int>(items_.size());
        int borrowed = static_cast<int>(
            std::count_if(items_.begin(), items_.end(),
                          [](const auto& i) { return i->borrowed(); }));
        std::cout << "\n=== 통계 ===\n"
                  << "  전체 소장: " << total << "건\n"
                  << "  대출 중:   " << borrowed << "건\n"
                  << "  대출 가능: " << (total - borrowed) << "건\n"
                  << "  등록 회원: " << members_.size() << "명\n";
    }

    // 회원 목록
    void listMembers() const {
        std::cout << "\n=== 회원 목록 (" << members_.size() << "명) ===\n";
        for (const auto& m : members_) m->print();
    }

private:
    Item* findItem(int id) const {
        auto it = std::find_if(items_.begin(), items_.end(),
                               [id](const auto& i) { return i->id() == id; });
        return (it != items_.end()) ? it->get() : nullptr;
    }

    Member* findMember(int id) const {
        auto it = std::find_if(members_.begin(), members_.end(),
                               [id](const auto& m) { return m->id() == id; });
        return (it != members_.end()) ? it->get() : nullptr;
    }

    std::vector<std::unique_ptr<Item>>   items_;
    std::vector<std::shared_ptr<Member>> members_;
};

// ── main ───────────────────────────────────────────────────────────────────
int main() {
    Library lib;

    // 소장 아이템 등록
    lib.addItem(std::make_unique<Book>(1, "클린 코드",       "로버트 마틴", "프로그래밍"));
    lib.addItem(std::make_unique<Book>(2, "C++ 프라이머",    "리포먼",      "프로그래밍"));
    lib.addItem(std::make_unique<Book>(3, "디자인 패턴",     "GoF",         "소프트웨어"));
    lib.addItem(std::make_unique<Book>(4, "운영체제",        "실버샤츠",    "컴퓨터공학"));
    lib.addItem(std::make_unique<Magazine>(5, "과학동아", "동아사이언스", 372));
    lib.addItem(std::make_unique<Magazine>(6, "네이처",   "Springer",    601));

    // 회원 등록
    lib.addMember(std::make_shared<Member>(101, "김철수", "010-1234-5678"));
    lib.addMember(std::make_shared<Member>(102, "이영희", "010-9876-5432"));
    lib.addMember(std::make_shared<Member>(103, "박지수", "010-5555-7777"));

    lib.listItems();
    lib.listMembers();

    std::cout << "\n--- 대출/반납 테스트 ---\n";
    lib.borrowItem(101, 1);
    lib.borrowItem(102, 3);
    lib.borrowItem(101, 1); // 이미 대출 중 — 오류

    lib.listItems();
    lib.statistics();

    std::cout << "\n--- 반납 ---\n";
    lib.returnItem(101, 1);
    lib.statistics();

    std::cout << "\n--- 제목 검색: \"C++\" ---\n";
    lib.searchByTitle("C++");

    std::cout << "\n--- 제목 검색: \"패턴\" ---\n";
    lib.searchByTitle("패턴");

    return 0;
}
