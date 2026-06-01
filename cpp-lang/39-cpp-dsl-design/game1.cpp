#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct Student {
    int id;
    string name;
    int grade;
    int score;
};

class QueryValidator {
public:
    static bool isValidQuery(const string& query, const string& required) {
        return query.find(required) != string::npos;
    }

    static int countMatches(const vector<Student>& students, const string& condition) {
        int count = 0;

        if (condition == "grade > 2") {
            for (const auto& s : students) {
                if (s.grade > 2) count++;
            }
        } else if (condition == "score >= 80") {
            for (const auto& s : students) {
                if (s.score >= 80) count++;
            }
        } else if (condition == "grade = 1") {
            for (const auto& s : students) {
                if (s.grade == 1) count++;
            }
        }

        return count;
    }
};

class GameChallenge {
public:
    string description;
    string hint;
    string goal_query;
    string validation_key;
    int points;

    GameChallenge(string desc, string h, string goal, string key, int pts)
        : description(desc), hint(h), goal_query(goal), validation_key(key), points(pts) {}
};

int main() {
    cout << "=== DSL 게임: SQL 쿼리 빌더 챌린지 ===" << endl << endl;

    vector<Student> students = {
        {1, "Alice", 1, 95},
        {2, "Bob", 2, 78},
        {3, "Charlie", 1, 88},
        {4, "Diana", 3, 92},
        {5, "Eve", 2, 85},
        {6, "Frank", 1, 91},
        {7, "Grace", 3, 88}
    };

    cout << "📚 학생 데이터베이스:" << endl;
    cout << "ID | 이름    | 학년 | 점수" << endl;
    cout << "---|---------|------|-----" << endl;
    for (const auto& s : students) {
        cout << s.id << "  | " << s.name << " | " << s.grade << "    | " << s.score << endl;
    }
    cout << endl;

    vector<GameChallenge> challenges = {
        GameChallenge(
            "1. 1학년 학생 찾기",
            "WHERE 절을 사용하여 grade = 1 조건을 추가하세요",
            "SELECT * FROM students WHERE grade = 1",
            "WHERE grade = 1",
            10
        ),
        GameChallenge(
            "2. 80점 이상 학생 조회",
            "score >= 80 조건으로 필터링하세요",
            "SELECT id, name, score FROM students WHERE score >= 80",
            "WHERE score >= 80",
            15
        ),
        GameChallenge(
            "3. 2학년 이상 학생 정렬",
            "ORDER BY를 사용하여 점수 내림차순으로 정렬하세요",
            "SELECT name, score FROM students WHERE grade > 2 ORDER BY score DESC",
            "ORDER BY score DESC",
            20
        ),
        GameChallenge(
            "4. 상위 3명의 고등학생",
            "3학년 학생을 점수 내림차순으로 정렬한 후 LIMIT를 사용하세요",
            "SELECT name, score FROM students WHERE grade = 3 ORDER BY score DESC LIMIT 1",
            "LIMIT 1",
            25
        )
    };

    int total_points = 0;
    int completed = 0;

    cout << "🎮 게임 시작!" << endl;
    cout << "==================" << endl << endl;

    for (auto& challenge : challenges) {
        cout << challenge.description << endl;
        cout << "힌트: " << challenge.hint << endl;
        cout << "정답 쿼리: " << challenge.goal_query << endl;

        if (QueryValidator::isValidQuery(challenge.goal_query, challenge.validation_key)) {
            cout << "✅ 정답! +" << challenge.points << " 포인트" << endl;
            total_points += challenge.points;
            completed++;
        } else {
            cout << "❌ 오답입니다" << endl;
        }
        cout << endl;
    }

    cout << "==================" << endl;
    cout << "🎯 게임 결과" << endl;
    cout << "완료한 챌린지: " << completed << "/" << challenges.size() << endl;
    cout << "획득 포인트: " << total_points << endl;

    if (completed == (int)challenges.size()) {
        cout << "🏆 완벽합니다! 모든 챌린지를 완료했습니다!" << endl;
    } else if (completed >= 3) {
        cout << "🌟 우수합니다! 계속 노력하세요!" << endl;
    } else {
        cout << "💪 더 열심히 도전해보세요!" << endl;
    }

    cout << endl;
    cout << "💡 DSL을 사용하면 복잡한 쿼리도 읽기 쉬운 코드로 작성할 수 있습니다!" << endl;

    return 0;
}
