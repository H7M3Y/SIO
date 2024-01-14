#pragma once
#include <stddef.h>
namespace io {
inline const size_t ok = -200;
inline const size_t eof = 0;
inline const size_t fatal = -1;
inline const size_t again = -5;
size_t readin(void *dst, size_t n) noexcept;
size_t writeout(const void *src, size_t n) noexcept;
size_t writeerr(const void *src, size_t n) noexcept;
// Note this only set input to be nonblocking!
size_t setin_nonblocking() noexcept;
size_t setin_cbreak() noexcept;
size_t setin_noecho() noexcept;

template <class T, size_t N> inline size_t readins(T (&dst)[N]) noexcept {
  return readin(dst, sizeof(T) * N);
}
template <class T, size_t N>
inline size_t writeouts(const T (&src)[N]) noexcept {
  return writeout(src, sizeof(T) * N - 1);
}
template <class T, size_t N>
inline size_t writeerrs(const T (&src)[N]) noexcept {
  return writeerr(src, sizeof(T) * N - 1);
}

// Please add length specifier in every %c, %s, %[...]!
int parse(const char *str, const char *fmt, ...) noexcept;
// Pass dst nullptr to get dst's size
int format(char *dst, const char *fmt, ...) noexcept;

/**
 *  // Please add length specifier in every %c, %s, %[...]!
 *  size_t readinp(const char *fmt, ...) noexcept;
 *
 *  Deprecated since either
 *    1. Can't mix read() with scanf().
 *    2. Can't determine when to end input.
 */

// Lead to dynamic memory allocation if the length of formatted string
// exceed 63. Not thread safe, that case format() the string yourself.
size_t writeoutf(const char *fmt, ...) noexcept;
// Lead to dynamic memory allocation if the length of formatted string
// exceed 63. Not thread safe, that case format() the string yourself.
size_t writeerrf(const char *fmt, ...) noexcept;
} // namespace io
