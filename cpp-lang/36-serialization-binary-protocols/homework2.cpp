#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct Book {
    std::string title;
    std::string author;
    int year;
    double price;

    // JSON 문자열로 인코딩
    std::string to_json() const {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"title\": \"" << title << "\",\n";
        ss << "  \"author\": \"" << author << "\",\n";
        ss << "  \"year\": " << year << ",\n";
        ss << "  \"price\": " << price << "\n";
        ss << "}";
        return ss.str();
    }

    // JSON 문자열에서 디코딩 (단순 파서)
    void from_json(const std::string& json) {
        // 매우 간단한 JSON 파서 - 실제 프로덕션 코드에서는 라이브러리 사용
        size_t pos = 0;

        // title 추출
        pos = json.find("\"title\": \"");
        if (pos != std::string::npos) {
            pos += 10;
            size_t end = json.find("\"", pos);
            title = json.substr(pos, end - pos);
        }

        // author 추출
        pos = json.find("\"author\": \"");
        if (pos != std::string::npos) {
            pos += 11;
            size_t end = json.find("\"", pos);
            author = json.substr(pos, end - pos);
        }

        // year 추출
        pos = json.find("\"year\": ");
        if (pos != std::string::npos) {
            year = std::stoi(json.substr(pos + 8));
        }

        // price 추출
        pos = json.find("\"price\": ");
        if (pos != std::string::npos) {
            price = std::stod(json.substr(pos + 9));
        }
    }
};

int main() {
    std::vector<Book> books;
    books.push_back({"The Great Gatsby", "F. Scott Fitzgerald", 1925, 12.99});
    books.push_back({"1984", "George Orwell", 1949, 15.99});
    books.push_back({"To Kill a Mockingbird", "Harper Lee", 1960, 18.99});

    std::cout << "=== 도서 컬렉션 저장 (JSON 형식) ===\n\n";

    // JSON 배열로 파일에 저장
    std::ofstream file("books.json");
    file << "[\n";
    for (size_t i = 0; i < books.size(); ++i) {
        file << books[i].to_json();
        if (i < books.size() - 1) file << ",";
        file << "\n";
    }
    file << "]\n";
    file.close();

    std::cout << "파일 저장: books.json\n\n";

    // 파일에서 읽기
    std::ifstream in_file("books.json");
    std::string json_content((std::istreambuf_iterator<char>(in_file)),
                             std::istreambuf_iterator<char>());
    in_file.close();

    // JSON 배열 파싱 (단순 구현)
    std::vector<Book> loaded_books;
    size_t pos = 0;
    while ((pos = json_content.find("{", pos)) != std::string::npos) {
        size_t end = json_content.find("}", pos) + 1;
        std::string book_json = json_content.substr(pos, end - pos);
        Book book{};
        book.from_json(book_json);
        loaded_books.push_back(book);
        pos = end;
    }

    std::cout << "=== 복원된 도서 목록 ===\n";
    for (const auto& book : loaded_books) {
        std::cout << "제목: " << book.title << "\n";
        std::cout << "저자: " << book.author << "\n";
        std::cout << "출판년: " << book.year << "\n";
        std::cout << "가격: $" << book.price << "\n\n";
    }

    return 0;
}
