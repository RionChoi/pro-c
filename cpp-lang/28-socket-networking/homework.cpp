// TCP 에코 서버 & 클라이언트
// 사용법:
//   서버: ./homework --server [포트]        (기본 포트: 9000)
//   클라이언트: ./homework --client [포트]  (기본 포트: 9000)
//
// 서버는 select()로 여러 클라이언트를 단일 스레드에서 처리한다.
// 클라이언트가 보낸 메시지를 그대로 되돌려 준다.

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

static constexpr int DEFAULT_PORT = 9000;
static constexpr int BACKLOG      = 10;
static constexpr int BUF_SIZE     = 1024;

// ─── 헬퍼 ────────────────────────────────────────────────────────────────────

static int make_server_socket(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cerr << "socket() 실패\n";
        return -1;
    }

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(static_cast<uint16_t>(port));
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "bind() 실패\n";
        close(fd);
        return -1;
    }
    if (listen(fd, BACKLOG) < 0) {
        std::cerr << "listen() 실패\n";
        close(fd);
        return -1;
    }
    return fd;
}

static int make_client_socket(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cerr << "socket() 실패\n";
        return -1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(static_cast<uint16_t>(port));
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "connect() 실패 — 서버가 실행 중인지 확인하세요.\n";
        close(fd);
        return -1;
    }
    return fd;
}

// ─── 서버 ────────────────────────────────────────────────────────────────────

static int run_server(int port) {
    int listen_fd = make_server_socket(port);
    if (listen_fd < 0) return 1;

    std::cout << "[서버] 포트 " << port << " 에서 대기 중...\n";

    std::vector<int> clients;
    char buf[BUF_SIZE];

    while (true) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(listen_fd, &readfds);
        int maxfd = listen_fd;

        for (int fd : clients) {
            FD_SET(fd, &readfds);
            maxfd = std::max(maxfd, fd);
        }

        int ready = select(maxfd + 1, &readfds, nullptr, nullptr, nullptr);
        if (ready < 0) {
            std::cerr << "select() 실패\n";
            break;
        }

        // 새 연결
        if (FD_ISSET(listen_fd, &readfds)) {
            sockaddr_in cli{};
            socklen_t   len = sizeof(cli);
            int         cfd = accept(listen_fd, reinterpret_cast<sockaddr*>(&cli), &len);
            if (cfd >= 0) {
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &cli.sin_addr, ip, sizeof(ip));
                std::cout << "[서버] 클라이언트 연결: " << ip
                          << ":" << ntohs(cli.sin_port) << " (fd=" << cfd << ")\n";
                clients.push_back(cfd);
            }
        }

        // 기존 클라이언트 데이터
        for (auto it = clients.begin(); it != clients.end(); ) {
            int cfd = *it;
            if (!FD_ISSET(cfd, &readfds)) { ++it; continue; }

            ssize_t n = recv(cfd, buf, BUF_SIZE - 1, 0);
            if (n <= 0) {
                std::cout << "[서버] 클라이언트 연결 종료 (fd=" << cfd << ")\n";
                close(cfd);
                it = clients.erase(it);
            } else {
                buf[n] = '\0';
                std::cout << "[서버] 수신: " << buf;
                send(cfd, buf, static_cast<size_t>(n), 0);  // 에코
                ++it;
            }
        }
    }

    for (int fd : clients) close(fd);
    close(listen_fd);
    return 0;
}

// ─── 클라이언트 ──────────────────────────────────────────────────────────────

static int run_client(int port) {
    int fd = make_client_socket(port);
    if (fd < 0) return 1;

    std::cout << "[클라이언트] 서버에 연결됨. 메시지를 입력하세요 (종료: Ctrl+D):\n";

    char buf[BUF_SIZE];
    std::string line;
    while (std::getline(std::cin, line)) {
        line += '\n';
        if (send(fd, line.c_str(), line.size(), 0) < 0) {
            std::cerr << "send() 실패\n";
            break;
        }
        ssize_t n = recv(fd, buf, BUF_SIZE - 1, 0);
        if (n <= 0) {
            std::cout << "[클라이언트] 서버 연결 종료\n";
            break;
        }
        buf[n] = '\0';
        std::cout << "[에코] " << buf;
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
