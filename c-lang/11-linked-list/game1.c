/*
 * 세션 11 - 미니 게임: 단어 추리 게임 (Word Guess)
 * 연결 리스트로 단어 목록 관리, "_" 기반 숨김 표시
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_WORD_LEN 64
#define MAX_GUESS    6

typedef struct WordNode {
    char word[MAX_WORD_LEN];
    char hint[MAX_WORD_LEN];
    struct WordNode *next;
} WordNode;

void add_word(WordNode **head, const char *word, const char *hint) {
    WordNode *new_node = (WordNode *)malloc(sizeof(WordNode));
    if (new_node == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        return;
    }
    strncpy(new_node->word, word, MAX_WORD_LEN - 1);
    new_node->word[MAX_WORD_LEN - 1] = '\0';
    strncpy(new_node->hint, hint, MAX_WORD_LEN - 1);
    new_node->hint[MAX_WORD_LEN - 1] = '\0';
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
        return;
    }
    WordNode *curr = *head;
    while (curr->next != NULL) curr = curr->next;
    curr->next = new_node;
}

WordNode *get_nth(WordNode *head, int n) {
    int idx = 0;
    while (head != NULL && idx < n) {
        head = head->next;
        idx++;
    }
    return head;
}

int list_size(WordNode *head) {
    int count = 0;
    while (head != NULL) {
        count++;
        head = head->next;
    }
    return count;
}

void free_words(WordNode *head) {
    while (head != NULL) {
        WordNode *next = head->next;
        free(head);
        head = next;
    }
}

char *strupper(char *s) {
    for (char *p = s; *p; p++) *p = (char)toupper((unsigned char)*p);
    return s;
}

int main(void) {
    WordNode *word_list = NULL;
    add_word(&word_list, "algorithm", "문제를 해결하기 위한 단계적 절차");
    add_word(&word_list, "compiler", "소스코드를 기계어로 번역하는 프로그램");
    add_word(&word_list, "variable", "값을 저장하는 메모리 공간");
    add_word(&word_list, "function", "특정 작업을 수행하는 코드 블록");
    add_word(&word_list, "pointer", "메모리 주소를 저장하는 변수");
    add_word(&word_list, "recursion", "함수가 자기 자신을 호출하는 기법");
    add_word(&word_list, "compiler", "고급 언어를 저급 언어로 변환");

    srand((unsigned int)time(NULL));
    int n = list_size(word_list);
    WordNode *target = get_nth(word_list, rand() % n);

    int word_len = (int)strlen(target->word);
    int revealed[MAX_WORD_LEN] = {0};
    int chances = MAX_GUESS;

    printf("=== 단어 추리 게임 ===\n");
    printf("힌트: %s\n", target->hint);
    printf("_%c", ' ');

    printf("\n\n남은 기회: %d\n", chances);

    while (chances > 0) {
        printf("글자 하나를 입력하세요: ");
        char guess[8];
        if (fgets(guess, sizeof(guess), stdin) == NULL) {
            fprintf(stderr, "입력 오류\n");
            break;
        }
        guess[strcspn(guess, "\n")] = '\0';
        if (strlen(guess) == 0) {
            printf("글자를 입력하세요.\n");
            continue;
        }
        char g = (char)tolower((unsigned char)guess[0]);

        int hit = 0;
        for (int i = 0; i < word_len; i++) {
            if (target->word[i] == g && !revealed[i]) {
                revealed[i] = 1;
                hit = 1;
            }
        }

        if (!hit) {
            chances--;
            printf("없습니다! 남은 기회: %d\n", chances);
        } else {
            printf("맞았습니다!\n");
        }

        /* 현재 상태 출력 */
        printf("\n현재: ");
        int all_done = 1;
        for (int i = 0; i < word_len; i++) {
            if (revealed[i]) {
                printf("%c ", (char)toupper((unsigned char)target->word[i]));
            } else {
                printf("_ ");
                all_done = 0;
            }
        }
        printf("\n");

        if (all_done) {
            printf("\n축하합니다! 정답: %s\n", strupper(target->word));
            free_words(word_list);
            return 0;
        }
    }

    printf("\n실패! 정답은: %s\n", strupper(target->word));
    free_words(word_list);
    return 0;
}
