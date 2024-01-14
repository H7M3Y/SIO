#include "io.hxx"
#include <optional>
#if defined(__unix__) || defined(__APPLE__)
#define _tool_hxx_UNIX
#elif defined(_WIN32) || defined(_WIN64)
#define _tool_hxx_WINDOWS
#endif
namespace io {
bool nonblock = false;
#ifdef _tool_hxx_UNIX
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
size_t readin(void *dst, size_t n) noexcept {
  ssize_t rn;
  return (rn = read(STDIN_FILENO, dst, n)) >= 0    ? rn
         : errno == EAGAIN || errno == EWOULDBLOCK ? io::again
                                                   : io::fatal;
}
size_t writeout(const void *src, size_t n) noexcept {
  ssize_t rn;
  return (rn = write(STDOUT_FILENO, src, n)) >= 0 ? rn : io::fatal;
}
size_t writeerr(const void *src, size_t n) noexcept {
  ssize_t rn;
  return (rn = write(STDERR_FILENO, src, n)) >= 0 ? rn : io::fatal;
}
#elif defined(_tool_hxx_WINDOWS)
#include <windows.h>
size_t readin(void *dst, size_t n) noexcept {
  if (nonblock) {
    DWORD bytesRead;
    DWORD totalBytesAvailable;
    BOOL peekResult;
    peekResult = PeekNamedPipe(GetStdHandle(STD_INPUT_HANDLE), NULL, 0, NULL,
                               &totalBytesAvailable, NULL);
    if (!peekResult)
      return io::fatal;
    if (totalBytesAvailable == 0)
      return io::again;
    if (ReadFile(GetStdHandle(STD_INPUT_HANDLE), dst, n, &bytesRead, NULL))
      return bytesRead;
    else
      return io::fatal;
  }
  DWORD bytesRead;
  if (ReadFile(GetStdHandle(STD_INPUT_HANDLE), dst, n, &bytesRead, NULL))
    return bytesRead;
  else
    return io::fatal;
}
size_t writeout(const void *src, size_t n) noexcept {
  DWORD bytesWritten;
  if (WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), src, n, &bytesWritten, NULL))
    return bytesWritten;
  else
    return io::fatal;
}
size_t writeerr(const void *src, size_t n) noexcept {
  DWORD bytesWritten;
  if (WriteFile(GetStdHandle(STD_ERROR_HANDLE), src, n, &bytesWritten, NULL))
    return bytesWritten;
  else
    return io::fatal;
}
#endif
size_t setin_nonblocking() noexcept {
#ifdef _tool_hxx_WINDOWS
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD mode;
  if (!GetConsoleMode(hStdin, &mode))
    return io::fatal;
  mode |= ENABLE_WINDOW_INPUT;
  if (!SetConsoleMode(hStdin, mode))
    return io::fatal;
#elif defined(_tool_hxx_UNIX)
  auto flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  if (flags == -1)
    return io::fatal;
  if (fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK) == -1)
    return io::fatal;
#endif
  nonblock = true;
  return io::ok;
}
#ifdef _tool_hxx_UNIX
struct termios_wrapper {
  struct termios origin_tios;
  std::optional<struct termios> now_tios;
  inline termios_wrapper() noexcept : origin_tios(), now_tios() {
    if (tcgetattr(STDIN_FILENO, &origin_tios) == -1)
      std::terminate();
  }
  inline size_t set_cbreak() noexcept {
    if (!now_tios)
      if (tcgetattr(STDIN_FILENO, &*now_tios) == -1)
        return io::fatal;
    now_tios->c_lflag &= ~ICANON;
    now_tios->c_cc[VMIN] = 1;
    now_tios->c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &*now_tios) == -1)
      return io::fatal;
    return io::ok;
  }
  inline size_t set_noecho() noexcept {
    if (!now_tios)
      if (tcgetattr(STDIN_FILENO, &*now_tios) == -1)
        return io::fatal;
    now_tios->c_lflag &= ~ECHO;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &*now_tios) == -1)
      return io::fatal;
    return io::ok;
  }
  inline ~termios_wrapper() noexcept {
    if (now_tios)
      if (tcsetattr(STDIN_FILENO, TCSADRAIN, &origin_tios) == -1)
        std::terminate();
  }
} tios;
#endif
size_t setin_cbreak() noexcept {
#ifdef _tool_hxx_UNIX
  return tios.set_cbreak();
#elif defined(_tool_hxx_WINDOWS)
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD mode;
  if (!GetConsoleMode(hStdin, &mode))
    return io::fatal;
  mode &= ~ENABLE_LINE_INPUT;
  if (!SetConsoleMode(hStdin, mode))
    return io::fatal;
#endif
  return io::ok;
}
size_t setin_noecho() noexcept {
#ifdef _tool_hxx_UNIX
  return tios.set_noecho();
#elif defined(_tool_hxx_WINDOWS)
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD mode;
  if (!GetConsoleMode(hStdin, &mode))
    return io::fatal;
  mode &= ~ENABLE_ECHO_INPUT;
  if (!SetConsoleMode(hStdin, mode))
    return io::fatal;
#endif
  return io::ok;
}
} // namespace io
