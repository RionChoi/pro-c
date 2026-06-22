#include <iostream>
#include <string>

class Score {
private:
    int value;

public:
    explicit Score(int v = 0) : value(v) {}

    Score operator+(const Score& other) const {
        return Score(value + other.value);
    }

    Score& operator+=(const Score& other) {
        value += other.value;
        return *this;
    }

    bool operator>(const Score& other) const {
        return value > other.value;
    }

    friend std::ostream& operator<<(std::ostream& os, const Score& s) {
        os << s.value;
        return os;
    }
};

int main() {
    Score player1;
    Score player2;

    player1 += Score(12);
    player1 += Score(8);

    player2 += Score(10);
    player2 += Score(7);

    std::cout << "Player1 score: " << player1 << '\n';
    std::cout << "Player2 score: " << player2 << '\n';

    if (player1 > player2) {
        std::cout << "Winner: Player1\n";
    } else if (player2 > player1) {
        std::cout << "Winner: Player2\n";
    } else {
        std::cout << "Draw\n";
    }

    return 0;
}
