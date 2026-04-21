/*
 * 세션 08 — 과제 2: 동적 배열 기반 파일 통계 분석기
 *
 * 기능:
 *   - 파일명을 입력받아 정수 데이터를 동적 배열에 저장
 *   - 초기 용량 10(calloc), 부족하면 2배씩 realloc 확장
 *   - 평균/최소/최대 출력
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_SIZE 256
#define INITIAL_CAPACITY 10

int main(void) {
    char filename[NAME_SIZE];

    printf("통계 분석할 파일명: ");
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

    int capacity = INITIAL_CAPACITY;
    int count = 0;
    int *numbers = (int *)calloc((size_t)capacity, sizeof(int));
    if (numbers == NULL) {
        fclose(fp);
        fprintf(stderr, "메모리 할당 실패\n");
        return 1;
    }

    int value;
    while (fscanf(fp, "%d", &value) == 1) {
        if (count == capacity) {
            int new_capacity = capacity * 2;
            int *tmp = (int *)realloc(numbers, (size_t)new_capacity * sizeof(int));
            if (tmp == NULL) {
                free(numbers);
                fclose(fp);
                fprintf(stderr, "메모리 재할당 실패\n");
                return 1;
            }
            numbers = tmp;
            capacity = new_capacity;
        }

        numbers[count] = value;
        count++;
    }

    fclose(fp);

    if (count == 0) {
        free(numbers);
        fprintf(stderr, "빈 파일 또는 정수 데이터 없음\n");
        return 1;
    }

    long long sum = 0;
    int min_value = numbers[0];
    int max_value = numbers[0];

    for (int i = 0; i < count; i++) {
        const int x = numbers[i];
        sum += (long long)x;
        if (x < min_value) {
            min_value = x;
        }
        if (x > max_value) {
            max_value = x;
        }
    }

    const double average = (double)sum / (double)count;

    printf("파일: %s\n", filename);
    printf("데이터 수: %d\n", count);
    printf("합계: %lld\n", sum);
    printf("평균: %.2f\n", average);
    printf("최소: %d\n", min_value);
    printf("최대: %d\n", max_value);

    free(numbers);
    return 0;
}
