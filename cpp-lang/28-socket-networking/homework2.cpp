// UDP 간단 채팅 (단방향 브로드캐스트 시뮬레이션)
// 사용법:
//   서버: ./homework2 --server [포트]        (기본 포트: 9001)
//   클라이언트: ./homework2 --client [포트]  (기본 포트: 9001)
//
// 클라이언트가 메시지를 서버로 보내면, 서버가 모든 알려진 클라이언트에게 전달한다.
// select()로 stdin과 소켓을 동시에 감시한다.

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <map>
#include <string>

static constexpr int DEFAULT_PORT = 9001;
static constexpr int BUF_SIZE     = 1024;

// sockaddr_in 비교용 키
struct AddrKey {
    uint32_t ip;
    uint16_t port;
    bool operator<(const AddrKey& o) const {
        return ip < o.ip || (ip == o.ip && port < o.port);
    }
};

static AddrKey to_key(const sockaddr_in& a) {
    return {a.sin_addr.s_addr, a.sin_port};
}

// ─── 서버 ────────────────────────────────────────────────────────────────────

static int run_server(int port) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) { std::cerr << "socket() 실패\n"; return 1; }

    sockaddr_in saddr{};
    saddr.sin_family      = AF_INET;
    saddr.sin_port        = htons(static_cast<uint16_t>(port));
    saddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, reinterpret_cast<sockaddr*>(&saddr), sizeof(saddr)) < 0) {
        std::cerr << "bind() 실패\n"; close(fd); return 1;
    }

    std::cout << "[서버] UDP 포트 " << port << " 에서 대기 중...\n";

    std::map<AddrKey, sockaddr_in> clients;
    char buf[BUF_SIZE];

    while (true) {
        fd_set rdfds;
        FD_ZERO(&rdfds);
        FD_SET(fd, &rdfds);

        int ready = select(fd + 1, &rdfds, nullptr, nullptr, nullptr);
        if (ready < 0) break;

        if (!FD_ISSET(fd, &rdfds)) continue;

        sockaddr_in sender{};
        socklen_t   slen = sizeof(sender);
        ssize_t     n    = recvfrom(fd, buf, BUF_SIZE - 1, 0,
                                    reinterpret_cast<sockaddr*>(&sender), &slen);
        if (n <= 0) continue;
        buf[n] = '\0';

        // 신규 클라이언트 등록
        auto key = to_key(sender);
        if (clients.find(key) == clients.end()) {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &sender.sin_addr, ip, sizeof(ip));
            std::cout << "[서버] 신규 클라이언트: " << ip
                      << ":" << ntohs(sender.sin_port) << "\n";
            clients[key] = sender;
        }

        // 모든 클라이언트에게 전달 (브로드캐스트)
        for (auto& [k, caddr] : clients) {
            sendto(fd, buf, static_cast<size_t>(n), 0,
                   reinterpret_cast<sockaddr*>(&caddr), sizeof(caddr));
        }

        std::cout << "[서버] 전달: " << buf;
    }

    close(fd);
    return 0;
}

// ─── 클라이언트 ──────────────────────────────────────────────────────────────

static int run_client(int port) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) { std::cerr << "socket() 실패\n"; return 1; }

    // 수신을 위해 임의 포트에 바인드
    sockaddr_in laddr{};
    laddr.sin_family      = AF_INET;
    laddr.sin_port        = 0;
    laddr.sin_addr.s_addr = INADDR_ANY;
    bind(fd, reinterpret_cast<sockaddr*>(&laddr), sizeof(laddr));

    sockaddr_in saddr{};
    saddr.sin_family = AF_INET;
    saddr.sin_port   = htons(static_cast<uint16_t>(port));
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr);

    // 서버에 첫 메시지를 보내 등록
    const char* hello = "[입장]\n";
    sendto(fd, hello, strlen(hello), 0,
           reinterpret_cast<sockaddr*>(&saddr), sizeof(saddr));

    std::cout << "[클라이언트] 채팅 시작 (종료: Ctrl+D):\n";

    char buf[BUF_SIZE];

    while (true) {
        fd_set rdfds;
        FD_ZERO(&rdfds);
        FD_SET(fd, &rdfds);
        FD_SET(STDIN_FILENO, &rdfds);
        int maxfd = std::max(fd, STDIN_FILENO);

        int ready = select(maxfd + 1, &rdfds, nullptr, nullptr, nullptr);
        if (ready < 0) break;

        // 소켓 수신
        if (FD_ISSET(fd, &rdfds)) {
            sockaddr_in from{};
            socklen_t   flen = sizeof(from);
            ssize_t     n    = recvfrom(fd, buf, BUF_SIZE - 1, 0,
                                        reinterpret_cast<sockaddr*>(&from), &flen);
            if (n > 0) {
                buf[n] = '\0';
                std::cout << "[수신] " << buf;
            }
        }

        // 키보드 입력
        if (FD_ISSET(STDIN_FILENO, &rdfds)) {
            std::string line;
            if (!std::getline(std::cin, line)) break;
            line += '\n';
            sendto(fd, line.c_str(), line.size(), 0,
                   reinterpret_cast<sockaddr*>(&saddr), sizeof(saddr));
        }
    }

    const char* bye = "[퇴장]\n";
    sendto(fd, bye, strlen(bye), 0,
           reinterpret_cast<sockaddr*>(&saddr), sizeof(saddr));
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
