/*
 * 세션 06 — 게임: RPG 캐릭터 선택 & 전투 시스템
 *
 * 규칙:
 *   - enum으로 캐릭터 클래스(전사/마법사/궁수/힐러) 정의
 *   - typedef로 구조체 별칭 사용
 *   - 각 클래스마다 스탯(HP, ATK, DEF, SPD)이 다름
 *   - 몬스터와 턴제 전투 (공격/방어/스킬/포션 선택)
 *   - 3라운드까지 진행하며 최종 점수 계산
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum {
    CLASS_WARRIOR,
    CLASS_MAGE,
    CLASS_ARCHER,
    CLASS_HEALER,
    CLASS_COUNT
} CharClass;

typedef struct {
    char        name[32];
    CharClass   cls;
    int         hp;
    int         max_hp;
    int         atk;
    int         def;
    int         spd;
    int         potions;
} Character;

typedef struct {
    char name[32];
    int  hp;
    int  atk;
    int  def;
} Monster;

/* 클래스 이름을 반환한다 */
const char *class_name(CharClass c) {
    static const char *names[] = {"전사", "마법사", "궁수", "힐러"};
    if (c >= 0 && c < CLASS_COUNT) return names[c];
    return "알 수 없음";
}

/* 캐릭터를 초기화한다 */
Character create_character(const char *name, CharClass cls) {
    Character ch;
    snprintf(ch.name, sizeof(ch.name), "%s", name);
    ch.cls = cls;
    ch.potions = 2;

    switch (cls) {
        case CLASS_WARRIOR:
            ch.max_hp = 120; ch.atk = 18; ch.def = 15; ch.spd = 8;  break;
        case CLASS_MAGE:
            ch.max_hp = 80;  ch.atk = 25; ch.def = 8;  ch.spd = 12; break;
        case CLASS_ARCHER:
            ch.max_hp = 90;  ch.atk = 20; ch.def = 10; ch.spd = 18; break;
        case CLASS_HEALER:
            ch.max_hp = 100; ch.atk = 12; ch.def = 12; ch.spd = 14; break;
        default:
            ch.max_hp = 100; ch.atk = 15; ch.def = 10; ch.spd = 10; break;
    }
    ch.hp = ch.max_hp;
    return ch;
}

/* 몬스터를 생성한다 */
Monster create_monster(int round) {
    Monster m;
    static const char *monster_names[] = {
        "슬라임", "고블린", "오크 전사", "다크 나이트", "드래곤"
    };
    int idx = round;
    if (idx > 4) idx = 4;

    snprintf(m.name, sizeof(m.name), "%s", monster_names[idx]);
    m.hp  = 50 + round * 30;
    m.atk = 10 + round * 5;
    m.def = 5  + round * 3;
    return m;
}

/* 데미지를 계산한다 (최소 1) */
int calc_damage(int atk, int def) {
    int dmg = atk - def / 2 + (rand() % 6);
    return dmg < 1 ? 1 : dmg;
}

/* 캐릭터 상태를 출력한다 */
void print_status(const Character *ch, const Monster *m) {
    printf("\n  %-12s HP: %3d/%3d  | 포션: %d개\n",
           ch->name, ch->hp, ch->max_hp, ch->potions);
    printf("  %-12s HP: %3d\n", m->name, m->hp);
    printf("  ─────────────────────────\n");
}

