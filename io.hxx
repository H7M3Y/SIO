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
size_t setin_nonblocking() noexcept;
size_t setin_cbreak() noexcept;
size_t setin_noecho() noexcept;
} // namespace io
