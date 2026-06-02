#include <iostream>
#include <string>
#include <map>
#include <vector>

class HTTPRequest {
private:
    std::string url_str;
    std::string method_str;
    std::map<std::string, std::string> headers;
    std::string body_str;
    int timeout_ms;

public:
    HTTPRequest() : url_str(""), method_str("GET"), body_str(""), timeout_ms(0) {}

    class Builder {
    private:
        std::string url_str;
        std::string method_str;
        std::map<std::string, std::string> headers;
        std::string body_str;
        int timeout_ms;

    public:
        Builder() : url_str(""), method_str("GET"), body_str(""), timeout_ms(0) {}

        Builder& url(const std::string& u) {
            url_str = u;
            return *this;
        }

        Builder& method(const std::string& m) {
            method_str = m;
            return *this;
        }

        Builder& header(const std::string& key, const std::string& value) {
            headers[key] = value;
            return *this;
        }

        Builder& body(const std::string& b) {
            body_str = b;
            return *this;
        }

        Builder& timeout(int ms) {
            timeout_ms = ms;
            return *this;
        }

        HTTPRequest build() const {
            HTTPRequest req;
            req.url_str = url_str;
            req.method_str = method_str;
            req.headers = headers;
            req.body_str = body_str;
            req.timeout_ms = timeout_ms;
            return req;
        }
    };

    static Builder builder() {
        return Builder();
    }

    void print() const {
        std::cout << method_str << " " << url_str << std::endl;
        std::cout << "Headers:" << std::endl;
        for (const auto& header : headers) {
            std::cout << "  " << header.first << ": " << header.second << std::endl;
        }
        if (!body_str.empty()) {
            std::cout << "Body: " << body_str << std::endl;
        }
        if (timeout_ms > 0) {
            std::cout << "Timeout: " << timeout_ms << "ms" << std::endl;
        }
        std::cout << std::endl;
    }
};

int main() {
    std::cout << "=== HTTP 요청 빌더 (Builder Pattern) ===" << std::endl << std::endl;

    // Example 1: Simple GET request
    std::cout << "요청 1 - 간단한 GET 요청:" << std::endl;
    HTTPRequest req1 = HTTPRequest::builder()
        .url("https://api.example.com/users")
        .method("GET")
        .timeout(3000)
        .build();
    req1.print();

    // Example 2: POST request with headers
    std::cout << "요청 2 - 헤더가 있는 POST 요청:" << std::endl;
    HTTPRequest req2 = HTTPRequest::builder()
        .url("https://api.example.com/users")
        .method("POST")
        .header("Content-Type", "application/json")
        .header("Authorization", "Bearer token123")
        .build();
    req2.print();

    // Example 3: POST with body
    std::cout << "요청 3 - 바디가 있는 POST 요청:" << std::endl;
    HTTPRequest req3 = HTTPRequest::builder()
        .url("https://api.example.com/users")
        .method("POST")
        .header("Content-Type", "application/json")
        .body("{\"name\": \"John\", \"age\": 30}")
        .timeout(5000)
        .build();
    req3.print();

    // Example 4: PUT request
    std::cout << "요청 4 - PUT 요청:" << std::endl;
    HTTPRequest req4 = HTTPRequest::builder()
        .url("https://api.example.com/users/42")
        .method("PUT")
        .header("Content-Type", "application/json")
        .header("X-API-Key", "secret-key")
        .body("{\"name\": \"Jane\", \"status\": \"active\"}")
        .timeout(8000)
        .build();
    req4.print();

    // Example 5: DELETE request
    std::cout << "요청 5 - DELETE 요청:" << std::endl;
    HTTPRequest req5 = HTTPRequest::builder()
        .url("https://api.example.com/users/42")
        .method("DELETE")
        .header("Authorization", "Bearer token456")
        .timeout(3000)
        .build();
    req5.print();

    // Example 6: Complex request with multiple headers
    std::cout << "요청 6 - 복잡한 요청:" << std::endl;
    HTTPRequest req6 = HTTPRequest::builder()
        .url("https://api.example.com/data")
        .method("POST")
        .header("Content-Type", "application/json")
        .header("Accept", "application/json")
        .header("User-Agent", "MyApp/1.0")
        .header("Authorization", "Bearer mytoken")
        .body("{\"action\": \"create\", \"data\": {\"field1\": \"value1\"}}")
        .timeout(10000)
        .build();
    req6.print();

    std::cout << "=== HTTP 빌더 성공 ===" << std::endl;

    return 0;
}