/* 전투를 수행한다 */
int battle(Character *ch, Monster *m) {
    int choice;

    printf("\n  ⚔ %s VS %s ⚔\n", ch->name, m->name);

    while (ch->hp > 0 && m->hp > 0) {
        print_status(ch, m);
        printf("  행동 선택:\n");
        printf("    1. 공격   2. 방어   3. 스킬   4. 포션\n");
        printf("  → ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }

        int dmg;
        switch (choice) {
            case 1: /* 일반 공격 */
                dmg = calc_damage(ch->atk, m->def);
                m->hp -= dmg;
                printf("  ⚔ %s의 공격! %s에게 %d 데미지!\n",
                       ch->name, m->name, dmg);
                break;

            case 2: /* 방어 (다음 턴 데미지 50% 감소) */
                printf("  🛡 %s이(가) 방어 태세!\n", ch->name);
                /* 몬스터 공격 데미지 감소 */
                if (m->hp > 0) {
                    dmg = calc_damage(m->atk, ch->def * 2);
                    ch->hp -= dmg;
                    printf("  👹 %s의 반격! %d 데미지 (방어로 감소)\n",
                           m->name, dmg);
                }
                continue; /* 몬스터 턴 스킵 (이미 반격함) */

            case 3: /* 스킬 (클래스별 특수 공격) */
                switch (ch->cls) {
                    case CLASS_WARRIOR:
                        dmg = calc_damage(ch->atk * 2, m->def);
                        m->hp -= dmg;
                        printf("  🗡 강타! %d 데미지!\n", dmg);
                        break;
                    case CLASS_MAGE:
                        dmg = calc_damage(ch->atk * 2 + 5, m->def / 2);
                        m->hp -= dmg;
                        printf("  🔥 파이어볼! %d 데미지!\n", dmg);
                        break;
                    case CLASS_ARCHER:
                        dmg = calc_damage(ch->atk + 10, m->def / 3);
                        m->hp -= dmg;
                        printf("  🏹 관통 사격! %d 데미지!\n", dmg);
                        break;
                    case CLASS_HEALER:
                        ch->hp += 25;
                        if (ch->hp > ch->max_hp) ch->hp = ch->max_hp;
                        dmg = calc_damage(ch->atk, m->def);
                        m->hp -= dmg;
                        printf("  ✨ 신성 일격! %d 데미지 + 25 회복!\n", dmg);
                        break;
                    default:
                        break;
                }
                break;

            case 4: /* 포션 */
                if (ch->potions <= 0) {
                    printf("  포션이 없습니다!\n");
                    continue;
                }
                ch->potions--;
                ch->hp += 30;
                if (ch->hp > ch->max_hp) ch->hp = ch->max_hp;
                printf("  🧪 포션 사용! HP 30 회복! (남은 포션: %d)\n",
                       ch->potions);
                break;

            default:
                printf("  1~4 사이의 번호를 선택하세요.\n");
                continue;
        }

        /* 몬스터 턴 */
        if (m->hp > 0) {
            dmg = calc_damage(m->atk, ch->def);
            ch->hp -= dmg;
            printf("  👹 %s의 공격! %d 데미지!\n", m->name, dmg);
        }
    }

    if (ch->hp <= 0) {
        printf("\n  💀 %s 패배...\n", ch->name);
        return 0;
    }
    printf("\n  🎉 %s 처치! 승리!\n", m->name);
    return 1;
}

int main(void) {
    srand((unsigned int)time(NULL));

    char name[32];
    int cls_choice;
    int score = 0;

    printf("=== RPG 전투 게임 ===\n\n");

    /* 캐릭터 이름 입력 */
    printf("캐릭터 이름: ");
    if (fgets(name, sizeof(name), stdin) == NULL) {
        fprintf(stderr, "입력 오류\n");
        return 1;
    }
    name[strcspn(name, "\n")] = '\0';

    /* 클래스 선택 */
    printf("\n클래스 선택:\n");
    for (int i = 0; i < CLASS_COUNT; i++) {
        printf("  %d. %s", i + 1, class_name((CharClass)i));
        switch ((CharClass)i) {
            case CLASS_WARRIOR: printf(" (HP↑ ATK↑ DEF↑ SPD↓)\n"); break;
            case CLASS_MAGE:    printf(" (HP↓ ATK↑↑ DEF↓ SPD→)\n"); break;
            case CLASS_ARCHER:  printf(" (HP→ ATK↑ DEF→ SPD↑↑)\n"); break;
            case CLASS_HEALER:  printf(" (HP→ ATK↓ DEF→ SPD↑)\n"); break;
            default: printf("\n"); break;
        }
    }
    printf("선택: ");
    if (scanf("%d", &cls_choice) != 1 || cls_choice < 1 || cls_choice > CLASS_COUNT) {
        fprintf(stderr, "유효하지 않은 선택입니다.\n");
        return 1;
    }

    Character hero = create_character(name, (CharClass)(cls_choice - 1));
    printf("\n🎮 %s [%s] 생성 완료!\n", hero.name, class_name(hero.cls));
    printf("   HP:%d ATK:%d DEF:%d SPD:%d\n",
           hero.max_hp, hero.atk, hero.def, hero.spd);

    /* 3라운드 전투 */
    for (int round = 0; round < 3; round++) {
        printf("\n══════ 라운드 %d/3 ══════\n", round + 1);
        Monster enemy = create_monster(round);

        if (battle(&hero, &enemy)) {
            score += (round + 1) * 100;
            /* 체력 일부 회복 */
            hero.hp += 20;
            if (hero.hp > hero.max_hp) hero.hp = hero.max_hp;
            hero.potions++; /* 포션 보상 */
        } else {
            printf("\n게임 오버! 라운드 %d에서 탈락.\n", round + 1);
            break;
        }
    }

    printf("\n═══════════════════════\n");
    printf("  최종 점수: %d\n", score);
    printf("═══════════════════════\n");
    printf("게임을 종료합니다!\n");

    return 0;
}
