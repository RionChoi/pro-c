#include <iostream>
#include <string>
#include <map>
#include <vector>

class HTTPRequest {
    std::string method;
    std::string url;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    HTTPRequest& setMethod(const std::string& m) {
        method = m;
        return *this;
    }

    HTTPRequest& setURL(const std::string& u) {
        url = u;
        return *this;
    }

    HTTPRequest& setHeader(const std::string& key, const std::string& value) {
        headers[key] = value;
        return *this;
    }

    HTTPRequest& setBody(const std::string& b) {
        body = b;
        return *this;
    }

    std::string build() const {
        std::string request = method + " " + url + " HTTP/1.1\n";

        for (const auto& header : headers) {
            request += header.first + ": " + header.second + "\n";
        }

        if (!body.empty()) {
            request += "Content-Length: " + std::to_string(body.length()) + "\n";
        }

        request += "\n";
        if (!body.empty()) {
            request += body;
        }

        return request;
    }

    void print() const {
        std::cout << build();
    }
};

int main() {
    std::cout << "=== HTTP 요청 빌더 (Builder Pattern) ===" << std::endl;

    std::cout << "\n요청 1: GET 요청" << std::endl;
    std::cout << "---" << std::endl;
    HTTPRequest request1;
    request1
        .setMethod("GET")
        .setURL("/api/users")
        .setHeader("Host", "api.example.com")
        .setHeader("User-Agent", "MyClient/1.0")
        .print();

    std::cout << "\n요청 2: POST 요청" << std::endl;
    std::cout << "---" << std::endl;
    HTTPRequest request2;
    request2
        .setMethod("POST")
        .setURL("/api/users")
        .setHeader("Host", "api.example.com")
        .setHeader("Content-Type", "application/json")
        .setHeader("Authorization", "Bearer token123")
        .setBody(R"({"name": "John", "email": "john@example.com"})")
        .print();

    std::cout << "\n✓ Builder 패턴으로 HTTP 요청을 직관적으로 구성했습니다!" << std::endl;

    return 0;
}
