/*
 * 세션 07 — 게임: 동적 맵 보물 탐험
 *
 * 규칙:
 *   - 사용자가 맵 크기(N)를 정하면 NxN 맵을 동적 할당
 *   - 플레이어(P)는 (0,0)에서 시작, 보물(T)은 랜덤 위치
 *   - W/A/S/D로 이동, 벽 밖으로는 이동 불가
 *   - 이동 횟수와 방문 로그를 동적 배열로 관리(realloc)
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EMPTY_CELL '.'
#define PLAYER_CELL 'P'
#define TREASURE_CELL 'T'
#define MAX_MAP_SIZE 12

typedef struct {
    int row;
    int col;
} Position;

static void print_map(char **map, int size, Position player) {
    for (int r = 0; r < size; r++) {
        for (int c = 0; c < size; c++) {
            if (r == player.row && c == player.col) {
                printf("%c ", PLAYER_CELL);
            } else {
                printf("%c ", map[r][c]);
            }
        }
        printf("\n");
    }
}

static int inside_map(int size, Position p) {
    return p.row >= 0 && p.row < size && p.col >= 0 && p.col < size;
}

int main(void) {
    int size;

    printf("맵 크기를 입력하세요 (3~12): ");
    if (scanf("%d", &size) != 1) {
        fprintf(stderr, "입력 오류\n");
        return 1;
    }

    if (size < 3 || size > MAX_MAP_SIZE) {
        fprintf(stderr, "맵 크기는 3~12만 허용됩니다.\n");
        return 1;
    }

    while (getchar() != '\n') {
        /* 입력 버퍼 비우기 */
    }

    char **map = (char **)malloc((size_t)size * sizeof(char *));
    if (map == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        return 1;
    }

    for (int r = 0; r < size; r++) {
        map[r] = (char *)malloc((size_t)size * sizeof(char));
        if (map[r] == NULL) {
            fprintf(stderr, "메모리 할당 실패\n");
            for (int i = 0; i < r; i++) {
                free(map[i]);
            }
            free(map);
            return 1;
        }

        for (int c = 0; c < size; c++) {
            map[r][c] = EMPTY_CELL;
        }
    }

    srand((unsigned int)time(NULL));

    Position player = {0, 0};
    Position treasure;
    do {
        treasure.row = rand() % size;
        treasure.col = rand() % size;
    } while (treasure.row == 0 && treasure.col == 0);

    map[treasure.row][treasure.col] = TREASURE_CELL;

    int log_capacity = 8;
    int log_count = 0;
    Position *logs = (Position *)malloc((size_t)log_capacity * sizeof(Position));
    if (logs == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        for (int r = 0; r < size; r++) {
            free(map[r]);
        }
        free(map);
        return 1;
    }

    int moves = 0;
    char input[32];

    printf("\n=== 동적 맵 보물 탐험 ===\n");
    printf("W/A/S/D로 이동, Q로 종료\n\n");

    while (1) {
        print_map(map, size, player);

        if (player.row == treasure.row && player.col == treasure.col) {
            printf("\n보물을 찾았습니다! 총 이동 횟수: %d\n", moves);
            break;
        }

        printf("\n명령 입력: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "입력 오류\n");
            break;
        }

        char cmd = input[0];
        Position next = player;

        if (cmd == 'w' || cmd == 'W') {
            next.row -= 1;
        } else if (cmd == 's' || cmd == 'S') {
            next.row += 1;
        } else if (cmd == 'a' || cmd == 'A') {
            next.col -= 1;
        } else if (cmd == 'd' || cmd == 'D') {
            next.col += 1;
        } else if (cmd == 'q' || cmd == 'Q') {
            printf("게임을 종료합니다.\n");
            break;
        } else {
            printf("유효하지 않은 명령입니다.\n");
            continue;
        }

        if (!inside_map(size, next)) {
            printf("맵 밖으로 이동할 수 없습니다.\n");
            continue;
        }

        player = next;
        moves++;

        if (log_count == log_capacity) {
            int new_capacity = log_capacity * 2;
            Position *tmp = (Position *)realloc(logs, (size_t)new_capacity * sizeof(Position));
            if (tmp == NULL) {
                fprintf(stderr, "이동 로그 확장 실패\n");
                break;
            }
            logs = tmp;
            log_capacity = new_capacity;
        }

        logs[log_count] = player;
        log_count++;
    }

    if (log_count > 0) {
        printf("\n[이동 로그]\n");
        for (int i = 0; i < log_count; i++) {
            printf("%2d: (%d, %d)\n", i + 1, logs[i].row, logs[i].col);
        }
    }

    free(logs);

    for (int r = 0; r < size; r++) {
        free(map[r]);
    }
    free(map);

    return 0;
}
