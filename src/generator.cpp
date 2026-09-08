#include "generator.hpp"
#include "config.hpp"
#include "secure_string.hpp"

#include <algorithm>
#include <cstddef>
#include <random>
#include <string_view>

constexpr std::string_view f_Lowercase("abcdefghijklmnopqrstuvwxyz");
constexpr std::string_view f_Uppercase("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
constexpr std::string_view f_Numerics("0123456789");
constexpr std::string_view f_Symbols("!@#$%&*-?");

SecureString getAlphabet(const Config &config) {
    SecureString alphabet{};
    if (config.has(PasswordFlags::Lowercase)) {
        alphabet.append_range(f_Lowercase);
    }
    if (config.has(PasswordFlags::Uppercase)) {
        alphabet.append_range(f_Uppercase);
    }
    if (config.has(PasswordFlags::Numeric)) {
        alphabet.append_range(f_Numerics);
    }
    if (config.has(PasswordFlags::Symbols)) {
        alphabet.append_range(f_Symbols);
    }

    return alphabet;
}

void fillCharPerFlag(const Config &config, std::random_device &rd,
                     SecureString &pw) {
    auto pick = [&rd](auto chars) {
        return chars[std::uniform_int_distribution<std::size_t>(
            0, chars.size() - 1)(rd)];
    };
    if (config.has(PasswordFlags::Lowercase)) {
        pw.push_back(pick(f_Lowercase));
    }
    if (config.has(PasswordFlags::Uppercase)) {
        pw.push_back(pick(f_Uppercase));
    }
    if (config.has(PasswordFlags::Numeric)) {
        pw.push_back(pick(f_Numerics));
    }
    if (config.has(PasswordFlags::Symbols)) {
        pw.push_back(pick(f_Symbols));
    }
}

SecureString generate(const Config &config) {
    auto alphabet = getAlphabet(config);
    auto length = config.length;

    std::random_device rd;
    if (rd.entropy() == 0) {
        throw GeneratorError{};
    }

    SecureString password;
    password.reserve(length);

    fillCharPerFlag(config, rd, password);

    std::uniform_int_distribution<std::size_t> dist(0, alphabet.size() - 1);
    while (password.size() < length) {
        password.push_back(alphabet[dist(rd)]);
    }

    std::ranges::shuffle(password, rd);

    return password;
}
