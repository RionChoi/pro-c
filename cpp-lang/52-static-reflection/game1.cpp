// Session 52: Static reflection - type-safe configuration console
#include "reflection.hpp"

#include <charconv>
#include <iostream>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <utility>
#include <vector>

struct ServiceConfig {
    std::string host;
    int port;
    bool tls;
    int workers;
};

template <>
struct meta::Reflect<ServiceConfig> {
    static constexpr std::string_view type_name = "ServiceConfig";

    static constexpr auto fields() {
        return std::tuple{
            meta::field("host", &ServiceConfig::host),
            meta::field("port", &ServiceConfig::port),
            meta::field("tls", &ServiceConfig::tls),
            meta::field("workers", &ServiceConfig::workers),
        };
    }
};

template <typename T>
bool parse_value(std::string_view text, T& value) {
    if constexpr (std::same_as<T, std::string>) {
        value = text;
        return true;
    } else if constexpr (std::same_as<T, bool>) {
        if (text == "true" || text == "1") {
            value = true;
            return true;
        }
        if (text == "false" || text == "0") {
            value = false;
            return true;
        }
        return false;
    } else if constexpr (std::integral<T>) {
        T parsed{};
        const auto [end, error] =
            std::from_chars(text.data(), text.data() + text.size(), parsed);
        if (error != std::errc{} || end != text.data() + text.size()) {
            return false;
        }
        value = parsed;
        return true;
    } else {
        return false;
    }
}

template <meta::Reflectable T>
bool apply_setting(T& object, std::string_view key, std::string_view text) {
    bool matched = false;
    bool parsed = false;

    meta::for_each_field(object, [&](std::string_view name, auto& value) {
        if (!matched && name == key) {
            matched = true;
            parsed = parse_value(text, value);
        }
    });
    return matched && parsed;
}

std::vector<std::string> validate(const ServiceConfig& config) {
    std::vector<std::string> errors;
    if (config.host.empty()) {
        errors.emplace_back("host must not be empty");
    }
    if (config.port < 1 || config.port > 65535) {
        errors.emplace_back("port must be between 1 and 65535");
    }
    if (config.workers < 1 || config.workers > 64) {
        errors.emplace_back("workers must be between 1 and 64");
    }
    return errors;
}

void print_config(const ServiceConfig& config) {
    meta::for_each_field(config, [](std::string_view name, const auto& value) {
        std::cout << "  " << name << " = ";
        if constexpr (std::same_as<std::remove_cvref_t<decltype(value)>, bool>) {
            std::cout << std::boolalpha;
        }
        std::cout << value << '\n';
    });
}

int main() {
    ServiceConfig config{"localhost", 8080, false, 4};
    const std::vector<std::pair<std::string_view, std::string_view>> commands{
        {"host", "api.internal"},
        {"port", "443"},
        {"tls", "true"},
        {"workers", "8"},
        {"unknown", "ignored"},
        {"port", "not-a-number"},
    };

    std::cout << "Applying reflected settings\n";
    for (const auto& [key, value] : commands) {
        std::cout << "- " << key << '=' << value << ": "
                  << (apply_setting(config, key, value) ? "accepted" : "rejected")
                  << '\n';
    }

    std::cout << "\nFinal configuration\n";
    print_config(config);

    const auto errors = validate(config);
    std::cout << "\nValidation: " << (errors.empty() ? "passed" : "failed") << '\n';
    for (const std::string& error : errors) {
        std::cout << "- " << error << '\n';
    }
}
