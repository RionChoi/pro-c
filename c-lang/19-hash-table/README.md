# 세션 19: 해시 테이블 (Hash Table)

## 학습 목표
- 해시 테이블의 핵심 원리(해시 함수, 충돌 해결)를 이해한다.
- 분리 연결법(seperate chaining)을 사용하여 충돌을 처리한다.
- 키-값 쌍의 삽입, 검색, 삭제를 구현한다.
- 해시 테이블의 시간 복잡도(O(1) 평균)를 체감한다.

## 핵심 개념

### 해시 함수
```c
/* 단순한 해시 함수 */
unsigned int hash(const char *key, int table_size)
{
    unsigned int h = 0;
    while (*key)
        h = h * 31 + (unsigned char)(*key++);
    return h % table_size;
}
```

### 분리 연결법 (Separate Chaining)
```c
typedef struct Node {
    char *key;
    int   value;
    struct Node *next;
} Node;

typedef struct {
    Node **buckets;
    int    size;
} HashTable;
```

### 충돌 해결 전략
- **분리 연결법**: 같은 버킷에 연결 리스트로 노드 추가
- **개방 주소법**: 다른 빈 버킷을 선형/이차 탐색으로 찾음

## 과제 구성
| 파일 | 내용 |
|------|------|
| `homework.c` | 해시 테이블 구현 — 삽입, 검색, 삭제, 메모리 해제 |
| `homework2.c` | 단어 빈도수 세기 — 텍스트 파일에서 단어 등장 횟수를 해시 테이블로 카운트 |
| `game1.c` | 해시 테이블 기반 미니 게임 — 간단한 inventory 시스템 (아이템 추가/제거/조회) |

## 연습 문제
1. 재해시(rehashing)를 구현해 테이블이 일정 채우율 이상일 때 자동 확장되도록 한다.
2. 다양한 해시 함수(djb2, sdbm)를 비교한다.
3. 개방 주소법(선형 탐사) 버전으로 재구현한다.
