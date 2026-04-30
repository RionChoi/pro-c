#include <iostream>

class Time {
private:
    int hour;
    int minute;

    void normalize() {
        if (minute >= 60) {
            hour += minute / 60;
            minute %= 60;
        }
        if (hour >= 24) {
            hour %= 24;
        }
    }

public:
    Time(int h, int m) : hour(h), minute(m) {
        normalize();
    }

    Time operator+(const Time& other) const {
        return Time(hour + other.hour, minute + other.minute);
    }

    bool operator<(const Time& other) const {
        if (hour != other.hour) {
            return hour < other.hour;
        }
        return minute < other.minute;
    }

    friend std::ostream& operator<<(std::ostream& os, const Time& t) {
        if (t.hour < 10) {
            os << '0';
        }
        os << t.hour << ':';
        if (t.minute < 10) {
            os << '0';
        }
        os << t.minute;
        return os;
    }
};

int main() {
    const Time study(1, 45);
    const Time breakTime(0, 30);
    const Time total = study + breakTime;

    std::cout << "Study: " << study << '\n';
    std::cout << "Break: " << breakTime << '\n';
    std::cout << "Total: " << total << '\n';
    std::cout << "Study < Total ? " << ((study < total) ? "true" : "false") << '\n';
    return 0;
}
