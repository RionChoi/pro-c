/*
 * homework.c — 파일 복사 유틸 (cp)
 *
 * 사용법: cp [-v] [-i] <원본> <사본>
 *   -v: 상세 출력 모드
 *   -i: 덮어쓰기 전 확인
 *
 * 예시:
 *   ./cp -v input.txt output.txt
 *   ./cp -i data.bin backup.bin
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUF_SIZE 4096

static bool verbose = false;
static bool interactive = false;

static void print_usage(const char *prog)
{
    fprintf(stderr, "사용법: %s [-v] [-i] <원본> <사본>\n", prog);
    fprintf(stderr, "  -v  상세 출력 모드\n");
    fprintf(stderr, "  -i  덮어쓰기 전 확인\n");
}

static bool file_exists(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (fp) { fclose(fp); return true; }
    return false;
}

static bool confirm_overwrite(const char *dst)
{
    char answer[8];
    fprintf(stderr, "'%s'을(를) 덮어쓰시겠습니까? (y/N): ", dst);
    if (fgets(answer, sizeof(answer), stdin) == NULL)
        return false;
    return answer[0] == 'y' || answer[0] == 'Y';
}

static int copy_file(const char *src, const char *dst)
{
    FILE *in  = fopen(src, "rb");
    if (in == NULL) {
        fprintf(stderr, "오류: '%s'을(를) 열 수 없습니다.\n", src);
        return 1;
    }

    /* 덮어쓰기 확인 */
    if (interactive && file_exists(dst)) {
        if (!confirm_overwrite(dst)) {
            fclose(in);
            fprintf(stderr, "취소되었습니다.\n");
            return 2;
        }
    }

    FILE *out = fopen(dst, "wb");
    if (out == NULL) {
        fprintf(stderr, "오류: '%s'을(를) 생성할 수 없습니다.\n", dst);
        fclose(in);
        return 1;
    }

    unsigned char buf[BUF_SIZE];
    size_t bytes_read;
    size_t total = 0;

    while ((bytes_read = fread(buf, 1, BUF_SIZE, in)) > 0) {
        size_t written = fwrite(buf, 1, bytes_read, out);
        if (written != bytes_read) {
            fprintf(stderr, "오류: 쓰기 실패\n");
            fclose(in); fclose(out);
            return 1;
        }
        total += bytes_read;
    }

    if (ferror(in)) {
        fprintf(stderr, "오류: 읽기 실패\n");
        fclose(in); fclose(out);
        return 1;
    }

    fclose(in);
    fclose(out);

    if (verbose)
        printf("'%s' -> '%s' (%zu bytes)\n", src, dst, total);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    /* 옵션 파싱 */
    int opt_end = 1;
    for (int i = 1; i < argc - 2; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = true;
            opt_end++;
        } else if (strcmp(argv[i], "-i") == 0) {
            interactive = true;
            opt_end++;
        } else {
            break;
        }
    }

    char *src = argv[opt_end];
    char *dst = argv[opt_end + 1];

    if (src == NULL || dst == NULL) {
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(src, dst) == 0) {
        fprintf(stderr, "오류: 원본과 대상이 같습니다.\n");
        return 1;
    }

    return copy_file(src, dst);
}
