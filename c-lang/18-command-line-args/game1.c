/*
 * game1.c — 터미널 RPG (커맨드라인 인자)
 *
 * 사용법: rpg [-h] [-Hp HP] [-At ATK] [-Df DEF] [-w WEAPON]
 *   -h: 도움말
 *   -Hp: 초기 HP (기본 100)
 *   -At: 공격력  (기본 20)
 *   -Df: 방어력  (기본 10)
 *   -w: 무기 이름 (기본 "주먹")
 *
 * 예시:
 *   ./rpg -Hp 150 -At 30 -Df 15 -w "火焰의 검"
 *   ./rpg --help
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DEFAULT_HP     100
#define DEFAULT_ATK    20
#define DEFAULT_DEF    10

typedef struct {
    int hp;
    int atk;
    int def;
    char weapon[64];
} Character;

typedef struct {
    const char *name;
    int hp;
    int atk;
    int def;
} Monster;

static Monster monsters[] = {
    { "슬라임",       30,  5,  2 },
    { "고블린",       60, 12,  5 },
    { "오크",        100, 20, 10 },
    { "드래곤",      200, 40, 20 }
};
static int monster_count = 4;

static void print_help(const char *prog)
{
    printf("사용법: %s [-h] [-Hp HP] [-At ATK] [-Df DEF] [-w WEAPON]\n", prog);
    printf("  -h     도움말 출력\n");
    printf("  -Hp    초기 HP (기본 %d)\n", DEFAULT_HP);
    printf("  -At    공격력  (기본 %d)\n", DEFAULT_ATK);
    printf("  -Df    방어력  (기본 %d)\n", DEFAULT_DEF);
    printf("  -w     무기 이름 (기본 \"주먹\")\n");
}

static void print_status(const Character *p, int stage)
{
    printf("\n[내 상태] HP: %d  ATK: %d  DEF: %d  무기: %s\n",
           p->hp, p->atk, p->def, p->weapon);
    printf("[보스 %d] %s — HP: %d  ATK: %d  DEF: %d\n",
           stage + 1, monsters[stage].name,
           monsters[stage].hp, monsters[stage].atk, monsters[stage].def);
}

static int compute_damage(int atk, int def)
{
    int dmg = atk - def / 2;
    return dmg < 1 ? 1 : dmg;
}

static void battle(Character *player, int stage)
{
    Monster m = monsters[stage];
    int turn = 1;

    printf("\n══════════════════════════════");
    printf("\n  【전투 시작! — %s】", m.name);
    printf("\n══════════════════════════════\n");

    while (player->hp > 0 && m.hp > 0) {
        printf("\n[Turn %d]", turn++);

        /* 플레이어 공격 */
        int player_dmg = compute_damage(player->atk, m.def);
        m.hp -= player_dmg;
        if (m.hp < 0) m.hp = 0;
        printf(" %s의 공격! %s에게 %d 데미지! (남은 HP: %d)\n",
               player->weapon, m.name, player_dmg, m.hp);

        if (m.hp <= 0) {
            printf("\n★★★ %s을(를) 물리쳤다! ★★★\n", m.name);
            return;
        }

        /* 몬스터 공격 */
        int monster_dmg = compute_damage(m.atk, player->def);
        player->hp -= monster_dmg;
        if (player->hp < 0) player->hp = 0;
        printf(" %s의 역공! %d 데미지! (남은 HP: %d)\n",
               m.name, monster_dmg, player->hp);
    }

    if (player->hp <= 0) {
        printf("\n■■■ 패배... ■■■\n");
    }
}

int main(int argc, char *argv[])
{
    Character player = { DEFAULT_HP, DEFAULT_ATK, DEFAULT_DEF, "주먹" };

    /* 옵션 파싱 */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        }
        if (i + 1 >= argc) {
            fprintf(stderr, "오류: %s 뒤에 값이 필요합니다.\n", argv[i]);
            return 1;
        }

        if (strcmp(argv[i], "-Hp") == 0)
            player.hp = atoi(argv[++i]);
        else if (strcmp(argv[i], "-At") == 0)
            player.atk = atoi(argv[++i]);
        else if (strcmp(argv[i], "-Df") == 0)
            player.def = atoi(argv[++i]);
        else if (strcmp(argv[i], "-w") == 0)
            strncpy(player.weapon, argv[++i], 63);
        else {
            fprintf(stderr, "알 수 없는 옵션: %s\n", argv[i]);
            print_help(argv[0]);
            return 1;
        }
    }

    /* 유효성 검사 */
    if (player.hp <= 0)  { fprintf(stderr, "HP는 양수여야 합니다.\n"); return 1; }
    if (player.atk < 0)  { fprintf(stderr, "ATK는 음수일 수 없습니다.\n"); return 1; }
    if (player.def < 0)  { fprintf(stderr, "DEF는 음수일 수 없습니다.\n"); return 1; }

    printf("=== 터미널 RPG ===\n");
    printf("캐릭터: HP=%d ATK=%d DEF=%d 무기=%s\n",
           player.hp, player.atk, player.def, player.weapon);

    for (int i = 0; i < monster_count; i++) {
        print_status(&player, i);
        battle(&player, i);

        if (player.hp <= 0) {
            printf("\n게임 오버. 당신은 %d라운드까지 생존했습니다.\n", i);
            return 0;
        }

        /* 힐링 between stages */
        int heal = 20;
        player.hp += heal;
        printf("\n[ Stage %d 클리어! HP +%d 회복 ]\n", i + 1, heal);
    }

    printf("\n★★★ 모든 보스를 물리쳤다! 게임 클리어! ★★★\n");
    return 0;
}
