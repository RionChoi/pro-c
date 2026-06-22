// homework.c — 에러 처리 연습: 다양한 시나리오 처리
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define MAX 100

static void trim_newline(char *s) {
    s[strcspn(s, "\n")] = '\0';
}

// 배열에서 인덱스 조회 — 경계 검사
static int safe_get(const int *arr, int size, int index) {
    assert(arr != NULL && size > 0);
    if (index < 0 || index >= size) {
        fprintf(stderr, "인덱스 오류: %d는 범위 [0, %d) 밖입니다.\n", index, size);
        errno = EINVAL;
        return -1;
    }
    return arr[index];
}

// 파일 전체 읽기 — 에러 처리
static int read_file_and_sum(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "파일 열기 실패 (%s): %s\n", filename, strerror(errno));
        return -1;
    }

    int sum = 0;
    int line_no = 0;
    char buf[128];

    while (fgets(buf, sizeof(buf), fp)) {
        line_no++;
        // 정수만 합산, 그 외 무시
        char *end;
        long val = strtol(buf, &end, 10);
        if (end != buf && isspace((unsigned char)*end) == 0 && *end != '\0') {
            // 정수가 아닌 경우 — strtol이 숫자로 해석 못함
            fprintf(stderr, "줄 %d: 정수 아님 — '%s'\n", line_no, buf);
            continue;
        }
        sum += (int)val;
    }

    if (ferror(fp)) {
        fprintf(stderr, "읽기 오류: %s\n", strerror(errno));
        fclose(fp);
        errno = EIO;
        return -1;
    }

    if (feof(fp) == 0) {
        fprintf(stderr, "알 수 없는 EOF 상태.\n");
    }

    fclose(fp);
    return sum;
}

int main(void) {
    // 1) assert 테스트
    printf("=== assert 테스트 ===\n");
    int data[] = {10, 20, 30, 40, 50};
    int sz = (int)(sizeof(data) / sizeof(data[0]));

    printf("배열[0] = %d\n", safe_get(data, sz, 0));
    printf("배열[4] = %d\n", safe_get(data, sz, 4));

    errno = 0;
    int bad = safe_get(data, sz, 10);
    if (errno == EINVAL) {
        printf("→ 인덱스 10: 예외 처리됨 (예상된 오류)\n");
    }

    // 2) 잘못된 파일
    printf("\n=== 파일 읽기 테스트 ===\n");
    int result = read_file_and_sum("nonexistent.txt");
    if (result < 0) {
        printf("→ 파일 없음: 예외 처리됨 (예상된 오류)\n");
    }

    // 3) 임시 파일 생성 후 읽기
    printf("\n=== 정상 파일 처리 ===\n");
    FILE *tmp = fopen("/tmp/sum_test.txt", "w");
    if (!tmp) {
        fprintf(stderr, "임시 파일 생성 실패: %s\n", strerror(errno));
        return 1;
    }
    fprintf(tmp, "10\n20\n30\nabc\n40\n");
    fclose(tmp);

    result = read_file_and_sum("/tmp/sum_test.txt");
    if (result >= 0) {
        printf("파일 합계: %d (예상: 100)\n", result);
    }

    printf("\n모든 테스트 완료.\n");
    return 0;
}
