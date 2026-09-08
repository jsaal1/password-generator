# passgen

A small command-line password generator written in C++23. Generates
cryptographically random passwords from configurable character classes and
copies the result straight to the clipboard.

## Features

- Character classes: lowercase, uppercase, numeric, symbols — mix and match
- Configurable length (8–128 characters, default 16)
- Guarantees at least one character from each selected class
- Uses `std::random_device` for entropy
- Password memory is wiped on deallocation (`SecureString`, see
  `include/secure_string.hpp`)
- Copies the generated password to the clipboard (macOS `pbcopy` currently)

## Build

Requires CMake 3.25+ and a C++23 compiler.

```sh
cmake -B build
cmake --build build
```

The resulting binary is `build/passgen`.

## Usage

```
Usage: passgen [OPTIONS]

Options:
  -l, --length <N>     Password length (default: 16, min: 8, max: 128)
  -s, --symbols        Include symbols (!@#$%&^-?)
  -n, --numeric        Include numeric characters (0-9)
  -u, --uppercase      Include uppercase letters (A-Z)
  -L, --lowercase      Include lowercase letters (a-z)
  -S, --service        Service name (optional label)
  -U, --user           Username (optional label)
  -h, --help           Displays this help message
  -v, --version        Show version
```

At least one character class must be enabled.

### Examples

```sh
# 20-character password with symbols, numbers, upper and lowercase
passgen -sunl 20

# same, using long options
passgen --symbols --numeric --uppercase --lowercase --length 32

# label a password with a service and username (label is not yet persisted)
passgen -snuL -S github -U jsaal1
```

## Status

This is a work in progress. Notable gaps:

- Only macOS clipboard support (`pbcopy`) is implemented; other platforms
  print the password but don't copy it.
- `--service`/`--user` are accepted but not yet stored anywhere (no keychain
  integration yet).

## License

MIT — see [LICENSE](LICENSE).
