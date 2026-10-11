#include "parser.hpp"
#include "config.hpp"

#include <algorithm>
#include <charconv>
#include <format>
#include <functional>
#include <initializer_list>
#include <span>
#include <string_view>
#include <unordered_map>

struct OptionHandler {
    bool takes_value;
    std::function<void(Config &, std::string_view)> fn;
};

// Named handlers so short and long forms share the same callable.
static const auto handle_length = [](Config &c, std::string_view val) {
    std::uint32_t tmp{};
    auto result = std::from_chars(val.begin(), val.end(), tmp);
    if (result.ec != std::errc{} || result.ptr != val.end() ||
        tmp < Config::MIN_LENGTH || tmp > Config::MAX_LENGTH) {
        throw ParseError{
            ErrorClass::InvalidValue,
            std::format("invalid length '{}': must be between {} and {}", val,
                        (int)Config::MIN_LENGTH, (int)Config::MAX_LENGTH)};
    }
    c.length = static_cast<std::uint8_t>(tmp);
};

static const auto handle_symbols = [](Config &c, std::string_view) {
    c.flags |= PasswordFlags::Symbols;
};

static const auto handle_numeric = [](Config &c, std::string_view) {
    c.flags |= PasswordFlags::Numeric;
};

static const auto handle_uppercase = [](Config &c, std::string_view) {
    c.flags |= PasswordFlags::Uppercase;
};

static const auto handle_lowercase = [](Config &c, std::string_view) {
    c.flags |= PasswordFlags::Lowercase;
};

static const auto handle_service = [](Config &c, std::string_view val) {
    c.serviceName = std::string(val);
};

static const auto handle_user = [](Config &c, std::string_view val) {
    c.username = std::string(val);
};

static const std::unordered_map<std::string_view, OptionHandler> opts = {
    {"length", {true, handle_length}},
    {"l", {true, handle_length}},
    {"symbols", {false, handle_symbols}},
    {"s", {false, handle_symbols}},
    {"numeric", {false, handle_numeric}},
    {"n", {false, handle_numeric}},
    {"uppercase", {false, handle_uppercase}},
    {"u", {false, handle_uppercase}},
    {"lowercase", {false, handle_lowercase}},
    {"L", {false, handle_lowercase}},
    {"service", {true, handle_service}},
    {"S", {true, handle_service}},
    {"user", {true, handle_user}},
    {"U", {true, handle_user}},
};

Config parse(int argc, char *argv[]) {
    auto args = std::span<char *>(argv + 1, argc - 1);

    auto is_present = [&](std::initializer_list<std::string_view> tokens) {
        return std::ranges::any_of(args, [&](std::string_view arg) {
            return std::ranges::find(tokens, arg) != tokens.end();
        });
    };

    if (is_present({"--help", "-h"}))
        throw ParseError{ErrorClass::Help, ""};
    if (is_present({"--version", "-v"}))
        throw ParseError{ErrorClass::Version, ""};

    Config config{};

    for (auto i{0uz}; i < args.size(); ++i) {
        std::string_view arg{args[i]};

        if (arg == "--") {
            break;
        } else if (arg.starts_with("--")) {
            std::string_view rest = arg.substr(2);
            std::string_view name = rest;
            std::string_view value = {};
            if (auto eq = rest.find('='); eq != std::string_view::npos) {
                name = rest.substr(0, eq);
                value = rest.substr(eq + 1);
            }

            auto it = opts.find(name);
            if (it == opts.end())
                throw ParseError{ErrorClass::UnknownOption,
                                 std::format("unknown option: --{}", name)};

            const auto &handler = it->second;
            if (handler.takes_value && value.empty()) {
                if (i + 1 >= args.size())
                    throw ParseError{
                        ErrorClass::MissingValue,
                        std::format("missing value for --{}", name)};
                value = args[++i];
            }

            handler.fn(config, value);
        } else if (arg.starts_with("-")) {
            std::string_view chars = arg.substr(1);

            for (auto j{0uz}; j < chars.size(); ++j) {
                std::string_view key{chars.data() + j, 1};

                if (key == "h") {
                    throw ParseError{ErrorClass::Help, ""};
                }
                if (key == "v") {
                    throw ParseError{ErrorClass::Version, ""};
                }

                auto it = opts.find(key);
                if (it == opts.end())
                    throw ParseError{
                        ErrorClass::UnknownOption,
                        std::format("unknown option: -{}", chars[j])};

                const auto &handler = it->second;
                if (!handler.takes_value) {
                    handler.fn(config, {});
                } else {
                    std::string_view value = chars.substr(j + 1);
                    if (value.empty()) {
                        if (i + 1 >= args.size())
                            throw ParseError{
                                ErrorClass::MissingValue,
                                std::format("missing value for -{}", chars[j])};
                        value = args[++i];
                    }
                    handler.fn(config, value);
                    break;
                }
            }
        } else {
            throw ParseError{ErrorClass::UnknownOption,
                             std::format("unexpected argument: {}", arg)};
        }
    }

    if (config.flags == PasswordFlags::None)
        throw ParseError{ErrorClass::NoCharacterClass,
                         "at least one character class must be enabled "
                         "(--symbols, --numeric, --uppercase, --lowercase)"};

    return config;
}
