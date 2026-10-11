#include "config.hpp"
#include "generator.hpp"
#include "parser.hpp"
#include "secure_string.hpp"
#include <cstdio>
#include <iostream>
#include <string_view>

static constexpr std::string_view VERSION = "0.1.0";

static constexpr std::string_view HELP =
    "Usage: passgen [OPTIONS]\n"
    "\n"
    "Options:\n"
    "  -l, --length <N>     Password length (default: 16, min: 8, max: 128)\n"
    "  -s, --symbols        Include symbols (!@#$%&*-?)\n"
    "  -n, --numeric        Include numeric characters (0-9)\n"
    "  -u, --uppercase      Include uppercase letters (A-Z)\n"
    "  -L, --lowercase      Include lowercase letters (a-z)\n"
    "  -S, --service        Service name (optional label)\n"
    "  -U, --user           Username (optional label)\n"
    "  -h, --help           Displays this help message\n"
    "  -v, --version        Show version\n"
    "\n"
    "Examples:\n"
    "  passgen -sunL -l 20\n"
    "  passgen --symbols --numeric --uppercase --lowercase --length 32\n"
    "  passgen -snuL -S github -U jsaal1\n";

int main(int argc, char **argv) {
    Config config;
    SecureString password;

    try {
        config = parse(argc, argv);
    } catch (const ParseError &pe) {
        switch (pe.errorClass) {
        case ErrorClass::Help:
            std::cout << HELP << '\n';
            return 0;
        case ErrorClass::Version:
            std::cout << VERSION << '\n';
            return 0;
        default:
            std::cerr << "error: " << pe.message << '\n';
            return 1;
        }
    }

    try {
        password = generate(config);
    } catch (const GeneratorError &) {
        std::cerr << "error: unable to find entropy device\n";
        return 1;
    }

    bool copied = false;
#ifdef __APPLE__
    if (FILE *clip = popen("pbcopy", "w")) {
        std::fwrite(password.data(), 1, password.size(), clip);
        copied = pclose(clip) == 0;
    }
#endif
    // TODO: clipboard support for Linux and Windows

    // TODO: add keychain storing if service and user are defined

    std::cout << "Password: " << password << '\n';
    if (copied) {
        std::cout << "Copied to clipboard\n";
    }
}
