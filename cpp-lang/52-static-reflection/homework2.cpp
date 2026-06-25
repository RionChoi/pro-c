// Session 52: Static reflection - recursive JSON serialization
#include "reflection.hpp"

#include <iomanip>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>

struct Address {
    std::string city;
    std::string country;
};

struct Profile {
    std::string name;
    int level;
    bool active;
    Address address;
};

template <>
struct meta::Reflect<Address> {
    static constexpr std::string_view type_name = "Address";

    static constexpr auto fields() {
        return std::tuple{
            meta::field("city", &Address::city),
            meta::field("country", &Address::country),
        };
    }
};

template <>
struct meta::Reflect<Profile> {
    static constexpr std::string_view type_name = "Profile";

    static constexpr auto fields() {
        return std::tuple{
            meta::field("name", &Profile::name),
            meta::field("level", &Profile::level),
            meta::field("active", &Profile::active),
            meta::field("address", &Profile::address),
        };
    }
};

template <typename T>
void write_json(std::ostream& output, const T& value) {
    using Type = std::remove_cvref_t<T>;

    if constexpr (std::same_as<Type, std::string> ||
                  std::same_as<Type, std::string_view>) {
        output << std::quoted(value);
    } else if constexpr (std::same_as<Type, bool>) {
        output << (value ? "true" : "false");
    } else if constexpr (std::is_arithmetic_v<Type>) {
        output << value;
    } else if constexpr (meta::Reflectable<Type>) {
        bool first = true;
        output << '{';
        meta::for_each_field(value, [&](std::string_view name, const auto& field_value) {
            if (!first) {
                output << ',';
            }
            first = false;
            output << std::quoted(name) << ':';
            write_json(output, field_value);
        });
        output << '}';
    } else {
        static_assert(std::is_same_v<Type, void>, "Unsupported JSON type");
    }
}

int main() {
    const Profile profile{
        "Rion \"C++\" Choi",
        52,
        true,
        {"Seoul", "Korea"},
    };

    std::cout << "Serialized " << meta::Reflect<Profile>::type_name << ":\n";
    write_json(std::cout, profile);
    std::cout << '\n';
}
