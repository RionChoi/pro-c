"""
Python-03 game1.py
텍스트 분석 도구 — Counter, defaultdict, itertools, dict 종합 활용
실제 로그 분석·텍스트 마이닝 패턴을 간단히 구현한다.
"""
import itertools
import re
from collections import Counter, defaultdict
from typing import Iterator


SAMPLE_TEXT = """
Python은 읽기 쉽고 배우기 쉬운 언어입니다.
Python은 데이터 과학, 웹 개발, 자동화 등 다양한 분야에서 사용됩니다.
데이터 과학자들은 Python과 함께 pandas, numpy, scikit-learn을 자주 사용합니다.
웹 개발에서는 Django와 FastAPI가 Python의 인기 프레임워크입니다.
Python은 인터프리터 언어이지만 성능이 중요할 때는 Rust나 C++을 함께 사용합니다.
Rust는 메모리 안전성과 성능을 동시에 제공하는 시스템 프로그래밍 언어입니다.
Go는 클라우드 네이티브 환경에서 많이 사용되는 언어입니다.
Python, Go, Rust 모두 현대 개발자에게 중요한 언어입니다.
데이터 과학과 웹 개발 모두 Python 생태계의 강점입니다.
"""

SAMPLE_LOGS = [
    "2026-05-11 09:01:32 INFO  서버 시작",
    "2026-05-11 09:01:45 DEBUG DB 연결 성공",
    "2026-05-11 09:02:11 INFO  요청 처리: GET /api/users",
    "2026-05-11 09:02:15 WARN  응답 지연: 450ms",
    "2026-05-11 09:02:30 INFO  요청 처리: POST /api/orders",
    "2026-05-11 09:03:01 ERROR DB 연결 실패: timeout",
    "2026-05-11 09:03:05 ERROR DB 연결 실패: timeout",
    "2026-05-11 09:03:10 WARN  재시도 중 (1/3)",
    "2026-05-11 09:03:15 INFO  DB 재연결 성공",
    "2026-05-11 09:04:00 INFO  요청 처리: GET /api/products",
    "2026-05-11 09:04:22 DEBUG 캐시 히트: /api/products",
    "2026-05-11 09:05:00 ERROR 인증 실패: 잘못된 토큰",
    "2026-05-11 09:05:30 INFO  요청 처리: DELETE /api/sessions",
]


# ── 섹션 1: 단어 빈도 분석 ───────────────────────────────────────────────
def word_frequency(text: str) -> None:
    print("=" * 55)
    print("📝 단어 빈도 분석")
    print("=" * 55)

    # 한글·영문만 추출, 소문자 정규화
    words = re.findall(r"[가-힣a-zA-Z]+", text)
    total = len(words)
    cnt = Counter(words)

    print(f"  총 단어 수: {total}개  |  고유 단어: {len(cnt)}개\n")
    print(f"  {'단어':<12} {'빈도':>4}  {'비율':>6}  막대")
    print(f"  {'-'*45}")
    for word, freq in cnt.most_common(10):
        ratio = freq / total * 100
        bar = "█" * freq
        print(f"  {word:<12} {freq:>4}회  {ratio:>5.1f}%  {bar}")


# ── 섹션 2: 동시 출현 단어 쌍 ───────────────────────────────────────────
def cooccurrence(text: str, window: int = 4) -> None:
    print("\n📊 동시 출현 단어 쌍 (윈도우 크기: {})".format(window))
    print("=" * 55)

    words = re.findall(r"[가-힣a-zA-Z]+", text)
    pair_cnt: Counter = Counter()

    # 슬라이딩 윈도우로 인접 단어 쌍 추출
    for i in range(len(words) - 1):
        window_words = words[i:i + window]
        # combinations로 윈도우 내 모든 쌍 생성
        for a, b in itertools.combinations(set(window_words), 2):
            pair = tuple(sorted([a, b]))   # 순서 정규화
            pair_cnt[pair] += 1

    print(f"  {'단어 쌍':<25} {'빈도':>4}")
    print(f"  {'-'*35}")
    for (w1, w2), freq in pair_cnt.most_common(8):
        print(f"  {w1} ↔ {w2:<20} {freq:>4}회")


# ── 섹션 3: 로그 분석 ────────────────────────────────────────────────────
def log_analysis(logs: list[str]) -> None:
    print("\n🔍 로그 분석")
    print("=" * 55)

    # defaultdict로 레벨별 로그 그룹화
    by_level: dict[str, list[str]] = defaultdict(list)
    for log in logs:
        parts = log.split(None, 3)   # 최대 4개로 분리
        if len(parts) >= 4:
            level = parts[2]
            message = parts[3]
            by_level[level].append(message)

    # Counter로 레벨 빈도 집계
    level_cnt = Counter({level: len(msgs) for level, msgs in by_level.items()})
    total = sum(level_cnt.values())

    print(f"  {'레벨':<8} {'건수':>4}  {'비율':>6}  내용")
    print(f"  {'-'*50}")
    for level, count in level_cnt.most_common():
        ratio = count / total * 100
        msgs = by_level[level]
        preview = msgs[0][:25] + "..." if len(msgs[0]) > 25 else msgs[0]
        print(f"  {level:<8} {count:>4}건  {ratio:>5.1f}%  예) {preview}")

    # groupby로 연속 같은 레벨 구간 탐지
    print("\n  연속 ERROR/WARN 구간:")
    level_seq = [log.split(None, 3)[2] for log in logs if len(log.split(None, 3)) >= 3]
    for level, group in itertools.groupby(enumerate(level_seq), key=lambda t: t[1]):
        indices = list(group)
        if level in ("ERROR", "WARN") and len(indices) >= 2:
            start, end = indices[0][0], indices[-1][0]
            print(f"    {level} {len(indices)}건 연속 (로그 {start+1}~{end+1}번째)")


# ── 섹션 4: n-gram 생성 ──────────────────────────────────────────────────
def ngram_analysis(text: str, n: int = 2) -> None:
    print(f"\n🔗 {n}-gram 분석")
    print("=" * 55)

    words = re.findall(r"[가-힣a-zA-Z]+", text)

    def ngrams(seq: list, n: int) -> Iterator[tuple]:
        # islice + zip으로 n-gram 생성
        iterators = [itertools.islice(iter(seq), i, None) for i in range(n)]
        return zip(*iterators)

    ngram_cnt = Counter(ngrams(words, n))
    print(f"  고유 {n}-gram: {len(ngram_cnt)}개\n")
    print(f"  {'n-gram':<30} {'빈도':>4}")
    print(f"  {'-'*38}")
    for gram, freq in ngram_cnt.most_common(8):
        print(f"  {' + '.join(gram):<30} {freq:>4}회")


# ── main ─────────────────────────────────────────────────────────────────
def main() -> None:
    print("🔬 텍스트 & 로그 분석 도구\n")
    word_frequency(SAMPLE_TEXT)
    cooccurrence(SAMPLE_TEXT, window=4)
    log_analysis(SAMPLE_LOGS)
    ngram_analysis(SAMPLE_TEXT, n=2)


if __name__ == "__main__":
    main()
