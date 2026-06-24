// Session 51: C++26 status - compiler and feature-test macro report
#include <cstddef>
#include <iostream>
#include <string_view>

struct FeatureStatus {
    std::string_view name;
    long value;
    long minimum;
};

constexpr std::string_view compiler_name() {
#if defined(__apple_build_version__)
    return "Apple Clang";
#elif defined(__clang__)
    return "Clang";
#elif defined(__GNUC__)
    return "GCC";
#elif defined(_MSC_VER)
    return "MSVC";
#else
    return "Unknown compiler";
#endif
}

constexpr std::string_view support_label(const FeatureStatus& feature) {
    if (feature.value == 0) {
        return "unavailable";
    }
    if (feature.value < feature.minimum) {
        return "partial/older";
    }
    return "supported";
}

int main() {
#ifdef __cpp_placeholder_variables
    constexpr long placeholder_variables = __cpp_placeholder_variables;
#else
    constexpr long placeholder_variables = 0;
#endif

#ifdef __cpp_pack_indexing
    constexpr long pack_indexing = __cpp_pack_indexing;
#else
    constexpr long pack_indexing = 0;
#endif

#ifdef __cpp_static_assert
    constexpr long static_assert_message = __cpp_static_assert;
#else
    constexpr long static_assert_message = 0;
#endif

#ifdef __cpp_constexpr
    constexpr long constexpr_revision = __cpp_constexpr;
#else
    constexpr long constexpr_revision = 0;
#endif

    constexpr FeatureStatus features[] = {
        {"placeholder variables", placeholder_variables, 202306L},
        {"pack indexing", pack_indexing, 202311L},
        {"user-generated static_assert message", static_assert_message, 202306L},
        {"C++26 constexpr updates", constexpr_revision, 202306L},
    };

    std::cout << "Compiler: " << compiler_name() << '\n';
    std::cout << "__cplusplus: " << __cplusplus << "\n\n";
    std::cout << "Feature-test macro report\n";

    for (const FeatureStatus& feature : features) {
        std::cout << "- " << feature.name << ": " << support_label(feature);
        if (feature.value != 0) {
            std::cout << " (" << feature.value << ')';
        }
        std::cout << '\n';
    }
}
