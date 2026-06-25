// Session 52: Static reflection - type metadata printer
#include "reflection.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>

struct Developer {
    std::string name;
    int experience_years;
    bool available;
};

template <>
struct meta::Reflect<Developer> {
    static constexpr std::string_view type_name = "Developer";

    static constexpr auto fields() {
        return std::tuple{
            meta::field("name", &Developer::name),
            meta::field("experience_years", &Developer::experience_years),
            meta::field("available", &Developer::available),
        };
    }
};

template <typename T>
constexpr std::string_view category() {
    using Type = std::remove_cvref_t<T>;
    if constexpr (std::same_as<Type, std::string>) {
        return "string";
    } else if constexpr (std::same_as<Type, bool>) {
        return "boolean";
    } else if constexpr (std::integral<Type>) {
        return "integer";
    } else if constexpr (std::floating_point<Type>) {
        return "floating-point";
    } else {
        return "object";
    }
}

int main() {
    const Developer developer{"Rion", 4, true};

    std::cout << "Reflection mode: "
              << (meta::has_native_reflection ? "native" : "portable facade")
              << '\n';
    std::cout << "Type: " << meta::Reflect<Developer>::type_name
              << " (" << meta::field_count<Developer> << " fields)\n";

    meta::for_each_field(developer, [](std::string_view name, const auto& value) {
        std::cout << "- " << name << " [" << category<decltype(value)>() << "] = ";
        if constexpr (std::same_as<std::remove_cvref_t<decltype(value)>, bool>) {
            std::cout << std::boolalpha;
        }
        std::cout << value << '\n';
    });
}
