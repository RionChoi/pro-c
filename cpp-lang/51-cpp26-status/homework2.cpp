// Session 51: C++26 status - requirement and fallback compatibility check
#include <iostream>
#include <string_view>

enum class Availability {
    native,
    fallback,
    unavailable,
};

struct Requirement {
    std::string_view feature;
    long detected;
    long minimum;
    bool has_fallback;
};

constexpr Availability classify(const Requirement& requirement) {
    if (requirement.detected >= requirement.minimum) {
        return Availability::native;
    }
    if (requirement.has_fallback) {
        return Availability::fallback;
    }
    return Availability::unavailable;
}

constexpr std::string_view label(Availability availability) {
    switch (availability) {
    case Availability::native:
        return "native";
    case Availability::fallback:
        return "fallback";
    case Availability::unavailable:
        return "unavailable";
    }
    return "unknown";
}

int main() {
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

#ifdef __cpp_placeholder_variables
    constexpr long placeholder_variables = __cpp_placeholder_variables;
#else
    constexpr long placeholder_variables = 0;
#endif

    constexpr Requirement requirements[] = {
        {"pack indexing", pack_indexing, 202311L, true},
        {"user-generated static_assert message", static_assert_message, 202306L, true},
        {"placeholder variables", placeholder_variables, 202306L, false},
    };

    bool can_build = true;
    std::cout << "C++26 compatibility gate\n";
    for (const Requirement& requirement : requirements) {
        const Availability availability = classify(requirement);
        std::cout << "- " << requirement.feature << ": " << label(availability)
                  << " (detected=" << requirement.detected
                  << ", required=" << requirement.minimum << ")\n";
        if (availability == Availability::unavailable) {
            can_build = false;
        }
    }

    std::cout << "\nDecision: "
              << (can_build ? "build with selected fallbacks" : "block until requirements change")
              << '\n';
}
