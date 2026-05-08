// 최종 프로젝트 — 고급 기능
// 적용 개념: 파일 I/O, 예외 처리 계층, 람다, 템플릿 범용 검색
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <iomanip>

// ── 예외 계층 ──────────────────────────────────────────────────────────────
class LibraryException : public std::runtime_error {
public:
    explicit LibraryException(const std::string& msg)
        : std::runtime_error("[도서관 오류] " + msg) {}
};

class ItemNotFoundException : public LibraryException {
public:
    explicit ItemNotFoundException(int id)
        : LibraryException("아이템 ID " + std::to_string(id) + "을(를) 찾을 수 없습니다.") {}
};

class AlreadyBorrowedException : public LibraryException {
public:
    explicit AlreadyBorrowedException(const std::string& title)
        : LibraryException("\"" + title + "\"은(는) 이미 대출 중입니다.") {}
};

class FileException : public LibraryException {
public:
    explicit FileException(const std::string& path)
        : LibraryException("파일 접근 실패: " + path) {}
};

// ── 간단한 도서 레코드 구조체 ──────────────────────────────────────────────
struct BookRecord {
    int         id;
    std::string title;
    std::string author;
    std::string genre;
    bool        borrowed;

    void print() const {
        std::cout << std::setw(4) << id
                  << " | " << std::setw(22) << std::left << title
                  << " | " << std::setw(12) << author
                  << " | " << std::setw(12) << genre
                  << " | " << (borrowed ? "대출중" : "대출가능") << "\n";
    }
};

// ── 템플릿 범용 검색 함수 ─────────────────────────────────────────────────
// Pred: BookRecord를 받아 bool을 반환하는 호출 가능 객체
template <typename Pred>
std::vector<BookRecord> search(const std::vector<BookRecord>& catalog, Pred predicate) {
    std::vector<BookRecord> results;
    std::copy_if(catalog.begin(), catalog.end(),
                 std::back_inserter(results), predicate);
    return results;
}

// ── CSV 저장 ───────────────────────────────────────────────────────────────
void saveCatalog(const std::vector<BookRecord>& catalog, const std::string& path) {
    std::ofstream file(path);
    if (!file.is_open()) throw FileException(path);

    file << "id,title,author,genre,borrowed\n";
    for (const auto& b : catalog) {
        file << b.id << ","
             << b.title << ","
             << b.author << ","
             << b.genre << ","
             << (b.borrowed ? 1 : 0) << "\n";
    }
    std::cout << "  [저장] " << catalog.size() << "건 → " << path << "\n";
}

// ── CSV 불러오기 ───────────────────────────────────────────────────────────
std::vector<BookRecord> loadCatalog(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) throw FileException(path);

    std::vector<BookRecord> catalog;
    std::string line;
    std::getline(file, line); // 헤더 건너뜀

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string token;
        BookRecord b{};
        std::getline(ss, token, ','); b.id      = std::stoi(token);
        std::getline(ss, b.title,  ',');
        std::getline(ss, b.author, ',');
        std::getline(ss, b.genre,  ',');
        std::getline(ss, token,    ','); b.borrowed = (token == "1");
        catalog.push_back(b);
    }
    std::cout << "  [불러오기] " << catalog.size() << "건 ← " << path << "\n";
    return catalog;
}

// ── 대출 처리 (예외 사용) ─────────────────────────────────────────────────
void borrowBook(std::vector<BookRecord>& catalog, int id) {
    auto it = std::find_if(catalog.begin(), catalog.end(),
                           [id](const BookRecord& b) { return b.id == id; });
    if (it == catalog.end()) throw ItemNotFoundException(id);
    if (it->borrowed)        throw AlreadyBorrowedException(it->title);
    it->borrowed = true;
    std::cout << "  ✅ \"" << it->title << "\" 대출 완료\n";
}

void returnBook(std::vector<BookRecord>& catalog, int id) {
    auto it = std::find_if(catalog.begin(), catalog.end(),
                           [id](const BookRecord& b) { return b.id == id; });
    if (it == catalog.end()) throw ItemNotFoundException(id);
    it->borrowed = false;
    std::cout << "  ✅ \"" << it->title << "\" 반납 완료\n";
}

// ── main ───────────────────────────────────────────────────────────────────
int main() {
    std::cout << "=== 고급 기능: 파일 I/O + 예외 처리 + 람다 + 템플릿 ===\n\n";

    // 초기 카탈로그
    std::vector<BookRecord> catalog = {
        {1, "클린 코드",    "로버트 마틴", "프로그래밍", false},
        {2, "C++ 프라이머", "리포먼",      "프로그래밍", false},
        {3, "디자인 패턴",  "GoF",         "소프트웨어", false},
        {4, "운영체제",     "실버샤츠",    "컴퓨터공학", false},
        {5, "알고리즘",     "커먼",        "컴퓨터공학", false},
    };

    // 1. CSV 저장 / 불러오기
    const std::string csvPath = "/tmp/library_catalog.csv";
    try {
        saveCatalog(catalog, csvPath);
        auto loaded = loadCatalog(csvPath);
        std::cout << "\n--- 불러온 카탈로그 ---\n";
        for (const auto& b : loaded) b.print();
    } catch (const FileException& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    // 2. 대출 / 예외 처리
    std::cout << "\n--- 대출 테스트 ---\n";
    try {
        borrowBook(catalog, 2);
        borrowBook(catalog, 2); // AlreadyBorrowedException 발생
    } catch (const AlreadyBorrowedException& e) {
        std::cerr << e.what() << "\n";
    } catch (const LibraryException& e) {
        std::cerr << e.what() << "\n";
    }

    try {
        borrowBook(catalog, 99); // ItemNotFoundException 발생
    } catch (const ItemNotFoundException& e) {
        std::cerr << e.what() << "\n";
    }

    // 3. 람다를 술어로 사용하는 템플릿 검색
    std::cout << "\n--- 장르 = '프로그래밍' 검색 ---\n";
    auto progBooks = search(catalog, [](const BookRecord& b) {
        return b.genre == "프로그래밍";
    });
    for (const auto& b : progBooks) b.print();

    std::cout << "\n--- 대출 가능 도서만 검색 ---\n";
    auto available = search(catalog, [](const BookRecord& b) {
        return !b.borrowed;
    });
    for (const auto& b : available) b.print();

    // 4. 람다로 제목 오름차순 정렬
    std::cout << "\n--- 제목 오름차순 정렬 ---\n";
    auto sorted = catalog;
    std::sort(sorted.begin(), sorted.end(),
              [](const BookRecord& a, const BookRecord& b) {
                  return a.title < b.title;
              });
    for (const auto& b : sorted) b.print();

    // 5. 변경사항 저장
    std::cout << "\n--- 변경사항 CSV 저장 ---\n";
    try {
        saveCatalog(catalog, csvPath);
    } catch (const FileException& e) {
        std::cerr << e.what() << "\n";
    }

    return 0;
}
