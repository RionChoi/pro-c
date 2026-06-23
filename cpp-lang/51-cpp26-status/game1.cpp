// Session 51: C++26 status - adoption readiness simulator
#include <algorithm>
#include <array>
#include <iostream>
#include <string_view>

struct Capability {
    std::string_view name;
    bool supported;
};

struct ProjectProfile {
    std::string_view name;
    std::array<std::size_t, 3> required_capabilities;
    std::size_t requirement_count;
};

int main() {
#ifdef __cpp_pack_indexing
    constexpr bool has_pack_indexing = __cpp_pack_indexing >= 202311L;
#else
    constexpr bool has_pack_indexing = false;
#endif

#ifdef __cpp_static_assert
    constexpr bool has_static_assert_message = __cpp_static_assert >= 202306L;
#else
    constexpr bool has_static_assert_message = false;
#endif

#ifdef __cpp_placeholder_variables
    constexpr bool has_placeholder_variables = __cpp_placeholder_variables >= 202306L;
#else
    constexpr bool has_placeholder_variables = false;
#endif

    constexpr std::array capabilities = {
        Capability{"pack indexing", has_pack_indexing},
        Capability{"user-generated static_assert message", has_static_assert_message},
        Capability{"placeholder variables", has_placeholder_variables},
    };

    constexpr std::array profiles = {
        ProjectProfile{"portable library", {1, 0, 0}, 1},
        ProjectProfile{"template toolkit", {0, 1, 0}, 2},
        ProjectProfile{"C++26 experiment", {0, 1, 2}, 3},
    };

    std::cout << "=== C++26 adoption readiness ===\n";
    for (const ProjectProfile& profile : profiles) {
        const auto begin = profile.required_capabilities.begin();
        const auto end = begin + static_cast<std::ptrdiff_t>(profile.requirement_count);
        const std::size_t native_count = static_cast<std::size_t>(
            std::count_if(begin, end, [&](std::size_t index) {
                return capabilities[index].supported;
            }));
        const int score = static_cast<int>(
            (native_count * 100U) / profile.requirement_count);

        std::cout << "\n" << profile.name << ": " << score << "% native\n";
        for (auto it = begin; it != end; ++it) {
            const Capability& capability = capabilities[*it];
            std::cout << "  [" << (capability.supported ? "native" : "fallback")
                      << "] " << capability.name << '\n';
        }
    }

    std::cout << "\nRecommendation: gate each feature with its SD-6 macro.\n";
}
