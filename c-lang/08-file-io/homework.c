/*
 * 세션 08 — 과제 1: 텍스트 파일 정수 합계 계산기
 *
 * 기능:
 *   - 파일명을 입력받아 파일을 연다
 *   - 파일 내 모든 정수를 읽어 합계/개수 출력
 *   - 파일이 없거나 정수가 없으면 오류 처리
 */

#include <stdio.h>
#include <string.h>

#define NAME_SIZE 256

int main(void) {
    char filename[NAME_SIZE];

    printf("읽을 파일명을 입력하세요: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        fprintf(stderr, "입력 오류\n");
        return 1;
    }

    filename[strcspn(filename, "\n")] = '\0';

    if (filename[0] == '\0') {
        fprintf(stderr, "파일명이 비어 있습니다.\n");
        return 1;
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "파일을 열 수 없습니다: %s\n", filename);
        return 1;
    }

    long long sum = 0;
    int count = 0;
    int value;

    while (fscanf(fp, "%d", &value) == 1) {
        sum += (long long)value;
        count++;
    }

    fclose(fp);

    if (count == 0) {
        fprintf(stderr, "파일에 정수 데이터가 없습니다.\n");
        return 1;
    }

    printf("파일: %s\n", filename);
    printf("정수 개수: %d\n", count);
    printf("합계: %lld\n", sum);

    return 0;
}
