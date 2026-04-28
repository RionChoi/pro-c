#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

int main(void) {
    std::cout << "=== 포인터 크레이지 ===\n";
    std::cout << "W/A/S/D로 움직여 * 아이템 5개를 모두 수집하세요!\n";
    std::cout << "30턴 이내 클리어!\n\n";

    const int WIDTH = 20;
    const int HEIGHT = 10;
    const int TOTAL_ITEMS = 5;
    const int MAX_TURNS = 30;

    // 보드 초기화
    std::vector<char> board(WIDTH * HEIGHT, '.');

    // 플레이어 위치 (포인터로 관리)
    int player_idx = WIDTH * (HEIGHT / 2) + (WIDTH / 2);
    char* player_ptr = board.data() + player_idx;
    *player_ptr = '@';

    // 아이템 배치
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    std::vector<int> item_indices;
    while (item_indices.size() < TOTAL_ITEMS) {
        int idx = std::rand() % (WIDTH * HEIGHT);
        if (idx != player_idx && board[idx] != '*') {
            board[idx] = '*';
            item_indices.push_back(idx);
        }
    }

    int collected = 0;
    int turn = 0;

    auto print_board = [&]() {
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                std::cout << board[y * WIDTH + x];
            }
            std::cout << "\n";
        }
        std::cout << "아이템: " << collected << "/" << TOTAL_ITEMS
                  << "  턴: " << turn << "/" << MAX_TURNS << "\n";
    };

    while (turn < MAX_TURNS && collected < TOTAL_ITEMS) {
        print_board();

        std::cout << "\n이동 (W/A/S/D): ";
        char move;
        std::cin >> move;

        // 이전 위치 지우기
        *player_ptr = '.';

        int new_idx = player_idx;
        if (move == 'W' || move == 'w') new_idx -= WIDTH;
        else if (move == 'S' || move == 's') new_idx += WIDTH;
        else if (move == 'A' || move == 'a') new_idx -= 1;
        else if (move == 'D' || move == 'd') new_idx += 1;
        else continue;

        // 경계 체크
        if (new_idx < 0 || new_idx >= WIDTH * HEIGHT) {
            *player_ptr = '@';
            std::cout << "벽에 부딪혔습니다!\n";
            continue;
        }
        if ((player_idx % WIDTH == 0 && new_idx % WIDTH == WIDTH - 1) ||
            (player_idx % WIDTH == WIDTH - 1 && new_idx % WIDTH == 0)) {
            *player_ptr = '@';
            std::cout << "벽에 부딪혔습니다!\n";
            continue;
        }

        // 아이템 수집
        if (*(board.data() + new_idx) == '*') {
            ++collected;
            std::cout << "아이템 수집! (" << collected << "/" << TOTAL_ITEMS << ")\n";
        }

        player_idx = new_idx;
        player_ptr = board.data() + player_idx;
        *player_ptr = '@';

        ++turn;
    }

    std::cout << "\n=== 최종 결과 ===\n";
    if (collected >= TOTAL_ITEMS) {
        std::cout << "클리어! " << turn << "턴 만에 모든 아이템을 수집했습니다!\n";
    } else {
        std::cout << "실패! " << collected << "/" << TOTAL_ITEMS << "개만 수집했습니다.\n";
    }

    return 0;
}
