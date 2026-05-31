// 네트워크 숫자 맞추기 게임 (1~100, 최대 7번)
// 서버(부모): 랜덤 숫자 생성, UP/DOWN/정답 힌트 전송
// 클라이언트(자식): 이진 탐색 전략으로 최적 추측
// 수학적 근거: log₂(100) ≈ 6.64 → 7번 안에 반드시 정답

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <array>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>

static constexpr int    PORT      = 9082;
static constexpr int    BACKLOG   = 1;
static constexpr size_t BUFSIZE   = 256;
static constexpr int    MAX_TRIES = 7;
static constexpr int    MIN_NUM   = 1;
static constexpr int    MAX_NUM   = 100;

// ─── 서버 ────────────────────────────────────────────────────────────────────

static void run_server() {
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) {
        std::cerr << "socket: " << strerror(errno) << "\n";
        return;
    }

    int opt = 1;
    (void)setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(static_cast<uint16_t>(PORT));

    if (bind(sfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "bind: " << strerror(errno) << "\n";
        close(sfd);
        return;
    }
    (void)listen(sfd, BACKLOG);

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    int secret = MIN_NUM + std::rand() % (MAX_NUM - MIN_NUM + 1);

    std::cout << "[서버] 게임 시작! 비밀 숫자: " << secret
              << " (클라이언트는 모름)\n";
    std::cout.flush();

    sockaddr_in cli{};
    socklen_t cli_len = sizeof(cli);
    int cfd = accept(sfd, reinterpret_cast<sockaddr*>(&cli), &cli_len);
    if (cfd < 0) {
        close(sfd);
        return;
    }

    // 게임 안내 메시지 전송
    std::string welcome =
        "=== 숫자 맞추기 게임 ===\n"
        "1~100 사이 숫자를 맞추세요! (기회: "
        + std::to_string(MAX_TRIES) + "번)\n";
    (void)send(cfd, welcome.c_str(), welcome.size(), 0);

    std::array<char, BUFSIZE> buf{};
    int tries = 0;

    while (tries < MAX_TRIES) {
        ssize_t n = recv(cfd, buf.data(), buf.size() - 1, 0);
        if (n <= 0) break;
        buf[static_cast<size_t>(n)] = '\0';

        int guess = std::atoi(buf.data());
        ++tries;

        std::string resp;
        if (guess == secret) {
            resp = "정답! " + std::to_string(tries) + "번 만에 맞췄습니다!\n";
            (void)send(cfd, resp.c_str(), resp.size(), 0);
            std::cout << "[서버] 정답 " << secret << " — "
                      << tries << "번 만에 맞춤\n";
            break;
        }

        int remaining = MAX_TRIES - tries;
        if (guess < secret) {
            resp = "UP!   (더 큰 수, 남은 기회: " + std::to_string(remaining) + ")\n";
        } else {
            resp = "DOWN! (더 작은 수, 남은 기회: " + std::to_string(remaining) + ")\n";
        }

        if (tries == MAX_TRIES) {
            resp += "게임 오버! 정답은 " + std::to_string(secret) + "이었습니다.\n";
            std::cout << "[서버] 게임 오버 — 클라이언트 실패\n";
        }
        (void)send(cfd, resp.c_str(), resp.size(), 0);
    }

    close(cfd);
    close(sfd);
}

// ─── 클라이언트 (이진 탐색 전략) ─────────────────────────────────────────────

static void run_client() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "socket: " << strerror(errno) << "\n";
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(static_cast<uint16_t>(PORT));
    (void)inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "connect: " << strerror(errno) << "\n";
        close(sock);
        return;
    }

    // 안내 메시지 수신
    std::array<char, BUFSIZE> buf{};
    ssize_t n = recv(sock, buf.data(), buf.size() - 1, 0);
    if (n > 0) {
        buf[static_cast<size_t>(n)] = '\0';
        std::cout << buf.data();
        std::cout.flush();
    }

    // 이진 탐색: 1~100 범위를 절반씩 좁힘
    int lo = MIN_NUM;
    int hi = MAX_NUM;

    for (int tries = 0; tries < MAX_TRIES; ++tries) {
        int guess = lo + (hi - lo) / 2;
        std::cout << "[클라이언트] 추측 #" << (tries + 1) << ": " << guess << "\n";
        std::cout.flush();

        std::string g = std::to_string(guess) + "\n";
        (void)send(sock, g.c_str(), g.size(), 0);

        buf.fill('\0');
        n = recv(sock, buf.data(), buf.size() - 1, 0);
        if (n <= 0) break;
        buf[static_cast<size_t>(n)] = '\0';

        std::string resp(buf.data());
        std::cout << "[서버 응답] " << resp;
        std::cout.flush();

        if (resp.find("정답") != std::string::npos) break;
        if (resp.find("UP") != std::string::npos)   lo = guess + 1;
        if (resp.find("DOWN") != std::string::npos) hi = guess - 1;
        if (resp.find("게임 오버") != std::string::npos) break;
    }

    close(sock);
}

// ─── main ────────────────────────────────────────────────────────────────────

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "fork 실패\n";
        return 1;
    }

    if (pid == 0) {
        sleep(1);       // 서버 listen() 완료 대기
        run_client();
    } else {
        run_server();
        (void)waitpid(pid, nullptr, 0);
    }

    return 0;
}
