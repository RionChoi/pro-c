#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <array>

enum class Suit { Hearts, Diamonds, Clubs, Spades };
enum class Rank {
    Two = 2, Three, Four, Five, Six, Seven, Eight, Nine, Ten,
    Jack = 11, Queen = 12, King = 13, Ace = 14
};

const char* suit_names[] = {"♥", "♦", "♣", "♠"};
const char* rank_names[] = {"", "", "2", "3", "4", "5", "6", "7", "8", "9",
                            "10", "J", "Q", "K", "A"};

struct Card {
    Suit suit;
    Rank rank;

    int value() const { return static_cast<int>(rank); }

    void display() const {
        std::cout << rank_names[value()] << suit_names[static_cast<int>(suit)];
    }
};

template <typename CardCollection>
class Hand {
    CardCollection cards_;

public:
    void add(const Card& card) {
        cards_.push_back(card);
    }

    int get_total() const {
        int total = 0;
        int aces = 0;
        for (const auto& card : cards_) {
            int val = card.value();
            if (val == 14) {
                aces++;
                total += 11;
            } else if (val >= 11) {
                total += 10;
            } else {
                total += val;
            }
        }

        while (total > 21 && aces > 0) {
            total -= 10;
            aces--;
        }

        return total;
    }

    void display() const {
        for (size_t i = 0; i < cards_.size(); ++i) {
            if (i > 0) std::cout << " ";
            cards_[i].display();
        }
    }

    void clear() {
        cards_.clear();
    }

    bool is_bust() const {
        return get_total() > 21;
    }

    const Card& get_card(size_t index) const {
        return cards_[index];
    }
};

class BlackjackDealer {
    std::mt19937 rng_;
    std::vector<Card> deck_;

    void init_deck() {
        deck_.clear();
        for (int s = 0; s < 4; ++s) {
            for (int r = 2; r <= 14; ++r) {
                deck_.push_back({
                    static_cast<Suit>(s),
                    static_cast<Rank>(r)
                });
            }
        }
        std::shuffle(deck_.begin(), deck_.end(), rng_);
    }

public:
    BlackjackDealer()
        : rng_(std::random_device{}()) {
        init_deck();
    }

    Card deal() {
        if (deck_.empty()) init_deck();
        Card card = deck_.back();
        deck_.pop_back();
        return card;
    }

    void play_round() {
        Hand<std::vector<Card>> player_hand;
        Hand<std::vector<Card>> dealer_hand;

        std::cout << "\n=== 새 라운드 ===\n";

        player_hand.add(deal());
        dealer_hand.add(deal());
        player_hand.add(deal());
        dealer_hand.add(deal());

        std::cout << "딜러: ";
        dealer_hand.get_card(0).display();
        std::cout << " ?\n";
        std::cout << "플레이어: ";
        player_hand.display();
        std::cout << " (합: " << player_hand.get_total() << ")\n";

        while (true) {
            std::cout << "\n(H)것, (S)탁 (기타=탁): ";
            char c;
            std::cin >> c;

            if (c == 'H' || c == 'h') {
                player_hand.add(deal());
                std::cout << "플레이어: ";
                player_hand.display();
                std::cout << " (합: " << player_hand.get_total() << ")\n";

                if (player_hand.is_bust()) {
                    std::cout << "버스트! 딜러 승리!\n";
                    return;
                }
            } else {
                break;
            }
        }

        std::cout << "\n딜러: ";
        dealer_hand.display();
        std::cout << " (합: " << dealer_hand.get_total() << ")\n";

        while (dealer_hand.get_total() < 17) {
            dealer_hand.add(deal());
            std::cout << "딜러 카드 추가: ";
            dealer_hand.display();
            std::cout << " (합: " << dealer_hand.get_total() << ")\n";
        }

        if (dealer_hand.is_bust()) {
            std::cout << "딜러 버스트! 플레이어 승리!\n";
        } else if (player_hand.get_total() > dealer_hand.get_total()) {
            std::cout << "플레이어 승리!\n";
        } else if (player_hand.get_total() < dealer_hand.get_total()) {
            std::cout << "딜러 승리!\n";
        } else {
            std::cout << "동점!\n";
        }
    }
};

int main() {
    BlackjackDealer game;

    std::cout << "=== 블랙잭 게임 ===\n";

    while (true) {
        game.play_round();
        std::cout << "\n다시 하시겠습니까? (Y/N): ";
        char c;
        std::cin >> c;
        if (c != 'Y' && c != 'y') break;
    }

    std::cout << "게임을 종료합니다.\n";
    return 0;
}
