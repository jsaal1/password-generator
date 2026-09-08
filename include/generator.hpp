#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "config.hpp"
#include "secure_string.hpp"

struct GeneratorError {};

SecureString generate(const Config &config);

#endif
