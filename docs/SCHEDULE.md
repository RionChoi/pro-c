# Hermes Agent 스케줄 설정 — pro-c 자동 학습

## 개요
매일 아침 7시(KST)에 C/C++ 학습 세션 2개를 자동으로 코딩하고 GitHub에 올립니다.

---

## Cron Job 설정

### 기본 정보
| 항목 | 값 |
|------|-----|
| **Name** | `pro-c-daily-learning` |
| **Schedule** | `0 22 * * *` (UTC) = KST 07:00 |
| **Delivery** | `local` |

### Prompt (전체)

```
pro-c C/C++ 자동 학습 세션을 실행합니다.

작업 디렉토리: /Users/choi/Movies/workspeace/github_ekko68/pro-c

1. docs/LEARNING_ROADMAP.md를 읽고 현재 진행 상태를 확인합니다.
2. 다음 미완료 2세션을 코딩합니다. 미완료 세션이 없으면 작업을 종료합니다.
   - C 세션 미완료 시: c-lang/ 폴더에 작성
   - C 완료 후: cpp-lang/ 폴더에 작성
3. 각 세션별로 다음 파일을 생성합니다:
   - README.md: 학습 내용 요약 (한국어)
   - homework.c/.cpp: 과제 1
   - homework2.c/.cpp: 과제 2
   - game1.c/.cpp: 미니 게임
4. 모든 코드를 컴파일하여 오류가 없는지 검증합니다.
   - C: cc -Wall -Wextra -Wpedantic <file.c> -o <output>
   - C++: g++ -std=c++17 -Wall -Wextra -Wpedantic <file.cpp> -o <output>
5. 코드 컨벤션을 준수합니다:
   - C: int main(void), scanf 반환값 검증, fgets 사용, fprintf(stderr, ...) 에러 출력
   - C++: int main(), std::cout/cin, RAII 패턴, const 참조 전달
6. docs/LEARNING_ROADMAP.md를 업데이트합니다 (완료된 세션 ✅ 표시).
7. git add, commit (영어 커밋 메시지), push를 실행합니다.
8. 40세션 전부 완료 시:
   - README.md에 프로젝트 완료 표시
   - 최종 커밋 후 cron job 자체를 제거합니다.
```

---

## 등록 방법

### 방법 1: Dashboard (웹 UI)
1. Hermes Dashboard → **Cron** 탭
2. **New Job** 클릭
3. 위 정보 입력 (Name, Schedule, Prompt, Delivery)
4. 저장

### 방법 2: Chat 명령어
```
/cron add "0 22 * * *" "pro-c C/C++ 자동 학습 세션을 실행합니다. 작업 디렉토리: /Users/choi/Movies/workspeace/github_ekko68/pro-c. 1) docs/LEARNING_ROADMAP.md를 읽고 현재 진행 상태를 확인. 2) 다음 미완료 2세션을 코딩. 3) 각 세션별 README.md, homework, homework2, game1 파일 생성. 4) 컴파일 검증. 5) 코드 컨벤션 준수. 6) LEARNING_ROADMAP.md 업데이트. 7) git add, commit, push. 8) 28세션 완료 시 프로젝트 종료."
/cron add "0 22 * * *" "pro-c C/C++ 자동 학습 세션을 실행합니다. 작업 디렉토리: /Users/choi/Movies/workspeace/github_ekko68/pro-c. 1) docs/LEARNING_ROADMAP.md를 읽고 현재 진행 상태를 확인. 2) 다음 미완료 2세션을 코딩. 미완료 세션이 없으면 작업을 종료. 3) 각 세션별 README.md, homework, homework2, game1 파일 생성. 4) 컴파일 검증. 5) 코드 컨벤션 준수. 6) LEARNING_ROADMAP.md 업데이트. 7) git add, commit, push. 8) 40세션 완료 시 프로젝트 종료."
```

### 방법 3: CLI
```bash
hermes cron create "0 22 * * *" \
  "pro-c C/C++ 자동 학습 세션을 실행합니다. ..." \
  --name "pro-c-daily-learning"
```

---

## 관리 명령어

| 작업 | 명령어 |
|------|--------|
| 목록 확인 | `/cron list` |
| 일시 정지 | `/cron pause <job_id>` |
| 재개 | `/cron resume <job_id>` |
| 즉시 실행 (테스트) | `/cron run <job_id>` |
| 삭제 | `/cron remove <job_id>` |
| 수정 | `/cron edit <job_id>` |

---

## 주의사항

1. **Hermes Agent가 상시 실행** 중이어야 스케줄이 동작합니다 (PM2 또는 Docker 권장)
2. **config.yaml**에서 `scheduler: enabled: true` 확인
3. **시간대**: UTC 기준이므로 KST 07:00 = UTC 22:00 (전날)
4. **Git 인증**: SSH 키 또는 토큰 기반 인증이 사전 설정되어 있어야 합니다
