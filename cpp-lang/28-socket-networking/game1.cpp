// 소켓 기반 숫자 맞히기 게임 (TCP)
// 사용법:
//   서버: ./game1 --server [포트]        (기본 포트: 9002)
//   클라이언트: ./game1 --client [포트]  (기본 포트: 9002)
//
// 서버가 1~100 사이 랜덤 숫자를 생성하고, 클라이언트가 추측한다.
// 서버는 "높음", "낮음", "정답!" 메시지를 보내고 시도 횟수를 추적한다.
// 클라이언트가 "quit"을 보내면 게임 종료.

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>

static constexpr int DEFAULT_PORT = 9002;
static constexpr int BACKLOG      = 1;
static constexpr int BUF_SIZE     = 256;
static constexpr int RANGE_MIN    = 1;
static constexpr int RANGE_MAX    = 100;

// ─── 서버 ────────────────────────────────────────────────────────────────────

static void send_line(int fd, const std::string& msg) {
    std::string out = msg + "\n";
    send(fd, out.c_str(), out.size(), 0);
}

static std::string recv_line(int fd) {
    char    buf[BUF_SIZE];
    ssize_t n = recv(fd, buf, BUF_SIZE - 1, 0);
    if (n <= 0) return "";
    buf[n] = '\0';
    std::string s(buf);
    // 줄 끝 제거
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r'))
        s.pop_back();
    return s;
}

static int run_server(int port) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { std::cerr << "socket() 실패\n"; return 1; }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(static_cast<uint16_t>(port));
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "bind() 실패\n"; close(listen_fd); return 1;
    }
    listen(listen_fd, BACKLOG);
    std::cout << "[서버] 포트 " << port << " 에서 클라이언트 대기 중...\n";

    // 한 클라이언트씩 처리
    while (true) {
        sockaddr_in cli{};
        socklen_t   len = sizeof(cli);
        int         cfd = accept(listen_fd, reinterpret_cast<sockaddr*>(&cli), &len);
        if (cfd < 0) continue;

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cli.sin_addr, ip, sizeof(ip));
        std::cout << "[서버] 클라이언트 연결: " << ip << "\n";

        std::srand(static_cast<unsigned>(std::time(nullptr)));
        int answer  = RANGE_MIN + std::rand() % (RANGE_MAX - RANGE_MIN + 1);
        int tries   = 0;

        send_line(cfd, "=== 숫자 맞히기 게임 ===");
        send_line(cfd, "1~100 사이 숫자를 맞혀보세요. (종료: quit)");

        bool playing = true;
        while (playing) {
            send_line(cfd, "추측: ");
            std::string guess_str = recv_line(cfd);
            if (guess_str.empty() || guess_str == "quit") {
                send_line(cfd, "게임을 종료합니다. 정답은 " + std::to_string(answer) + " 이었습니다.");
                playing = false;
                break;
            }

            int guess = 0;
            try {
                guess = std::stoi(guess_str);
            } catch (...) {
                send_line(cfd, "올바른 숫자를 입력하세요.");
                continue;
            }

            if (guess < RANGE_MIN || guess > RANGE_MAX) {
                send_line(cfd, "1~100 범위의 숫자를 입력하세요.");
                continue;
            }

            ++tries;
            if (guess < answer) {
                send_line(cfd, "더 높은 숫자입니다! (" + std::to_string(tries) + "번째 시도)");
            } else if (guess > answer) {
                send_line(cfd, "더 낮은 숫자입니다! (" + std::to_string(tries) + "번째 시도)");
            } else {
                send_line(cfd, "정답! " + std::to_string(tries) + "번 만에 맞혔습니다!");
                send_line(cfd, "다시 시작하려면 아무 키를 누르세요. 종료하려면 quit 입력.");
                std::string again = recv_line(cfd);
                if (again == "quit") {
                    send_line(cfd, "게임 종료. 수고하셨습니다!");
                    playing = false;
                } else {
                    std::srand(static_cast<unsigned>(std::time(nullptr)));
                    answer = RANGE_MIN + std::rand() % (RANGE_MAX - RANGE_MIN + 1);
                    tries  = 0;
                    send_line(cfd, "새 게임을 시작합니다!");
                }
            }
        }

        close(cfd);
        std::cout << "[서버] 클라이언트 연결 종료\n";
    }

    close(listen_fd);
    return 0;
}

// ─── 클라이언트 ──────────────────────────────────────────────────────────────

static int run_client(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { std::cerr << "socket() 실패\n"; return 1; }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(static_cast<uint16_t>(port));
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "connect() 실패 — 서버가 실행 중인지 확인하세요.\n";
        close(fd);
        return 1;
    }

    char buf[BUF_SIZE];

    while (true) {
        ssize_t n = recv(fd, buf, BUF_SIZE - 1, 0);
        if (n <= 0) break;
        buf[n] = '\0';
        std::cout << buf;
        std::cout.flush();

        // "추측: " 프롬프트가 오면 사용자 입력 대기
        std::string received(buf);
        if (received.find("추측: ") != std::string::npos ||
            received.find("아무 키") != std::string::npos) {
            std::string input;
            if (!std::getline(std::cin, input)) break;
            input += '\n';
            send(fd, input.c_str(), input.size(), 0);
        }
    }

    close(fd);
    return 0;
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main(int argc, char* argv[]) {
    int port = DEFAULT_PORT;

    if (argc < 2) {
        std::cerr << "사용법: " << argv[0] << " --server|--client [포트]\n";
        return 1;
    }

    if (argc >= 3) port = std::stoi(argv[2]);

    std::string mode = argv[1];
    if (mode == "--server") return run_server(port);
    if (mode == "--client") return run_client(port);

    std::cerr << "알 수 없는 모드: " << mode << "\n";
    return 1;
}
