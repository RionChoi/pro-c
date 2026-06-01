# Session 37: 정적 리플렉션 & 고급 메타프로그래밍 (Static Reflection & Advanced Metaprogramming)

## 학습 목표
- 컴파일 타임에 타입 정보를 검사하고 조작하는 기법 습득
- 고급 템플릿 메타프로그래밍 기법 적용
- 타입 특성(Type Traits)을 이용한 제네릭 코드 작성

## 핵심 개념

### 타입 특성 (Type Traits)
- 컴파일 타임에 타입의 특성을 검사하는 메타데이터
- `<type_traits>` 헤더에서 제공

```cpp
#include <type_traits>

// 기본 타입 특성
std::is_integral<int>::value;        // true
std::is_floating_point<double>::value; // true
std::is_pointer<int*>::value;        // true
std::is_const<const int>::value;     // true

// 타입 관계 검사
std::is_same<int, int32_t>::value;   // true (플랫폼 의존적)
std::is_convertible<int, double>::value; // true

// 타입 변환
typename std::remove_pointer<int*>::type; // int
typename std::remove_const<const int>::type; // int
typename std::add_pointer<int>::type;      // int*
```

### SFINAE (Substitution Failure Is Not An Error)
- 템플릿 치환 실패를 오류가 아닌 것처럼 처리
- 조건에 따라 다른 오버로드 선택

```cpp
// enable_if를 이용한 조건부 함수
template <typename T>
typename std::enable_if<std::is_integral<T>::value>::type
print_integer(T val) {
    std::cout << "Integer: " << val << "\n";
}

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value>::type
print_integer(T val) {
    std::cout << "Float: " << val << "\n";
}

// C++20: if constexpr 사용
template <typename T>
void process(T val) {
    if constexpr (std::is_integral_v<T>) {
        std::cout << "Integer type\n";
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << "Float type\n";
    }
}
```

### 리플렉션 패턴
- 런타임에 타입 정보 저장 및 접근
- 타입 이름, 멤버 정보 조회

```cpp
// 기본 struct 정보 저장
struct ReflectedStruct {
    std::string type_name;
    std::vector<std::string> member_names;
    std::vector<std::string> member_types;
};

// 수동 리플렉션 등록
ReflectedStruct register_person() {
    ReflectedStruct info;
    info.type_name = "Person";
    info.member_names = {"name", "age", "score"};
    info.member_types = {"string", "int", "double"};
    return info;
}
```

### 고급 템플릿 메타프로그래밍
```cpp
// 컴파일 타임 계산 (factorial)
template <int N>
struct Factorial {
    static constexpr int value = N * Factorial<N-1>::value;
};

template <>
struct Factorial<0> {
    static constexpr int value = 1;
};

int result = Factorial<5>::value; // 120, 컴파일 타임 계산

// 타입 리스트 조작
template <typename... Types>
struct TypeList {
    static constexpr size_t size = sizeof...(Types);
};

using MyTypes = TypeList<int, double, std::string>;
// MyTypes::size == 3
```

## 할당 (Homework)

### homework.cpp — 타입 특성을 이용한 제네릭 함수
1. `std::is_integral`, `std::is_floating_point` 등을 사용
2. `enable_if`를 이용해 타입별로 다른 처리
3. 정수형과 실수형에 대해 다른 연산 수행
4. 포인터 타입 감지 및 처리

**요구사항**:
- 최소 3가지 타입에 대한 오버로드
- 타입 검사 결과를 출력
- SFINAE 사용

### homework2.cpp — 런타임 리플렉션 구현
1. struct/class의 타입 정보를 런타임에 저장
2. 멤버 정보(이름, 타입) 등록
3. 리플렉션 정보 조회 및 출력
4. 여러 타입에 대해 자동 리플렉션 등록

### game1.cpp — 타입 안전 컨테이너 게임
1. 컴파일 타임에 저장되는 원소 타입 결정
2. 서로 다른 타입의 컨테이너 안전하게 관리
3. 게임 객체(몬스터, 아이템) 타입을 추적
4. 타입별 리스트 출력 및 검증

**요구사항**:
- `std::variant` 또는 템플릿을 이용한 타입 안전 저장
- 타입 정보 출력
- 타입 변환 안전성 확보

## 참고 자료
- `<type_traits>` — is_integral, enable_if, add_pointer 등
- `<typeinfo>` — typeid, type_info
- `if constexpr` (C++17)
- SFINAE와 enable_if 패턴
- 가변 템플릿 (variadic templates)
