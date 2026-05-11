# Python-03: 자료구조 심화

## 전제 조건
Python-01~02 완료. C++의 `std::map`, `std::unordered_map`, `std::set` 개념을 알고 있다.
이 세션은 Python 표준 라이브러리의 **고급 자료구조**와 **이터레이터 도구**에 집중한다.

---

## C++ STL과 비교

| C++ | Python | 특징 |
|-----|--------|------|
| `std::unordered_map` | `dict` | 해시 테이블, O(1) 평균 |
| `std::map` | `dict` (3.7+ 삽입 순서 보장) | Python dict는 순서도 유지 |
| `std::unordered_set` | `set` | 중복 없는 해시 집합 |
| `std::multimap` | `collections.defaultdict(list)` | 키 하나에 여러 값 |
| `std::deque` | `collections.deque` | 양방향 O(1) push/pop |
| `std::priority_queue` | `heapq` 모듈 | 최소 힙 |
| 없음 | `collections.Counter` | 빈도 집계 특화 |
| 없음 | `collections.namedtuple` | 이름 있는 튜플 |

---

## 1. dict 고급 활용

```python
# setdefault: 키 없을 때만 기본값 삽입
d = {}
d.setdefault("scores", []).append(95)

# dict 병합 (Python 3.9+)
a = {"x": 1}
b = {"y": 2}
merged = a | b          # 새 dict 반환
a |= b                  # in-place 병합

# dict 뷰 (lazy — 원본과 동기화됨)
for key, val in d.items():   # keys(), values(), items()
    ...
```

## 2. collections 모듈

```python
from collections import defaultdict, Counter, deque, namedtuple, OrderedDict

# defaultdict: 없는 키 접근 시 자동 기본값
graph = defaultdict(list)
graph["A"].append("B")   # KeyError 없이 동작

# Counter: 빈도 집계
words = ["apple", "banana", "apple", "cherry", "banana", "apple"]
cnt = Counter(words)
cnt.most_common(2)       # [('apple', 3), ('banana', 2)]

# deque: 양끝 O(1) 삽입/삭제 (list append/pop(0)의 O(n) 해결)
q = deque(maxlen=3)      # maxlen: 고정 크기 원형 버퍼

# namedtuple: 불변 레코드 (dataclass 이전의 경량 버전)
Point = namedtuple("Point", ["x", "y"])
p = Point(3, 4)
p.x, p.y                 # 이름으로 접근
```

## 3. itertools 모듈

```python
import itertools

itertools.chain([1,2], [3,4])          # 이터러블 연결
itertools.islice(range(100), 5, 15)    # 슬라이싱 (lazy)
itertools.groupby(data, key=...)       # 연속 같은 키 그룹화
itertools.product("AB", repeat=2)      # 데카르트 곱
itertools.combinations([1,2,3], 2)     # 조합
itertools.permutations([1,2,3], 2)     # 순열
itertools.accumulate([1,2,3,4], ...)   # 누적 계산
itertools.cycle([1,2,3])               # 무한 순환
```

---

## 과제 목표

| 파일 | 내용 |
|------|------|
| `homework.py` | dict 고급 + `defaultdict`, `Counter`, `deque`, `namedtuple` |
| `homework2.py` | `itertools` 전체 — chain, groupby, product, combinations, accumulate |
| `game1.py` | 텍스트 분석 도구 — `Counter` + `itertools` + `defaultdict` 종합 |
