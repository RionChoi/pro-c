#ifndef CONTRACT_HPP
#define CONTRACT_HPP

#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace lesson_contracts {

enum class CheckKind {
    Precondition,
    Postcondition,
    Assertion,
};

constexpr std::string_view kindName(CheckKind kind)
{
    switch (kind) {
        case CheckKind::Precondition:
            return "precondition";
        case CheckKind::Postcondition:
            return "postcondition";
        case CheckKind::Assertion:
            return "assertion";
    }

    return "unknown";
}

struct ViolationInfo {
    CheckKind kind;
    std::string expression;
    std::string message;
    std::string file;
    int line;
    std::string function;
};

inline std::string buildMessage(const ViolationInfo& info)
{
    std::ostringstream out;
    out << "contract " << kindName(info.kind) << " failed: " << info.expression;

    if (!info.message.empty()) {
        out << " (" << info.message << ")";
    }

    out << " at " << info.file << ':' << info.line << " in " << info.function;
    return out.str();
}

class ContractViolation : public std::logic_error {
public:
    explicit ContractViolation(ViolationInfo info)
        : std::logic_error(buildMessage(info)),
          info_(std::move(info))
    {
    }

    const ViolationInfo& info() const noexcept
    {
        return info_;
    }

private:
    ViolationInfo info_;
};

[[noreturn]] inline void fail(CheckKind kind,
                              std::string_view expression,
                              std::string_view message,
                              std::string_view file,
                              int line,
                              std::string_view function)
{
    throw ContractViolation(ViolationInfo{
        kind,
        std::string(expression),
        std::string(message),
        std::string(file),
        line,
        std::string(function),
    });
}

inline void check(bool condition,
                  CheckKind kind,
                  std::string_view expression,
                  std::string_view message,
                  std::string_view file,
                  int line,
                  std::string_view function)
{
    if (!condition) {
        fail(kind, expression, message, file, line, function);
    }
}

#if defined(__cpp_contracts) && __cpp_contracts >= 202502L
inline constexpr bool nativeContractsAvailable = true;
#else
inline constexpr bool nativeContractsAvailable = false;
#endif

}  // namespace lesson_contracts

#define CONTRACT_PRE(expression)                                                                 \
    ::lesson_contracts::check(static_cast<bool>(expression),                                      \
                              ::lesson_contracts::CheckKind::Precondition,                       \
                              #expression,                                                       \
                              "",                                                               \
                              __FILE__,                                                          \
                              __LINE__,                                                          \
                              __func__)

#define CONTRACT_PRE_MSG(expression, message)                                                     \
    ::lesson_contracts::check(static_cast<bool>(expression),                                      \
                              ::lesson_contracts::CheckKind::Precondition,                       \
                              #expression,                                                       \
                              (message),                                                         \
                              __FILE__,                                                          \
                              __LINE__,                                                          \
                              __func__)

#define CONTRACT_POST(expression)                                                                \
    ::lesson_contracts::check(static_cast<bool>(expression),                                      \
                              ::lesson_contracts::CheckKind::Postcondition,                      \
                              #expression,                                                       \
                              "",                                                               \
                              __FILE__,                                                          \
                              __LINE__,                                                          \
                              __func__)

#define CONTRACT_POST_MSG(expression, message)                                                    \
    ::lesson_contracts::check(static_cast<bool>(expression),                                      \
                              ::lesson_contracts::CheckKind::Postcondition,                      \
                              #expression,                                                       \
                              (message),                                                         \
                              __FILE__,                                                          \
                              __LINE__,                                                          \
                              __func__)

#define CONTRACT_ASSERT(expression)                                                              \
    ::lesson_contracts::check(static_cast<bool>(expression),                                      \
                              ::lesson_contracts::CheckKind::Assertion,                          \
                              #expression,                                                       \
                              "",                                                               \
                              __FILE__,                                                          \
                              __LINE__,                                                          \
                              __func__)

#define CONTRACT_ASSERT_MSG(expression, message)                                                  \
    ::lesson_contracts::check(static_cast<bool>(expression),                                      \
                              ::lesson_contracts::CheckKind::Assertion,                          \
                              #expression,                                                       \
                              (message),                                                         \
                              __FILE__,                                                          \
                              __LINE__,                                                          \
                              __func__)

#endif
