# AGENTS.md — pro-c Repository Guide

This file describes the structure, conventions, and development workflow for the **pro-c** repository. It is intended for AI assistants (such as Codex) to understand the codebase and contribute effectively.

---

## Project Overview

**pro-c** is a structured C and C++ language learning project. The core track contains 70 completed sessions: 20 sessions of C followed by 50 sessions of C++. Phase 6 is planned as 10 additional C++26 and production C++ sessions. Each session contains exercises and a practical mini-project. C code uses C11, foundational C++ sessions use C++17, advanced standard sessions use C++20/C++23, and Phase 6 may use C++26 feature experiments where compiler support allows, with no external dependencies.

The learner is **RionChoi**. Documentation is primarily written in Korean.

**Reference Course**: Bro Code YouTube (C Programming Full Course + C++ Full Course)

---

## Repository Structure

```
pro-c/
├── AGENTS.md                          # This file
├── README.md                          # Korean-language project summary
├── Helloworl.c                        # Initial Hello World program
├── docs/
│   ├── LEARNING_ROADMAP.md            # Full roadmap and progress
│   ├── NEXT_PHASE_PLAN.md             # Phase 6 candidate plan
│   ├── CODE_REVIEW.md                 # Code review standards
│   └── SCHEDULE.md                    # Hermes Agent cron job configuration
├── c-lang/                            # C language sessions (01-20)
│   ├── 01-variables-io/
│   │   ├── README.md
│   │   ├── homework.c
│   │   ├── homework2.c
│   │   └── game1.c
│   ├── 02-condition-loop/
│   ├── 03-function-array/
│   ├── 04-pointers-strings/
│   ├── 05-structs/
│   ├── 06-enum-typedef/
│   ├── 07-dynamic-memory/
│   └── ... (up to 20-final-project/)
└── cpp-lang/                          # C++ sessions (01-50 completed, 51-60 planned)
    ├── 01-cpp-basics-iostream/
    │   ├── README.md
    │   ├── homework.cpp
    │   ├── homework2.cpp
    │   └── game1.cpp
    └── ... (up to 50-distributed-kv-store/; 51-60 planned)
```

---

## Building and Running

There is no Makefile or build system. All compilation is done manually.

### C compilation
```bash
cc -Wall -Wextra -Wpedantic <file.c> -o <output>
./<output>
```

### C compilation (strict, recommended for review)
```bash
clang -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wformat=2 \
  -Wnull-dereference -Wdouble-promotion -Wstrict-prototypes <file.c> -o <output>
```

### C++ compilation
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic <file.cpp> -o <output>
./<output>
```

### C++ compilation (strict, recommended for review)
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion <file.cpp> -o <output>
```

### Examples
```bash
# C example
cc -Wall -Wextra -Wpedantic c-lang/01-variables-io/homework.c -o hw1
./hw1

# C++ example
g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/01-cpp-basics-iostream/homework.cpp -o hw1
./hw1
```

There are no tests to run, no linting tools to invoke, and no CI/CD pipeline. Verification is done by manually compiling and running each program.

---

## C Code Conventions

### Function signature
Always use `int main(void)` — never `int main()`. The latter produces a `-Wstrict-prototypes` warning.

```c
// Correct
int main(void) { ... }

// Wrong
int main() { ... }
```

### Input validation
Every `scanf()` call **must** check its return value.

```c
if (scanf("%d", &num) != 1) {
    fprintf(stderr, "입력 오류\n");
    return 1;
}
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

### Standard includes (C only)
- `<stdio.h>` — printf, scanf, fgets, fprintf
- `<stdlib.h>` — rand, srand, exit, malloc, free
- `<string.h>` — strcmp, strcspn, strlen
- `<time.h>` — time (for srand seeding)

---

## C++ Code Conventions

### Function signature
Use `int main()` for C++ (no `void` parameter needed).

```cpp
int main() { ... }
```

### I/O
Use `std::cout` / `std::cin` instead of `printf` / `scanf`.

```cpp
#include <iostream>
#include <string>

