/*
 * 세션 05 — 게임: 블랙잭 (간이 버전)
 *
 * 규칙:
 *   - 52장 카드 덱을 구조체 배열로 구현
 *   - 플레이어와 딜러가 카드를 뽑아 21에 가까운 쪽이 승리
 *   - 21 초과 시 버스트(패배)
 *   - 플레이어는 Hit(뽑기) 또는 Stand(멈추기) 선택
 *   - 딜러는 17 미만이면 자동으로 Hit
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DECK_SIZE 52

struct Card {
    char suit[10];   /* ♠ ♥ ♦ ♣ */
    char rank[4];    /* A, 2~10, J, Q, K */
    int  value;      /* 카드 점수 (A=11, J/Q/K=10) */
};

static struct Card deck[DECK_SIZE];
static int deck_index = 0;

/* 덱을 초기화한다 */
void init_deck(void) {
    const char *suits[] = {"Spade", "Heart", "Diamond", "Club"};
    const char *ranks[] = {"A","2","3","4","5","6","7","8","9","10","J","Q","K"};
    const int values[]  = {11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};

    int idx = 0;
    for (int s = 0; s < 4; s++) {
        for (int r = 0; r < 13; r++) {
            snprintf(deck[idx].suit, sizeof(deck[idx].suit), "%s", suits[s]);
            snprintf(deck[idx].rank, sizeof(deck[idx].rank), "%s", ranks[r]);
            deck[idx].value = values[r];
            idx++;
        }
    }
}

/* 덱을 셔플한다 (Fisher-Yates 알고리즘) */
void shuffle_deck(void) {
    for (int i = DECK_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        struct Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
    deck_index = 0;
}

/* 카드 한 장을 뽑는다 */
struct Card draw_card(void) {
    if (deck_index >= DECK_SIZE) {
        fprintf(stderr, "덱에 카드가 없습니다!\n");
        exit(1);
    }
    return deck[deck_index++];
}

/* 카드를 출력한다 */
void print_card(const struct Card *c) {
    printf("[%s %s]", c->suit, c->rank);
}

/* 핸드의 합계를 계산한다 (에이스 조정 포함) */
int calc_hand(const struct Card hand[], int count) {
    int sum = 0;
    int aces = 0;

    for (int i = 0; i < count; i++) {
        sum += hand[i].value;
        if (hand[i].rank[0] == 'A' && hand[i].rank[1] == '\0') {
            aces++;
        }
    }

    /* 에이스: 합이 21 초과이면 11→1로 조정 */
    while (sum > 21 && aces > 0) {
        sum -= 10;
        aces--;
    }

    return sum;
}

/* 핸드를 출력한다 */
void print_hand(const char *label, const struct Card hand[], int count) {
    printf("  %s: ", label);
    for (int i = 0; i < count; i++) {
        print_card(&hand[i]);
        if (i < count - 1) printf(" ");
    }
    printf(" (합계: %d)\n", calc_hand(hand, count));
}

int main(void) {
    srand((unsigned int)time(NULL));

    struct Card player_hand[12];
    struct Card dealer_hand[12];
    int player_count, dealer_count;
    char input[16];
    int wins = 0, losses = 0, draws = 0;

    printf("=== 블랙잭 게임 ===\n");
    printf("21에 가장 가깝게! 초과하면 버스트!\n");

    while (1) {
        /* 새 라운드 */
        init_deck();
        shuffle_deck();
        player_count = 0;
        dealer_count = 0;

        /* 초기 2장씩 배분 */
        player_hand[player_count++] = draw_card();
        dealer_hand[dealer_count++] = draw_card();
        player_hand[player_count++] = draw_card();
        dealer_hand[dealer_count++] = draw_card();

        printf("\n--- 새 라운드 ---\n");
        printf("  딜러: ");
        print_card(&dealer_hand[0]);
        printf(" [???]\n");
        print_hand("플레이어", player_hand, player_count);

        /* 플레이어 턴 */
        int player_bust = 0;
        while (1) {
            int sum = calc_hand(player_hand, player_count);
            if (sum == 21) {
                printf("  ★ 블랙잭! ★\n");
                break;
            }
            if (sum > 21) {
                printf("  ✗ 버스트! (합계: %d)\n", sum);
                player_bust = 1;
                break;
            }

            printf("\n  [H]it / [S]tand ? ");
            if (fgets(input, sizeof(input), stdin) == NULL) {
                return 1;
            }

            if (input[0] == 'H' || input[0] == 'h') {
                player_hand[player_count++] = draw_card();
                print_hand("플레이어", player_hand, player_count);
            } else if (input[0] == 'S' || input[0] == 's') {
                break;
            } else {
                printf("  H 또는 S를 입력하세요.\n");
            }
        }

        if (player_bust) {
            printf("  → 딜러 승리!\n");
            losses++;
        } else {
            /* 딜러 턴: 17 미만이면 계속 뽑기 */
            printf("\n  딜러 턴:\n");
            print_hand("딜러", dealer_hand, dealer_count);

            while (calc_hand(dealer_hand, dealer_count) < 17) {
                dealer_hand[dealer_count++] = draw_card();
                print_hand("딜러", dealer_hand, dealer_count);
            }

            int p_sum = calc_hand(player_hand, player_count);
            int d_sum = calc_hand(dealer_hand, dealer_count);

            printf("\n  === 결과 ===\n");
            printf("  플레이어: %d | 딜러: %d\n", p_sum, d_sum);

            if (d_sum > 21) {
                printf("  → 딜러 버스트! 플레이어 승리! 🎉\n");
                wins++;
            } else if (p_sum > d_sum) {
                printf("  → 플레이어 승리! 🎉\n");
                wins++;
            } else if (p_sum < d_sum) {
                printf("  → 딜러 승리!\n");
                losses++;
            } else {
                printf("  → 무승부!\n");
                draws++;
            }
        }

        printf("\n  [전적] %d승 %d패 %d무\n", wins, losses, draws);
        printf("\n  계속하시겠습니까? (Y/N): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        if (input[0] == 'N' || input[0] == 'n') {
            break;
        }
    }

    printf("\n최종 전적: %d승 %d패 %d무\n", wins, losses, draws);
    printf("게임을 종료합니다. 감사합니다!\n");
    return 0;
}
