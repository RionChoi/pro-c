#include <stdio.h>
#include <string.h>

void reverse_string(char *s) {
    int len = (int)strlen(s);
    if (len > 0 && s[len - 1] == '\n') {
        s[len - 1] = '\0';
        len--;
    }

    for (int i = 0; i < len / 2; i++) {
        char tmp = s[i];
        s[i] = s[len - 1 - i];
        s[len - 1 - i] = tmp;
    }
}

int main(void) {
    char text[100];

    printf("문자열 입력: ");
    if (fgets(text, sizeof(text), stdin) == NULL) {
        printf("입력 오류\n");
        return 1;
    }

    reverse_string(text);
    printf("뒤집은 문자열: %s\n", text);

    return 0;
}
