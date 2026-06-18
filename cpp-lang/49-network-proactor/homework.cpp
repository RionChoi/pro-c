// Session 49: C++ 네트워크 & Proactor 패턴 - 완료 큐 기반 이벤트 루프
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <utility>

class Proactor {
public:
    using Handler = std::function<void(std::string)>;

    void async_read(std::string source, Handler handler) {
        completions_.push([source = std::move(source), handler = std::move(handler)]() mutable {
            handler("read<" + source + ">");
        });
    }

    void async_write(std::string payload, Handler handler) {
        completions_.push([payload = std::move(payload), handler = std::move(handler)]() mutable {
            handler("wrote " + std::to_string(payload.size()) + " bytes");
        });
    }

    void run() {
        while (!completions_.empty()) {
            auto completion = std::move(completions_.front());
            completions_.pop();
            completion();
        }
    }

private:
    std::queue<std::function<void()>> completions_;
};

int main() {
    Proactor loop;

    loop.async_read("client request", [&](std::string data) {
        std::cout << "completion: " << data << '\n';
        loop.async_write("HTTP/2 response", [](std::string result) {
            std::cout << "completion: " << result << '\n';
        });
    });

    loop.async_read("metrics packet", [](std::string data) {
        std::cout << "completion: " << data << '\n';
    });

    loop.run();
}
