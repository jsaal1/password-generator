#ifndef SECURE_STRING_HPP
#define SECURE_STRING_HPP

#include <cstddef>
#include <memory>
#include <ostream>
#include <vector>

inline void secure_wipe(void *p, std::size_t len) {
    auto *vp = static_cast<volatile unsigned char *>(p);
    while (len--) {
        *vp++ = 0;
    }
}

template <typename T> struct SecureAllocator : std::allocator<T> {

    using value_type = T;

    SecureAllocator() noexcept = default;
    template <typename U>
    SecureAllocator(const SecureAllocator<U> &) noexcept {}

    template <typename U> struct rebind {
        using other = SecureAllocator<U>;
    };

    void deallocate(T *p, std::size_t n) {
        secure_wipe(p, n * sizeof(T));
        std::allocator<T>::deallocate(p, n);
    }
};

template <typename T, typename U>
bool operator==(const SecureAllocator<T> &,
                const SecureAllocator<U> &) noexcept {
    return true;
}

template <typename T, typename U>
bool operator!=(const SecureAllocator<T> &,
                const SecureAllocator<U> &) noexcept {
    return false;
}

// vector is used instead of basic_string to avoid SSO — strings under ~15-22
// chars (impl-defined) never heap-allocate, so deallocate (and the wipe) would
// never fire for short passwords.
using SecureString = std::vector<char, SecureAllocator<char>>;

inline std::ostream &operator<<(std::ostream &os, const SecureString &s) {
    return os.write(s.data(), static_cast<std::streamsize>(s.size()));
}

#endif
