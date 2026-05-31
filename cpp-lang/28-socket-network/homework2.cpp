// Prefork 에코 서버 — accept() 후 fork()로 클라이언트마다 자식 프로세스 생성
// 메인 프로세스가 먼저 3개의 클라이언트 자식을 생성한 뒤 서버 루프를 실행

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <array>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

static constexpr int     PORT        = 9081;
static constexpr int     BACKLOG     = 5;
static constexpr size_t  BUFSIZE     = 256;
static constexpr int     NUM_CLIENTS = 3;

// ─── 서버 ────────────────────────────────────────────────────────────────────

static void handle_client(int cfd, int id) {
    std::cout << "[서버 자식 #" << id << "] 클라이언트 처리 시작\n";
    std::cout.flush();

    std::array<char, BUFSIZE> buf{};
    for (;;) {
        ssize_t n = recv(cfd, buf.data(), buf.size() - 1, 0);
        if (n <= 0) break;
        buf[static_cast<size_t>(n)] = '\0';
        std::cout << "[서버 자식 #" << id << "] 수신: " << buf.data();
        std::cout.flush();
        (void)send(cfd, buf.data(), static_cast<size_t>(n), 0);
    }

    close(cfd);
    std::cout << "[서버 자식 #" << id << "] 처리 완료\n";
    std::cout.flush();
}

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

    std::cout << "[서버] Prefork 에코 서버 시작 (포트 " << PORT << ")\n";
    std::cout.flush();

    pid_t child_pids[NUM_CLIENTS]{};
    sockaddr_in cli{};
    socklen_t   cli_len = sizeof(cli);

    for (int i = 0; i < NUM_CLIENTS; ++i) {
        int cfd = accept(sfd, reinterpret_cast<sockaddr*>(&cli), &cli_len);
        if (cfd < 0) {
            if (errno == EINTR) { --i; continue; }
            break;
        }

        pid_t cpid = fork();
        if (cpid < 0) {
            close(cfd);
            continue;
        }
        if (cpid == 0) {
            // 자식: 소켓 처리 후 종료
            close(sfd);
            handle_client(cfd, i + 1);
            _exit(0);
        }
        // 부모: cfd 닫고 자식 pid 저장
        close(cfd);
        child_pids[i] = cpid;
    }

    close(sfd);

    // 모든 자식 프로세스 회수 (좀비 방지)
    for (int i = 0; i < NUM_CLIENTS; ++i) {
        if (child_pids[i] > 0) {
            (void)waitpid(child_pids[i], nullptr, 0);
        }
    }
    std::cout << "[서버] 모든 클라이언트 처리 완료\n";
}

// ─── 클라이언트 ──────────────────────────────────────────────────────────────

static void client_proc(int id) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) _exit(1);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(static_cast<uint16_t>(PORT));
    (void)inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "[클라이언트 #" << id << "] connect 실패: "
                  << strerror(errno) << "\n";
        close(sock);
        _exit(1);
    }

    std::array<char, BUFSIZE> buf{};
    for (int i = 0; i < 2; ++i) {
        std::string msg = "클라이언트 #" + std::to_string(id)
                        + " — 메시지 " + std::to_string(i + 1) + "\n";
        (void)send(sock, msg.c_str(), msg.size(), 0);

        ssize_t n = recv(sock, buf.data(), buf.size() - 1, 0);
        if (n > 0) {
            buf[static_cast<size_t>(n)] = '\0';
            std::cout << "[클라이언트 #" << id << "] 에코: " << buf.data();
            std::cout.flush();
        }
    }
    close(sock);
}

// ─── main ────────────────────────────────────────────────────────────────────

int main() {
    // 클라이언트 자식 프로세스 생성 (서버 준비 후 접속)
    for (int i = 1; i <= NUM_CLIENTS; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            std::cerr << "fork 실패\n";
            return 1;
        }
        if (pid == 0) {
            sleep(1);           // 서버 listen() 완료 대기
            client_proc(i);
            _exit(0);
        }
    }

    // 메인 프로세스: 서버 실행
    run_server();

    // 클라이언트 자식 프로세스 회수
    for (int i = 0; i < NUM_CLIENTS; ++i) {
        (void)wait(nullptr);
    }

    return 0;
}
