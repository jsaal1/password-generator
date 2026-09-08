#ifndef PARSER_HPP
#define PARSER_HPP

#include "config.hpp"

#include <string>

enum class ErrorClass {
    Help,
    Version,
    UnknownOption,
    MissingValue,
    InvalidValue,
    NoCharacterClass
};

struct ParseError {
    ErrorClass errorClass;
    std::string message;
};

Config parse(int argc, char *argv[]);

#endif
