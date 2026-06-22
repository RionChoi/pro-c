#include <iostream>
#include <vector>
#include <string>

class Query {
private:
    std::vector<std::string> select_cols;
    std::string from_table;
    std::string where_clause;
    std::string order_clause;
    int limit_count;

public:
    Query() : from_table(""), where_clause(""), order_clause(""), limit_count(-1) {}

    Query& select(const std::string& col1) {
        select_cols.push_back(col1);
        return *this;
    }

    Query& select(const std::string& col1, const std::string& col2) {
        select_cols.push_back(col1);
        select_cols.push_back(col2);
        return *this;
    }

    Query& select(const std::string& col1, const std::string& col2, const std::string& col3) {
        select_cols.push_back(col1);
        select_cols.push_back(col2);
        select_cols.push_back(col3);
        return *this;
    }

    Query& from(const std::string& table) {
        from_table = table;
        return *this;
    }

    Query& where(const std::string& condition) {
        where_clause = condition;
        return *this;
    }

    Query& orderBy(const std::string& order) {
        order_clause = order;
        return *this;
    }

    Query& limit(int count) {
        limit_count = count;
        return *this;
    }

    std::string build() const {
        std::string query = "SELECT ";

        // SELECT clause
        if (select_cols.empty()) {
            query += "*";
        } else {
            for (size_t i = 0; i < select_cols.size(); ++i) {
                if (i > 0) query += ", ";
                query += select_cols[i];
            }
        }

        // FROM clause
        if (!from_table.empty()) {
            query += " FROM " + from_table;
        }

        // WHERE clause
        if (!where_clause.empty()) {
            query += " WHERE " + where_clause;
        }

        // ORDER BY clause
        if (!order_clause.empty()) {
            query += " ORDER BY " + order_clause;
        }

        // LIMIT clause
        if (limit_count > 0) {
            query += " LIMIT " + std::to_string(limit_count);
        }

        return query;
    }

    void print() const {
        std::cout << build() << ";" << std::endl;
    }
};

int main() {
    std::cout << "=== SQL 쿼리 빌더 (Fluent Interface) ===" << std::endl << std::endl;

    // Example 1: Simple query
    std::cout << "쿼리 1 - 모든 사용자 조회:" << std::endl;
    Query q1;
    q1.from("users").print();

    // Example 2: With columns
    std::cout << "\n쿼리 2 - 특정 열 선택:" << std::endl;
    Query q2;
    q2.select("id", "name")
        .from("users")
        .print();

    // Example 3: With WHERE
    std::cout << "\n쿼리 3 - WHERE 조건 추가:" << std::endl;
    Query q3;
    q3.select("id", "name", "age")
        .from("users")
        .where("age > 18")
        .print();

    // Example 4: With ORDER BY
    std::cout << "\n쿼리 4 - ORDER BY 추가:" << std::endl;
    Query q4;
    q4.select("name", "age")
        .from("employees")
        .where("department = 'IT'")
        .orderBy("salary DESC")
        .print();

    // Example 5: Full query
    std::cout << "\n쿼리 5 - 완전한 쿼리:" << std::endl;
    Query q5;
    q5.select("id", "name")
        .from("employees")
        .where("salary > 50000")
        .orderBy("name ASC")
        .limit(5)
        .print();

    // Example 6: Minimal query
    std::cout << "\n쿼리 6 - 최소 쿼리 (FROM만):" << std::endl;
    Query q6;
    q6.from("products").print();

    std::cout << std::endl << "=== 쿼리 빌더 성공 ===" << std::endl;

    return 0;
}
