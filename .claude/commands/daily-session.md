---
description: 다음 C++ 학습 세션을 자동으로 진행하고 GitHub에 커밋/푸시/머지한다. 매일 한 세션씩 진행.
allowed-tools: Bash, Read, Write, Edit, WebSearch, mcp__github__create_pull_request, mcp__github__merge_pull_request, mcp__github__update_pull_request, mcp__github__list_pull_requests
---

## 현재 학습 로드맵

!`cat /home/user/pro-c/docs/LEARNING_ROADMAP.md`

## 현재 cpp-lang 디렉토리

!`ls /home/user/pro-c/cpp-lang/ | sort`

## 오늘 날짜

!`date '+%Y-%m-%d'`

---

## 작업 지시

### STEP 1: 다음 세션 확인

위 로드맵에서 상태가 `⬜` 인 **가장 첫 번째 세션**을 찾는다.

**모든 세션이 ✅ 완료 상태라면**:
1. WebSearch로 `"advanced C++ topics 2026 modern standards coroutines concurrency metaprogramming"` 검색
2. 이미 완료된 세션 주제와 중복되지 않는 새 주제 10개 선정
3. `docs/LEARNING_ROADMAP.md`에 새 Phase를 추가 (세션 번호는 이전 마지막 번호 + 1부터)
4. 오늘은 새 Phase의 첫 번째 세션을 진행한다

### STEP 2: 세션 파일 생성

`/home/user/pro-c/cpp-lang/XX-topic-name/` 디렉토리를 생성하고 4개 파일을 작성한다.

디렉토리 이름 형식: `NN-kebab-case-topic` (예: `25-memory-optimization`)

#### README.md
한국어로 작성:
- 학습 목표 (3~5개)
- 핵심 개념 표 (개념 | 설명)
- 코드 예시 (핵심 문법만)
- 컴파일 명령어

#### homework.cpp
- 세션 주제의 핵심 개념을 완전히 커버하는 예제 코드
- 여러 기능을 단계별로 시연 (`[1]`, `[2]`... 섹션)
- `main()` 실행 시 결과가 출력되어야 함

#### homework2.cpp
- 핵심 과제의 심화 또는 응용 버전
- 다른 각도에서 같은 개념 접근

#### game1.cpp
- 세션 개념을 응용한 인터랙티브 미니 게임 또는 시뮬레이션
- 사용자 입력 또는 흥미로운 출력 포함

**컴파일 표준 기준**:

| 세션 번호 | 표준 |
|-----------|------|
| 01 ~ 20   | `-std=c++17` |
| 21 ~ 22   | `-std=c++20` |
| 23 ~      | `-std=c++23` |

모든 파일에 `-Wall -Wextra -Wpedantic` 적용.

### STEP 3: 컴파일 검증

세 파일 모두 컴파일한다:
```bash
g++ -std=c++XX -Wall -Wextra -Wpedantic homework.cpp  -o /tmp/out
g++ -std=c++XX -Wall -Wextra -Wpedantic homework2.cpp -o /tmp/out
g++ -std=c++XX -Wall -Wextra -Wpedantic game1.cpp     -o /tmp/out
```

경고나 오류가 있으면 즉시 수정하고 재컴파일한다. 모든 파일이 경고 없이 통과할 때까지 반복한다.

또한 `homework.cpp`와 `homework2.cpp`를 실행해서 출력이 올바른지 확인한다.

### STEP 4: 로드맵 업데이트

`/home/user/pro-c/docs/LEARNING_ROADMAP.md`에서:
1. 해당 세션 상태를 `⬜` → `✅` 로 변경
2. 진행 기록 테이블 맨 아래에 한 줄 추가:
   `| YYYY-MM-DD | C++-XX | 세션 주제 (영문) |`

### STEP 5: Git 커밋 / 푸시 / PR / 머지

```bash
# 1. main에서 새 브랜치 생성
git fetch origin main
git checkout -b session/cpp-XX-topic-name origin/main

# 2. 파일 추가 및 커밋
git add cpp-lang/XX-topic-name/ docs/LEARNING_ROADMAP.md
git commit -m "add session XX: TopicName (brief description)"

# 3. 푸시
git push -u origin session/cpp-XX-topic-name
```

4. `mcp__github__create_pull_request` 로 **draft PR** 생성 (rionchoi/pro-c)
5. `mcp__github__update_pull_request` 로 draft → ready 전환
6. `mcp__github__merge_pull_request` 로 **squash merge**

### STEP 6: 완료 보고

다음 형식으로 사용자에게 보고한다:

```
✅ 세션 XX 완료: [주제명]

파일:
  - cpp-lang/XX-topic/README.md
  - cpp-lang/XX-topic/homework.cpp
  - cpp-lang/XX-topic/homework2.cpp
  - cpp-lang/XX-topic/game1.cpp

PR: merged → main

다음 세션: [다음 번호] — [다음 주제]
```
