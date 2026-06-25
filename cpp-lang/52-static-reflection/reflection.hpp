#ifndef PRO_C_SESSION_52_REFLECTION_HPP
#define PRO_C_SESSION_52_REFLECTION_HPP

#include <concepts>
#include <cstddef>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace meta {

template <typename Owner, typename Member>
struct Field {
    std::string_view name;
    Member Owner::*pointer;
};

template <typename Owner, typename Member>
constexpr auto field(std::string_view name, Member Owner::*pointer) {
    return Field<Owner, Member>{name, pointer};
}

template <typename T>
struct Reflect;

template <typename T>
using ReflectedType = std::remove_cvref_t<T>;

template <typename T>
concept Reflectable = requires {
    { Reflect<ReflectedType<T>>::type_name } -> std::convertible_to<std::string_view>;
    Reflect<ReflectedType<T>>::fields();
};

template <Reflectable T, typename Visitor>
constexpr void for_each_field(T&& object, Visitor&& visitor) {
    using Type = ReflectedType<T>;
    std::apply(
        [&](const auto&... descriptor) {
            (visitor(descriptor.name,
                     std::forward<T>(object).*(descriptor.pointer)),
             ...);
        },
        Reflect<Type>::fields());
}

template <Reflectable T>
inline constexpr std::size_t field_count =
    std::tuple_size_v<decltype(Reflect<ReflectedType<T>>::fields())>;

#ifdef __cpp_impl_reflection
inline constexpr bool has_native_reflection = true;
#else
inline constexpr bool has_native_reflection = false;
#endif

}  // namespace meta

#endif
