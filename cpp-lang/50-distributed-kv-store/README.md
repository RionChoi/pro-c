# 세션 50: Phase 5 최종 프로젝트 — 고성능 분산 키-값 스토어

## 학습 목표
1. Phase 5의 핵심 주제인 코루틴, Lock-free 패턴, Arena Allocator, 네트워크 프레이밍을 하나의 프로젝트로 통합한다
2. consistent hashing 기반 샤딩과 replication factor 개념을 구현한다
3. PUT/GET/DELETE 요청을 이진 프레임으로 직렬화하고 파싱한다
4. 완료 큐와 코루틴으로 비동기 요청 처리 흐름을 설계한다
5. 장애 노드, 복제본, 읽기 quorum을 단순 시뮬레이션으로 검증한다

## 핵심 개념

| 개념 | 설명 |
|------|------|
| shard | 키 해시를 기준으로 데이터가 배치되는 논리 파티션 |
| replication factor | 같은 값을 여러 노드에 저장하는 복제 수 |
| quorum read | 복제본 일부에서 값을 읽어 응답하는 방식 |
| binary frame | 네트워크 전송을 위한 길이/타입/payload 구조 |
| arena allocation | 요청 처리 중 짧게 사는 데이터를 일괄 할당/해제하는 방식 |
| completion queue | 비동기 작업 완료 이벤트를 소비하는 큐 |

## 과제

### homework.cpp
**분산 KV 라우팅과 복제**
- 해시 기반 노드 선택
- replication factor 기반 PUT/GET/DELETE
- 장애 노드 제외 읽기
- 간단한 클러스터 상태 출력

### homework2.cpp
**고성능 요청 처리 엔진**
- LinearArena 기반 임시 문자열 저장
- MPSC 스타일 completion queue
- 코루틴 `Task<T>` 기반 요청 처리
- 이진 프레임 직렬화/파싱

### game1.cpp
**통합 미니 시뮬레이션**
- 3노드 클러스터
- PUT/GET/DELETE 명령 스크립트
- 노드 장애/복구 이벤트
- 최종 데이터 배치와 진행률 출력

## 컴파일 명령어

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic homework.cpp  -o hw
g++ -std=c++20 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++20 -Wall -Wextra -Wpedantic game1.cpp     -o game
```
