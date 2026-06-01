#include <iostream>
#include <string>
#include <vector>

using namespace std;

class QueryBuilder {
private:
    string select_clause;
    string from_clause;
    vector<string> where_clauses;
    string order_clause;
    int limit_value;

public:
    QueryBuilder() : limit_value(-1) {}

    QueryBuilder& select(const string& columns) {
        select_clause = "SELECT " + columns;
        return *this;
    }

    QueryBuilder& from(const string& table) {
        from_clause = " FROM " + table;
        return *this;
    }

    QueryBuilder& where(const string& condition) {
        if (where_clauses.empty()) {
            where_clauses.push_back(" WHERE " + condition);
        } else {
            where_clauses.back() += " AND " + condition;
        }
        return *this;
    }

    QueryBuilder& orderBy(const string& column, const string& direction = "ASC") {
        order_clause = " ORDER BY " + column + " " + direction;
        return *this;
    }

    QueryBuilder& limit(int count) {
        limit_value = count;
        return *this;
    }

    string build() const {
        string query = select_clause + from_clause;
        for (const auto& w : where_clauses) {
            query += w;
        }
        if (!order_clause.empty()) {
            query += order_clause;
        }
        if (limit_value > 0) {
            query += " LIMIT " + to_string(limit_value);
        }
        return query;
    }
};

int main() {
    cout << "=== Fluent Interface 기초 ===" << endl << endl;

    cout << "1. 간단한 쿼리" << endl;
    string q1 = QueryBuilder()
        .select("id, name, age")
        .from("students")
        .build();
    cout << "   " << q1 << endl << endl;

    cout << "2. WHERE 조건 추가" << endl;
    string q2 = QueryBuilder()
        .select("id, name")
        .from("students")
        .where("age > 18")
        .where("grade = 'A'")
        .build();
    cout << "   " << q2 << endl << endl;

    cout << "3. 정렬 및 제한" << endl;
    string q3 = QueryBuilder()
        .select("*")
        .from("products")
        .where("price < 100")
        .orderBy("name", "ASC")
        .limit(10)
        .build();
    cout << "   " << q3 << endl << endl;

    cout << "4. 복잡한 쿼리 (메서드 체이닝)" << endl;
    string q4 = QueryBuilder()
        .select("id, username, email")
        .from("users")
        .where("status = 'active'")
        .where("created_at > '2020-01-01'")
        .orderBy("created_at", "DESC")
        .limit(50)
        .build();
    cout << "   " << q4 << endl << endl;

    cout << "✨ Fluent Interface는 메서드 체이닝으로 자연스러운 문장처럼 코드 작성!" << endl;

    return 0;
}
