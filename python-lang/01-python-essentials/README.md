# Python-01: Python 핵심 특성

## 전제 조건
C/C++ 40세션 완료. 변수, 조건문, 반복문, 함수, 포인터, OOP 기본 개념은 이미 알고 있다.
이 세션은 C/C++과의 **차이점**과 Python **고유 특성**에만 집중한다.

---

## C/C++과의 핵심 차이

| 항목 | C/C++ | Python |
|------|-------|--------|
| 타입 | 정적 (컴파일 타임 결정) | 동적 (런타임 결정) |
| 메모리 | 수동 관리 (malloc/free, RAII) | 가비지 컬렉터 자동 관리 |
| 컴파일 | 필수 (clang, g++) | 불필요 (인터프리터) |
| 문자열 | `char[]`, `std::string` | `str` (불변 유니코드) |
| 배열 | 고정 크기 | `list` (동적, 혼합 타입 가능) |
| 포인터 | 명시적 (`*`, `&`) | 없음 (모든 객체는 참조) |

---

## 1. 동적 타입과 Type Hints

Python은 변수에 타입을 선언하지 않는다. 단, **type hint**로 의도를 명시할 수 있다.

```python
# 타입 선언 없이 변수 사용
x = 42
x = "이제 문자열" # 같은 변수에 다른 타입 가능

# Type hint (실행에는 영향 없음, 가독성·IDE 지원용)
name: str = "리온"
score: int = 95
ratio: float = 0.95
```

## 2. f-string (포맷 문자열)

```python
name = "리온"
score = 95.5

# C: printf("이름: %s, 점수: %.1f\n", name, score);
print(f"이름: {name}, 점수: {score:.1f}")  # f-string
print(f"2 + 3 = {2 + 3}")                 # 표현식 직접 삽입 가능
print(f"{'정렬':>10}")                    # 포맷 스펙도 동일하게
```

## 3. List Comprehension

```python
# C: for 루프로 배열 생성
squares = [x ** 2 for x in range(10)]           # [0, 1, 4, 9, 16, ...]
evens   = [x for x in range(20) if x % 2 == 0] # 필터 조건 포함
matrix  = [[r * c for c in range(4)] for r in range(4)]  # 중첩

# dict comprehension
scores = {"철수": 85, "영희": 92, "민준": 78}
grades = {name: "A" if s >= 90 else "B" for name, s in scores.items()}
```

## 4. 언패킹 & 다중 할당

```python
a, b = 1, 2          # 동시 할당
a, b = b, a          # C에서는 임시 변수 필요 — Python은 한 줄
first, *rest = [1, 2, 3, 4, 5]  # 스타 언패킹: first=1, rest=[2,3,4,5]

x, y, z = (10, 20, 30)  # 튜플 언패킹
```

## 5. 내장 함수: enumerate, zip, map, filter

```python
items = ["사과", "바나나", "체리"]

# enumerate: 인덱스 + 값 동시에
for i, item in enumerate(items, start=1):
    print(f"{i}. {item}")

# zip: 두 시퀀스를 병렬로
names  = ["철수", "영희"]
scores = [85, 92]
for name, score in zip(names, scores):
    print(f"{name}: {score}")

# map / filter (함수형 스타일)
doubled  = list(map(lambda x: x * 2, [1, 2, 3]))
filtered = list(filter(lambda x: x > 2, [1, 2, 3, 4]))
```

## 6. None vs NULL

```python
result = None          # C의 NULL, C++의 nullptr과 유사
if result is None:     # == 대신 is 사용 (identity 비교)
    print("값 없음")
```

---

## 과제 목표

| 파일 | 내용 |
|------|------|
| `homework.py` | 동적 타입, f-string, comprehension, type hint 실습 |
| `homework2.py` | enumerate/zip/map/filter + 언패킹 고급 활용 |
| `game1.py` | comprehension + 내장함수로 만드는 성적 분석 도구 |