int main() {
    std::string name;
    std::cout << "이름을 입력하세요: ";
    std::getline(std::cin, name);
    std::cout << "안녕하세요, " << name << "님!\n";
    return 0;
}
```

### String input
Use `std::getline(std::cin, str)` for full-line string input.

### Naming conventions
- **Classes**: PascalCase (`StudentRecord`, `BankAccount`)
- **Functions/methods**: camelCase or snake_case consistently (`getScore()` or `get_score()`)
- **Constants**: ALL_CAPS (`MAX_STUDENTS`, `BOARD_SIZE`)
- **Variables**: camelCase or snake_case (`studentCount` or `student_count`)

### Memory management
- Prefer smart pointers (`std::unique_ptr`, `std::shared_ptr`) over raw `new`/`delete`
- Use RAII pattern for resource management
- Use `std::vector` instead of C-style arrays when possible

### Parameter passing
- Use `const &` for read-only objects larger than primitives
- Use value for small types (int, double, char)
- Use `&` for output parameters

```cpp
void printStudent(const Student& s);    // const reference
void fillStudent(Student& s);           // reference (output)
double calculate(int x, int y);         // value (small types)
```

### Standard includes (C++ only)
- `<iostream>` — std::cout, std::cin
- `<string>` — std::string, std::getline
- `<vector>` — std::vector
- `<algorithm>` — std::sort, std::find
- `<memory>` — std::unique_ptr, std::shared_ptr
- `<fstream>` — file I/O
- `<cstdlib>` — rand, srand
- `<ctime>` — time

---

## Lesson File Pattern

### C sessions (`c-lang/`)
| File | Purpose |
|---|---|
| `homework.c` | Core assignment — primary exercise for the lesson topic |
| `homework2.c` | Extended assignment — adds constraints or a related problem |
| `game1.c` | Mini-project game — applies the lesson concepts interactively |

### C++ sessions (`cpp-lang/`)
| File | Purpose |
|---|---|
| `homework.cpp` | Core assignment — primary exercise for the lesson topic |
| `homework2.cpp` | Extended assignment — adds constraints or a related problem |
| `game1.cpp` | Mini-project game — applies the lesson concepts interactively |

When adding new sessions, follow this pattern.

---

## Current Progress (as of 2026-06-23)

| Phase | Sessions | Status |
|---|---|---|
| C Language (c-lang/) | 01-20 completed | Complete |
| C++ (cpp-lang/) | 01-50 completed | Complete |
| C++ Phase 6 extension | 51-60 planned | Planned |

Core total: 70/70 sessions completed (C: 20/20, C++: 50/50).
Phase 6 extension: 0/10 sessions completed (C++: 0/10).

See `docs/LEARNING_ROADMAP.md` for detailed progress tracking.

---

## Development Workflow

### Automated daily sessions (`/daily-session` slash command — current)

The primary workflow is the `/daily-session` slash command (`.claude/commands/daily-session.md`):
1. It reads `LEARNING_ROADMAP.md` to find the first incomplete (⬜) session.
2. It generates the session directory with `README.md`, `homework.cpp`, `homework2.cpp`, `game1.cpp`.
3. It compiles and verifies each file with strict flags until warning-free.
4. It updates the roadmap, then commits/pushes and creates + merges a PR.
5. When all sessions are complete, it runs a web search to propose the next Phase.

### Legacy automation (Hermes Agent cron)

Earlier the project ran on a daily cron at 07:00 KST. See `docs/SCHEDULE.md` for the
historical configuration; it has been superseded by `/daily-session`.

### Adding a new session manually

1. Create `c-lang/XX-<topic>/` or `cpp-lang/XX-<topic>/` directory.
2. Add `README.md` — describe lesson goals, concepts, and assignment specs.
3. Add `homework.c/.cpp` — core exercise.
4. Add `homework2.c/.cpp` — an extended variant.
5. Add `game1.c/.cpp` — an interactive game.
6. Update `docs/LEARNING_ROADMAP.md` to reflect the new session.
7. Update the root `README.md` summary.

### Modifying existing code

- Read the relevant `README.md` first to understand the assignment intent.
- Compile with strict flags after every change to catch warnings.
- Do not introduce external libraries — standard library only.
- Preserve the existing error handling style.

### Git conventions

The repository uses descriptive commit messages in English. Write commits in the imperative mood.

Examples:
- `"add session 05-structs with homework, game, and summary"`
- `"complete C phase, begin C++ sessions"`

---

## Things to Avoid

- Do not add a Makefile, CMake, or other build system unless explicitly requested.
- Do not add external libraries or package managers.
- Do not create CI/CD configuration unless requested.
- **C**: Do not use `int main()` — always `int main(void)`.
- **C**: Do not call `scanf()` without checking the return value.
- **C**: Do not use `scanf("%s", ...)` for string input — use `fgets()`.
- Do not write to `stdout` for error messages — use `fprintf(stderr, ...)` (C) or `std::cerr` (C++).
- Do not commit compiled binaries (`*.o`, `*.dSYM`, executables without extension).

---

## Scheduling

The current workflow is the `/daily-session` slash command (run manually per session).
**Hermes Agent** cron is the legacy method; see `docs/SCHEDULE.md` for its historical config.

- Legacy schedule: `0 22 * * *` (UTC) = 07:00 KST
- Legacy job name: `pro-c-daily-learning`
- Completion: when all roadmap sessions are done, `/daily-session` proposes the next Phase.

---

## Branch Information

- Default branch: `main`
- Remote: `http://local_proxy@127.0.0.1:19695/git/RionChoi/pro-c`
