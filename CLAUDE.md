# CLAUDE.md — pro-c Repository Guide

This file describes the structure, conventions, and development workflow for the **pro-c** repository. It is intended for AI assistants (such as Claude) to understand the codebase and contribute effectively.

---

## Project Overview

**pro-c** is a structured C language learning project. It progresses through four core topics in a curriculum-style format, with each lesson containing exercises and a practical mini-project. All code is written in C (C11 standard) with no external dependencies.

The learner is **RionChoi**. Documentation is primarily written in Korean.

---

## Repository Structure

```
pro-c/
├── CLAUDE.md                       # This file
├── README.md                       # Korean-language project summary
├── Helloworl.c                     # Initial Hello World program (has a known typo in filename)
├── Helloworl.dSYM/                 # macOS debug symbols (not tracked by git)
├── docs/
│   ├── LEARNING_ROADMAP.md         # Weekly learning plan and progress tracking
│   └── CODE_REVIEW.md              # Code review standards and action items
└── course/
    ├── 01-variables-io/            # Lesson 1: Variables, printf, scanf
    │   ├── README.md
    │   ├── homework.c              # Core assignment (two-integer arithmetic)
    │   ├── homework2.c             # Extended assignment (temperature conversion)
    │   └── game1.c                 # Mini-project (number guessing game)
    ├── 02-condition-loop/          # Lesson 2: if/else, for, while
    │   ├── README.md
    │   ├── homework.c              # Menu-driven program (times table, sum, prime)
    │   ├── homework2.c             # Star pyramid display
    │   └── game1.c                 # Rock-Paper-Scissors vs computer
    ├── 03-function-array/          # Lesson 3: Functions, arrays, statistics
    │   ├── README.md
    │   ├── homework.c              # Student grade analysis (avg/max/grade)
    │   ├── homework2.c             # Pass/fail counting from 5 scores
    │   └── game1.c                 # Dice simulation game
    └── 04-pointers-strings/        # Lesson 4: Pointers, strings, fgets
        ├── README.md
        ├── homework.c              # Pointer-based score array processing
        ├── homework2.c             # String reversal via pointer iteration
        └── game1.c                 # String matching game (3 attempts)
```

---

## Building and Running

There is no Makefile or build system. All compilation is done manually.

### Standard compilation (minimum)
```bash
cc -Wall -Wextra -Wpedantic <file.c> -o <output>
./<output>
```

### Strict compilation (recommended for review)
```bash
clang -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wformat=2 \
  -Wnull-dereference -Wdouble-promotion -Wstrict-prototypes <file.c> -o <output>
```

### Examples
```bash
# Compile and run the Hello World file
cc -Wall -Wextra -Wpedantic Helloworl.c -o hello
./hello

# Compile and run a lesson homework
cc -Wall -Wextra -Wpedantic course/01-variables-io/homework.c -o hw1
./hw1

# Compile lesson 4 homework (uses string.h)
cc -Wall -Wextra -Wpedantic course/04-pointers-strings/homework.c -o hw4
./hw4
```

There are no tests to run, no linting tools to invoke, and no CI/CD pipeline. Verification is done by manually compiling and running each program.

---

## Code Conventions

### Function signature
Always use `int main(void)` — never `int main()`. The latter produces a `-Wstrict-prototypes` warning.

```c
// Correct
int main(void) { ... }

// Wrong
int main() { ... }
```

### Input validation
Every `scanf()` call **must** check its return value. Failure to do so is the primary safety issue flagged in code reviews.

```c
// Correct
if (scanf("%d", &num) != 1) {
    fprintf(stderr, "입력 오류\n");
    return 1;
}

// Wrong — do not omit the return value check
scanf("%d", &num);
```

### String input
Use `fgets()` instead of `scanf("%s", ...)` for string input. Strip the trailing newline with `strcspn()`.

```c
char name[64];
fgets(name, sizeof(name), stdin);
name[strcspn(name, "\n")] = '\0';
```

### Error output
Send error messages to `stderr`, not `stdout`.

```c
fprintf(stderr, "오류 메시지\n");
return 1;
```

