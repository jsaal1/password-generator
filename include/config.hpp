#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cstdint>
#include <optional>
#include <string>

enum class PasswordFlags : std::uint8_t {
    None = 0,
    Symbols = 1 << 0,
    Numeric = 1 << 1,
    Lowercase = 1 << 2,
    Uppercase = 1 << 3
};

constexpr PasswordFlags operator|(PasswordFlags a, PasswordFlags b) {
    return static_cast<PasswordFlags>(static_cast<std::uint8_t>(a) |
                                      static_cast<std::uint8_t>(b));
}

constexpr PasswordFlags operator&(PasswordFlags a, PasswordFlags b) {
    return static_cast<PasswordFlags>(static_cast<std::uint8_t>(a) &
                                      static_cast<std::uint8_t>(b));
}

constexpr PasswordFlags &operator|=(PasswordFlags &a, PasswordFlags b) {
    return a = a | b;
}

struct Config {
    constexpr static std::uint8_t MIN_LENGTH = 8;
    constexpr static std::uint8_t MAX_LENGTH = 128;
    constexpr static std::uint8_t DEFAULT = 16;

    std::uint8_t length{Config::DEFAULT};
    PasswordFlags flags{PasswordFlags::None};

    std::optional<std::string> serviceName;
    std::optional<std::string> username;

    constexpr bool has(PasswordFlags f) const {
        return (flags & f) != PasswordFlags::None;
    }
};

#endif
