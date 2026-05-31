#include <iostream>
#include <tuple>
#include <string>
#include <type_traits>
#include <cstdlib>
#include <ctime>
#include <vector>

template<typename T>
struct TypeInfo {
    static const char *name() { return "unknown"; }
    static const char *description() { return "unknown type"; }
};

template<>
struct TypeInfo<int> {
    static const char *name() { return "int"; }
    static const char *description() { return "32-bit integer"; }
};

template<>
struct TypeInfo<double> {
    static const char *name() { return "double"; }
    static const char *description() { return "64-bit floating-point"; }
};

template<>
struct TypeInfo<std::string> {
    static const char *name() { return "string"; }
    static const char *description() { return "variable-length text"; }
};

template<>
struct TypeInfo<bool> {
    static const char *name() { return "bool"; }
    static const char *description() { return "boolean (true/false)"; }
};

struct Product {
    int id;
    std::string name;
    double price;
    using Members = std::tuple<int, std::string, double>;
};

struct Account {
    std::string username;
    bool verified;
    int balance;
    using Members = std::tuple<std::string, bool, int>;
};

struct Config {
    std::string version;
    int timeout;
    double threshold;
    bool enabled;
    using Members = std::tuple<std::string, int, double, bool>;
};

class ReflectionGame {
private:
    int score = 0;
    int total_questions = 0;

    enum class QuestionType {
        COUNT_MEMBERS,
        IDENTIFY_TYPE,
        TYPE_SIZE
    };

    struct Question {
        std::string struct_name;
        std::string question_text;
        std::string correct_answer;
        QuestionType type;
    };

    std::vector<Question> questions;

    void create_questions() {
        questions.push_back({
            "Product",
            "Product 구조체의 멤버 개수는?",
            "3",
            QuestionType::COUNT_MEMBERS
        });

        questions.push_back({
            "Account",
            "Account의 세 번째 멤버의 타입은?",
            "int",
            QuestionType::IDENTIFY_TYPE
        });

        questions.push_back({
            "Config",
            "Config 구조체의 멤버 개수는?",
            "4",
            QuestionType::COUNT_MEMBERS
        });

        questions.push_back({
            "Product",
            "Product의 두 번째 멤버의 타입은?",
            "string",
            QuestionType::IDENTIFY_TYPE
        });

        questions.push_back({
            "Account",
            "Account의 첫 번째 멤버의 타입은?",
            "string",
            QuestionType::IDENTIFY_TYPE
        });
    }

public:
    void play() {
        std::cout << "🎮 정적 리플렉션 게임 - 구조체 내성 도전!\n\n";
        std::cout << "컴파일 타임에 반영된 구조체 정보를 맞혀보세요!\n";
        std::cout << "각 질문에 대해 정확한 답변을 입력하세요.\n\n";

        print_struct_info();

        create_questions();
        total_questions = questions.size();

        for (std::size_t i = 0; i < questions.size(); ++i) {
            ask_question(questions[i], i + 1);
        }

        std::cout << "\n🎮 게임 종료!\n";
        std::cout << "최종 점수: " << score << "/" << total_questions << "\n";

        if (score == total_questions) {
            std::cout << "🏆 완벽한 리플렉션 마스터!\n";
        } else if (score >= total_questions * 3 / 4) {
            std::cout << "🌟 훌륭한 성적!\n";
        } else if (score >= total_questions / 2) {
            std::cout << "💪 더 연습해봅시다!\n";
        } else {
            std::cout << "📚 기본부터 다시 살펴보세요!\n";
        }
    }

private:
    void print_struct_info() {
        std::cout << "📋 구조체 정보:\n";
        std::cout << "  • Product: { int id, string name, double price }\n";
        std::cout << "  • Account: { string username, bool verified, int balance }\n";
        std::cout << "  • Config: { string version, int timeout, double threshold, bool enabled }\n\n";
    }

    void ask_question(const Question &q, std::size_t num) {
        std::cout << "문제 " << num << ": [" << q.struct_name << "] "
                  << q.question_text << "\n";
        std::cout << "> ";

        std::string user_answer;
        std::getline(std::cin, user_answer);

        if (user_answer == q.correct_answer) {
            std::cout << "✅ 정답!\n";
            score++;
        } else {
            std::cout << "❌ 오답!\n";
            std::cout << "   정답: " << q.correct_answer << "\n";
        }
        std::cout << "\n";
    }
};

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    ReflectionGame game;
    game.play();

    return 0;
}
