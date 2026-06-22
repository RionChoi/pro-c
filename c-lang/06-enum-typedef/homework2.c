/*
 * 세션 06 — 과제 2: 상태 머신 시뮬레이터
 *
 * 기능:
 *   - 세탁기 동작을 상태 머신으로 모델링
 *   - 상태: IDLE → FILLING → WASHING → RINSING → SPINNING → DONE
 *   - 각 상태에서 가능한 전환을 enum으로 관리
 *   - typedef로 상태/이벤트 별칭 사용
 */

#include <stdio.h>

typedef enum {
    STATE_IDLE,
    STATE_FILLING,
    STATE_WASHING,
    STATE_RINSING,
    STATE_SPINNING,
    STATE_DONE,
    STATE_ERROR,
    STATE_COUNT
} State;

typedef enum {
    EVENT_START,
    EVENT_WATER_FULL,
    EVENT_WASH_DONE,
    EVENT_RINSE_DONE,
    EVENT_SPIN_DONE,
    EVENT_RESET,
    EVENT_FAULT,
    EVENT_COUNT
} Event;

/* 상태 이름을 반환한다 */
const char *state_name(State s) {
    static const char *names[] = {
        "대기(IDLE)", "급수(FILLING)", "세탁(WASHING)",
        "헹굼(RINSING)", "탈수(SPINNING)", "완료(DONE)", "오류(ERROR)"
    };
    if (s >= 0 && s < STATE_COUNT) {
        return names[s];
    }
    return "알 수 없음";
}

/* 이벤트 이름을 반환한다 */
const char *event_name(Event e) {
    static const char *names[] = {
        "시작", "급수 완료", "세탁 완료",
        "헹굼 완료", "탈수 완료", "리셋", "고장"
    };
    if (e >= 0 && e < EVENT_COUNT) {
        return names[e];
    }
    return "알 수 없음";
}

/* 상태 전환 테이블 */
typedef struct {
    State current;
    Event event;
    State next;
} Transition;

static const Transition transitions[] = {
    { STATE_IDLE,     EVENT_START,      STATE_FILLING  },
    { STATE_FILLING,  EVENT_WATER_FULL, STATE_WASHING  },
    { STATE_WASHING,  EVENT_WASH_DONE,  STATE_RINSING  },
    { STATE_RINSING,  EVENT_RINSE_DONE, STATE_SPINNING },
    { STATE_SPINNING, EVENT_SPIN_DONE,  STATE_DONE     },
    { STATE_DONE,     EVENT_RESET,      STATE_IDLE     },
    /* 어떤 상태에서든 고장 발생 가능 */
    { STATE_FILLING,  EVENT_FAULT,      STATE_ERROR    },
    { STATE_WASHING,  EVENT_FAULT,      STATE_ERROR    },
    { STATE_RINSING,  EVENT_FAULT,      STATE_ERROR    },
    { STATE_SPINNING, EVENT_FAULT,      STATE_ERROR    },
    { STATE_ERROR,    EVENT_RESET,      STATE_IDLE     },
};

#define TRANSITION_COUNT ((int)(sizeof(transitions) / sizeof(transitions[0])))

/* 현재 상태와 이벤트에 따라 다음 상태를 결정한다 */
State process_event(State current, Event event) {
    for (int i = 0; i < TRANSITION_COUNT; i++) {
        if (transitions[i].current == current &&
            transitions[i].event   == event) {
            return transitions[i].next;
        }
    }
    return current; /* 유효하지 않은 전환: 상태 유지 */
}

/* 현재 상태에서 가능한 이벤트를 출력한다 */
void show_available_events(State current) {
    printf("  가능한 이벤트:\n");
    int count = 0;
    for (int i = 0; i < TRANSITION_COUNT; i++) {
        if (transitions[i].current == current) {
            printf("    %d. %s\n", transitions[i].event, event_name(transitions[i].event));
            count++;
        }
    }
    if (count == 0) {
        printf("    (가능한 이벤트 없음)\n");
    }
}

int main(void) {
    State current = STATE_IDLE;
    int event_num;

    printf("=== 세탁기 상태 머신 시뮬레이터 ===\n");
    printf("상태 전환을 직접 제어해 보세요!\n");

    while (1) {
        printf("\n현재 상태: [%s]\n", state_name(current));
        show_available_events(current);
        printf("\n  이벤트 번호 입력 (-1: 종료): ");

        if (scanf("%d", &event_num) != 1) {
            fprintf(stderr, "  숫자를 입력하세요.\n");
            while (getchar() != '\n');
            continue;
        }

        if (event_num == -1) {
            printf("\n시뮬레이터를 종료합니다.\n");
            break;
        }

        if (event_num < 0 || event_num >= EVENT_COUNT) {
            fprintf(stderr, "  유효하지 않은 이벤트 번호입니다.\n");
            continue;
        }

        Event e = (Event)event_num;
        State next = process_event(current, e);

        if (next == current) {
            printf("  ⚠ '%s' 상태에서 '%s' 이벤트는 무효합니다.\n",
                   state_name(current), event_name(e));
        } else {
            printf("  → 전환: [%s] --(%s)--> [%s]\n",
                   state_name(current), event_name(e), state_name(next));
            current = next;

            if (current == STATE_DONE) {
                printf("\n  🎉 세탁 완료! 리셋(5)으로 다시 시작할 수 있습니다.\n");
            } else if (current == STATE_ERROR) {
                printf("\n  ❌ 고장 발생! 리셋(5)으로 복구하세요.\n");
            }
        }
    }

    return 0;
}
