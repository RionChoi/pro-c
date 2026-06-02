#include <iostream>
#include <string>
#include <vector>

class QueryBuilder {
    std::string selectClause;
    std::string fromClause;
    std::vector<std::string> whereConditions;
    std::string orderByClause;
    int limitValue = -1;

public:
    QueryBuilder& select(const std::string& columns) {
        selectClause = "SELECT " + columns;
        return *this;
    }

    QueryBuilder& from(const std::string& table) {
        fromClause = "FROM " + table;
        return *this;
    }

    QueryBuilder& where(const std::string& condition) {
        whereConditions.push_back(condition);
        return *this;
    }

    QueryBuilder& orderBy(const std::string& column) {
        orderByClause = "ORDER BY " + column;
        return *this;
    }

    QueryBuilder& limit(int count) {
        limitValue = count;
        return *this;
    }

    std::string build() const {
        std::string query = selectClause + " " + fromClause;

        if (!whereConditions.empty()) {
            query += " WHERE ";
            for (size_t i = 0; i < whereConditions.size(); ++i) {
                if (i > 0) query += " AND ";
                query += whereConditions[i];
            }
        }

        if (!orderByClause.empty()) {
            query += " " + orderByClause;
        }

        if (limitValue > 0) {
            query += " LIMIT " + std::to_string(limitValue);
        }

        return query;
    }
};

int main() {
    std::cout << "=== SQL 쿼리 빌더 (Fluent Interface) ===" << std::endl;

    std::string query1 = QueryBuilder()
        .select("id, name, email")
        .from("users")
        .where("age > 18")
        .orderBy("name")
        .build();

    std::cout << "\n쿼리 1:" << std::endl;
    std::cout << query1 << std::endl;

    std::string query2 = QueryBuilder()
        .select("*")
        .from("products")
        .where("price < 100")
        .where("stock > 0")
        .limit(10)
        .build();

    std::cout << "\n쿼리 2:" << std::endl;
    std::cout << query2 << std::endl;

    std::cout << "\n✓ DSL을 통해 직관적으로 쿼리를 구성할 수 있습니다!" << std::endl;

    return 0;
}