### Pointer printing
Always cast to `(void *)` when printing pointer values with `%p`.

```c
int x = 42;
int *p = &x;
printf("주소: %p\n", (void *)p);
```

### Return codes
- `return 0;` — success
- `return 1;` — error/failure

### Constants
Use `#define` for named constants at the top of the file.

```c
#define STUDENT_COUNT 5
#define NAME_SIZE 64
```

### Function decomposition
Break logic into helper functions. Each function should do one thing clearly.

```c
double average_score(const int scores[], int n);
int    max_score(const int scores[], int n);
char   to_grade(double avg);
```

Use `const` for array pointer parameters that are read-only.

### Array bounds
Always validate index bounds and user-supplied values before using them.

```c
if (score < 0 || score > 100) {
    fprintf(stderr, "점수 범위 오류\n");
    return 1;
}
```

### Standard includes
Use only standard C library headers:
- `<stdio.h>` — printf, scanf, fgets, fprintf
- `<stdlib.h>` — rand, srand, exit
- `<string.h>` — strcmp, strcspn, strlen
- `<time.h>` — time (for srand seeding)

---

## Lesson File Pattern

Every lesson follows the same three-file pattern:

| File | Purpose |
|---|---|
| `homework.c` | Core assignment — primary exercise for the lesson topic |
| `homework2.c` | Extended assignment — adds constraints or a related problem |
| `game1.c` | Mini-project game — applies the lesson concepts interactively |

When adding new lessons, follow this pattern.

---

## Current Progress (as of 2026-04-10)

| Week | Topic | Status |
|---|---|---|
| Week 1 | Variables, I/O, conditions, loops | In progress |
| Week 2 | Functions, arrays, strings | In progress |
| Week 3 | Pointers, memory | Started |
| Week 4 | File I/O, mini CLI project | Planned |

**Open action items (from CODE_REVIEW.md and LEARNING_ROADMAP.md):**
- Rename `Helloworl.c` → `hello_world.c`
- Fix `int main()` → `int main(void)` in `Helloworl.c`
- Add `scanf` return value check to `Helloworl.c`
- Extend `course/04-pointers-strings/homework.c` with:
  - Minimum value function
  - Ascending sort
  - Input retry logic (up to 3 attempts)

---

## Development Workflow

### Adding a new lesson

1. Create `course/05-<topic>/` directory.
2. Add `README.md` — describe lesson goals, concepts, and assignment specs.
3. Add `homework.c` — core exercise implementing the lesson topic.
4. Add `homework2.c` — an extended variant or related exercise.
5. Add `game1.c` — an interactive game that reinforces the lesson.
6. Update `docs/LEARNING_ROADMAP.md` to reflect the new lesson.
7. Update the root `README.md` summary.

### Modifying existing code

- Read the relevant `course/*/README.md` first to understand the assignment intent.
- Compile with strict flags after every change to catch warnings.
- Do not introduce external libraries — standard C library only.
- Preserve the existing error handling style (`fprintf(stderr, ...)` + `return 1`).

### Git conventions

The repository uses descriptive commit messages in English. Recent examples:
- `"add homework2 and game1 for each lesson, update roadmap and README"`
- `"reorganize lessons by course folders and add daily README summary"`
- `"add C lessons, homework templates, and code review"`

Write commits in the imperative mood, describing what changed and why.

---

## Things to Avoid

- Do not add a Makefile, CMake, or other build system unless explicitly requested — manual compilation is intentional for the learning context.
- Do not add external libraries or package managers.
- Do not create CI/CD configuration unless requested.
- Do not use `int main()` — always `int main(void)`.
- Do not call `scanf()` without checking the return value.
- Do not use `scanf("%s", ...)` for string input — use `fgets()`.
- Do not write to `stdout` for error messages — use `fprintf(stderr, ...)`.
- Do not commit compiled binaries (`*.o`, `*.dSYM`, executables without extension).

---

## Branch Information

- Default branch: `main`
- Active development branch: `claude/add-claude-documentation-ZrxKD`
- Remote: `http://local_proxy@127.0.0.1:19695/git/RionChoi/pro-c`
