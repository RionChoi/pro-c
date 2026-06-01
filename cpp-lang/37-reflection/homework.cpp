#include <iostream>
#include <type_traits>
#include <string>

// SFINAE를 이용한 정수형 처리
template <typename T>
typename std::enable_if<std::is_integral<T>::value>::type
process_value(T val) {
    std::cout << "정수형 (" << typeid(T).name() << "): " << val;
    std::cout << " - 제곱: " << (val * val);
    std::cout << " - 부호: " << (val > 0 ? "양수" : (val < 0 ? "음수" : "영")) << "\n";
}

// SFINAE를 이용한 실수형 처리
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value>::type
process_value(T val) {
    std::cout << "실수형 (" << typeid(T).name() << "): " << val;
    std::cout << " - 제곱: " << (val * val);
    std::cout << " - 절댓값: " << (val > 0 ? val : -val) << "\n";
}

// SFINAE를 이용한 포인터 처리
template <typename T>
typename std::enable_if<std::is_pointer<T>::value>::type
process_value(T val) {
    std::cout << "포인터 (" << typeid(T).name() << "): ";
    if (val != nullptr) {
        std::cout << "가리키는 주소: " << (void*)val << " - 역참조: " << *val;
    } else {
        std::cout << "null 포인터";
    }
    std::cout << "\n";
}

// SFINAE를 이용한 string 처리
template <typename T>
typename std::enable_if<std::is_same<T, std::string>::value>::type
process_value(T val) {
    std::cout << "문자열 (" << typeid(T).name() << "): \"" << val;
    std::cout << "\" - 길이: " << val.length() << "\n";
}

// 타입 검사 함수
template <typename T>
void print_type_info() {
    std::cout << "\n[타입: " << typeid(T).name() << "]\n";
    std::cout << "  정수형: " << (std::is_integral<T>::value ? "예" : "아니오") << "\n";
    std::cout << "  실수형: " << (std::is_floating_point<T>::value ? "예" : "아니오") << "\n";
    std::cout << "  포인터: " << (std::is_pointer<T>::value ? "예" : "아니오") << "\n";
    std::cout << "  const 수식자: " << (std::is_const<T>::value ? "예" : "아니오") << "\n";

    if constexpr (std::is_integral<T>::value) {
        std::cout << "  부호있음: " << (std::is_signed<T>::value ? "예" : "아니오") << "\n";
    }
}

int main() {
    std::cout << "=== 타입 특성 및 SFINAE 테스트 ===\n";

    // 정수형 테스트
    std::cout << "\n[정수형 처리]\n";
    print_type_info<int>();
    process_value(42);
    process_value(-7);

    // 실수형 테스트
    std::cout << "\n[실수형 처리]\n";
    print_type_info<double>();
    process_value(3.14);
    process_value(-2.71);

    // 포인터 테스트
    std::cout << "\n[포인터 처리]\n";
    print_type_info<int*>();
    int x = 100;
    process_value(&x);
    int* null_ptr = nullptr;
    process_value(null_ptr);

    // 문자열 테스트
    std::cout << "\n[문자열 처리]\n";
    print_type_info<std::string>();
    process_value(std::string("Hello C++"));

    // 부호있는/없는 정수형 비교
    std::cout << "\n[부호 검사]\n";
    std::cout << "int (부호있음): " << (std::is_signed<int>::value ? "예" : "아니오") << "\n";
    std::cout << "unsigned int (부호없음): " << (std::is_signed<unsigned int>::value ? "예" : "아니오") << "\n";

    // 타입 변환 가능성
    std::cout << "\n[타입 변환 가능성]\n";
    std::cout << "int → double: " << (std::is_convertible<int, double>::value ? "가능" : "불가") << "\n";
    std::cout << "double → int: " << (std::is_convertible<double, int>::value ? "가능" : "불가") << "\n";
    std::cout << "int* → double*: " << (std::is_convertible<int*, double*>::value ? "가능" : "불가") << "\n";

    std::cout << "\n✓ 타입 특성 및 SFINAE 처리 완료\n";
    return 0;
}
