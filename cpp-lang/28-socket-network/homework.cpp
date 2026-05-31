// TCP Echo 서버/클라이언트 — fork()로 단일 파일에서 구현
// 부모 프로세스: 에코 서버 (클라이언트 1개 수용 후 종료)
// 자식 프로세스: 3개 메시지를 전송하고 에코 응답 확인

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

static constexpr int     PORT    = 9080;
static constexpr int     BACKLOG = 1;
static constexpr size_t  BUFSIZE = 1024;

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

    std::cout << "[서버] 포트 " << PORT << " 대기 중...\n";
    std::cout.flush();

    sockaddr_in cli{};
    socklen_t cli_len = sizeof(cli);
    int cfd = accept(sfd, reinterpret_cast<sockaddr*>(&cli), &cli_len);
    if (cfd < 0) {
        std::cerr << "accept: " << strerror(errno) << "\n";
        close(sfd);
        return;
    }
    std::cout << "[서버] 클라이언트 연결됨\n";
    std::cout.flush();

    std::array<char, BUFSIZE> buf{};
    for (;;) {
        ssize_t n = recv(cfd, buf.data(), buf.size() - 1, 0);
        if (n <= 0) break;
        buf[static_cast<size_t>(n)] = '\0';
        std::cout << "[서버] 수신: " << buf.data();
        std::cout.flush();
        (void)send(cfd, buf.data(), static_cast<size_t>(n), 0);
    }

    close(cfd);
    close(sfd);
    std::cout << "[서버] 종료\n";
}

static void run_client() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "socket: " << strerror(errno) << "\n";
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(static_cast<uint16_t>(PORT));
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
        std::cerr << "inet_pton 실패\n";
        close(sock);
        return;
    }

    if (connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "connect: " << strerror(errno) << "\n";
        close(sock);
        return;
    }
    std::cout << "[클라이언트] 서버 연결 성공\n";
    std::cout.flush();

    const char* msgs[] = {
        "안녕하세요, TCP 서버!\n",
        "소켓 프로그래밍 학습 중!\n",
        "에코 테스트 완료!\n",
    };

    std::array<char, BUFSIZE> buf{};
    for (const char* m : msgs) {
        size_t len = strlen(m);
        (void)send(sock, m, len, 0);

        ssize_t n = recv(sock, buf.data(), buf.size() - 1, 0);
        if (n > 0) {
            buf[static_cast<size_t>(n)] = '\0';
            std::cout << "[클라이언트] 에코: " << buf.data();
            std::cout.flush();
        }
    }

    close(sock);
}

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "fork 실패\n";
        return 1;
    }

    if (pid == 0) {
        // 자식: 서버가 준비될 시간을 주고 클라이언트 실행
        sleep(1);
        run_client();
    } else {
        // 부모: 서버 실행 후 자식 대기
        run_server();
        (void)waitpid(pid, nullptr, 0);
    }

    return 0;
}
