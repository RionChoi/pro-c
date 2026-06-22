#include <iostream>
#include <stdexcept>
#include <string>

int main() {
    // 학습: try-catch-throw 기초

    std::cout << "=== 예외 처리 기초 ===\n\n";

    // 1. 간단한 예외 처리
    try {
        std::cout << "시도 1: 정상 코드\n";
        int x = 10, y = 0;

        if (y == 0) {
            throw std::invalid_argument("0으로 나눌 수 없습니다!");
        }

        int result = x / y;
        std::cout << "결과: " << result << "\n";

    } catch (const std::invalid_argument& e) {
        std::cerr << "예외 발생: " << e.what() << "\n\n";
    }

    // 2. std::out_of_range 예외
    try {
        std::cout << "시도 2: 범위 초과\n";
        int arr[5] = {10, 20, 30, 40, 50};
        int index = 10;

        if (index < 0 || index >= 5) {
            throw std::out_of_range("배열 인덱스가 범위를 벗어났습니다!");
        }

        std::cout << "arr[" << index << "] = " << arr[index] << "\n";

    } catch (const std::out_of_range& e) {
        std::cerr << "예외 발생: " << e.what() << "\n\n";
    }

    // 3. 다중 catch 블록
    try {
        std::cout << "시도 3: 다중 예외 처리\n";
        int choice = 2;

        if (choice == 1) {
            throw std::invalid_argument("잘못된 입력입니다!");
        } else if (choice == 2) {
            throw std::runtime_error("런타임 오류 발생!");
        } else if (choice == 3) {
            throw std::out_of_range("범위 초과!");
        }

    } catch (const std::invalid_argument& e) {
        std::cerr << "입력 오류: " << e.what() << "\n";
    } catch (const std::runtime_error& e) {
        std::cerr << "런타임 오류: " << e.what() << "\n";
    } catch (const std::out_of_range& e) {
        std::cerr << "범위 오류: " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "일반 예외: " << e.what() << "\n";
    }
    std::cout << "\n";

    // 4. 예외 재발생
    try {
        std::cout << "시도 4: 예외 재발생\n";
        try {
            throw std::logic_error("내부 오류");
        } catch (const std::logic_error& e) {
            std::cout << "내부에서 잡은 예외: " << e.what() << "\n";
            throw;  // 예외 재발생
        }
    } catch (const std::logic_error& e) {
        std::cerr << "외부에서 잡은 예외: " << e.what() << "\n";
    }
    std::cout << "\n";

    // 5. 모든 예외 처리 (ellipsis)
    try {
        std::cout << "시도 5: 일반 catch-all\n";
        throw std::runtime_error("어떤 오류");
    } catch (...) {
        std::cerr << "알 수 없는 예외가 발생했습니다!\n";
    }

    std::cout << "\n프로그램 정상 종료.\n";
    return 0;
}
