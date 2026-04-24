/*
 * homework2.c — 단어 빈도 카운터
 *
 * 사용법: wc2 [-n] [-c] [파일]
 *   -n: 숫자 순으로 정렬 (기본: 빈도순)
 *   -c: 대소문자 무시
 *   파일省略: stdin에서 읽음
 *
 * 예시:
 *   ./wc2 article.txt
 *   cat essay.txt | ./wc2 -n
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_WORDS 10000
#define WORD_BUF  256

typedef struct {
    char word[WORD_BUF];
    int  count;
} WordEntry;

static WordEntry entries[MAX_WORDS];
static int entry_count = 0;
static bool ignore_case = false;
static bool sort_by_num = false;

static void add_word(const char *word)
{
    if (ignore_case) {
        for (int i = 0; word[i]; i++) {
            char lower[WORD_BUF];
            strncpy(lower, word, WORD_BUF);
            for (int j = 0; lower[j]; j++)
                lower[j] = (char)tolower((unsigned char)lower[j]);
            word = lower;
            break;
        }
    }

    for (int i = 0; i < entry_count; i++) {
        if (strcmp(entries[i].word, word) == 0) {
            entries[i].count++;
            return;
        }
    }
    if (entry_count >= MAX_WORDS) return;
    strncpy(entries[entry_count].word, word, WORD_BUF - 1);
    entries[entry_count].word[WORD_BUF - 1] = '\0';
    entries[entry_count].count = 1;
    entry_count++;
}

static int cmp_count(const void *a, const void *b)
{
    return ((WordEntry *)b)->count - ((WordEntry *)a)->count;
}

static int cmp_alpha(const void *a, const void *b)
{
    return strcmp(((WordEntry *)a)->word, ((WordEntry *)b)->word);
}

static int cmp_num(const void *a, const void *b)
{
    /* 단어 내 숫자를 비교하기 위한 임시 파싱 */
    int na = 0, nb = 0;
    const char *pa = ((WordEntry *)a)->word;
    const char *pb = ((WordEntry *)b)->word;
    while (*pa && isdigit((unsigned char)*pa)) na = na * 10 + (*pa++ - '0');
    while (*pb && isdigit((unsigned char)*pb)) nb = nb * 10 + (*pb++ - '0');
    return na - nb;
}

static void print_result(void)
{
    if (sort_by_num)
        qsort(entries, entry_count, sizeof(WordEntry), cmp_count);
    else
        qsort(entries, entry_count, sizeof(WordEntry), cmp_alpha);

    printf("\n%-30s %s\n", "단어", "빈도");
    printf("------------------------------\n");
    for (int i = 0; i < entry_count; i++)
        printf("%-30s %d\n", entries[i].word, entries[i].count);
    printf("------------------------------\n");
    printf("총 고유 단어 수: %d\n", entry_count);
}

static void process_file(FILE *fp)
{
    char word[WORD_BUF];
    int pos = 0;
    int c;

    while ((c = fgetc(fp)) != EOF) {
        if (isalnum((unsigned char)c) || c == '_') {
            if (pos < WORD_BUF - 1)
                word[pos++] = (char)c;
        } else {
            if (pos > 0) {
                word[pos] = '\0';
                add_word(word);
                pos = 0;
            }
        }
    }
    if (pos > 0) {
        word[pos] = '\0';
        add_word(word);
    }
}

int main(int argc, char *argv[])
{
    FILE *fp = stdin;
    bool file_given = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0)
            ignore_case = true;
        else if (strcmp(argv[i], "-n") == 0)
            sort_by_num = true;
        else {
            fp = fopen(argv[i], "r");
            if (fp == NULL) {
                fprintf(stderr, "오류: '%s'을(를) 열 수 없습니다.\n", argv[i]);
                return 1;
            }
            file_given = true;
        }
    }

    process_file(fp);
    if (file_given) fclose(fp);

    print_result();
    return 0;
}
