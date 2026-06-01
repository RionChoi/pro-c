#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class HttpRequestBuilder {
private:
    string method;
    string url;
    map<string, string> headers;
    string body;
    int timeout;

public:
    HttpRequestBuilder() : method("GET"), timeout(30) {}

    HttpRequestBuilder& setMethod(const string& m) {
        method = m;
        return *this;
    }

    HttpRequestBuilder& setUrl(const string& u) {
        url = u;
        return *this;
    }

    HttpRequestBuilder& addHeader(const string& key, const string& value) {
        headers[key] = value;
        return *this;
    }

    HttpRequestBuilder& setBody(const string& b) {
        body = b;
        return *this;
    }

    HttpRequestBuilder& setTimeout(int seconds) {
        timeout = seconds;
        return *this;
    }

    string build() const {
        string request;
        request += method + " " + url + " HTTP/1.1\n";

        for (const auto& [key, value] : headers) {
            request += key + ": " + value + "\n";
        }

        if (!body.empty()) {
            request += "Content-Length: " + to_string(body.length()) + "\n";
        }

        request += "Timeout: " + to_string(timeout) + "s\n";

        if (!body.empty()) {
            request += "\n" + body;
        }

        return request;
    }
};

class JsonBuilder {
private:
    map<string, string> fields;
    string indent_str;

public:
    JsonBuilder() : indent_str("  ") {}

    JsonBuilder& add(const string& key, const string& value) {
        fields[key] = "\"" + value + "\"";
        return *this;
    }

    JsonBuilder& add(const string& key, int value) {
        fields[key] = to_string(value);
        return *this;
    }

    JsonBuilder& add(const string& key, double value) {
        fields[key] = to_string(value);
        return *this;
    }

    JsonBuilder& add(const string& key, bool value) {
        fields[key] = value ? "true" : "false";
        return *this;
    }

    string build() const {
        string json = "{\n";

        int count = 0;
        for (const auto& [key, value] : fields) {
            json += indent_str + "\"" + key + "\": " + value;
            if (++count < (int)fields.size()) {
                json += ",";
            }
            json += "\n";
        }

        json += "}";
        return json;
    }
};

int main() {
    cout << "=== Builder Pattern 심화 ===" << endl << endl;

    cout << "1. HTTP GET 요청 빌더" << endl;
    string req1 = HttpRequestBuilder()
        .setUrl("https://api.example.com/users")
        .addHeader("Authorization", "Bearer token123")
        .addHeader("Accept", "application/json")
        .build();
    cout << req1 << endl << endl;

    cout << "2. HTTP POST 요청 빌더" << endl;
    string req2 = HttpRequestBuilder()
        .setMethod("POST")
        .setUrl("https://api.example.com/users")
        .addHeader("Content-Type", "application/json")
        .addHeader("Authorization", "Bearer token123")
        .setBody("{\"name\": \"John\", \"age\": 30}")
        .setTimeout(60)
        .build();
    cout << req2 << endl << endl;

    cout << "3. HTTP DELETE 요청 빌더" << endl;
    string req3 = HttpRequestBuilder()
        .setMethod("DELETE")
        .setUrl("https://api.example.com/users/123")
        .addHeader("Authorization", "Bearer token123")
        .build();
    cout << req3 << endl << endl;

    cout << "4. JSON 객체 빌더" << endl;
    string json1 = JsonBuilder()
        .add("name", "Alice")
        .add("age", 25)
        .add("score", 95.5)
        .add("active", true)
        .build();
    cout << json1 << endl << endl;

    cout << "5. 중첩된 Builder 패턴 (HTTP + JSON)" << endl;
    string json_body = JsonBuilder()
        .add("username", "bob_smith")
        .add("email", "bob@example.com")
        .add("age", 28)
        .add("verified", true)
        .build();

    string req4 = HttpRequestBuilder()
        .setMethod("POST")
        .setUrl("https://api.example.com/register")
        .addHeader("Content-Type", "application/json")
        .setBody(json_body)
        .setTimeout(45)
        .build();
    cout << req4 << endl << endl;

    cout << "✨ Builder Pattern은 복잡한 객체를 직관적으로 생성할 수 있게 함!" << endl;

    return 0;
}
