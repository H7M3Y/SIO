#include "io.hxx"
#include <exception>
#include <thread>
int main() {
  char input[4];
  size_t n;
  io::setin_nonblocking();
  io::setin_cbreak();
  io::setin_noecho();
  io::writeout("Enter Ctrl-Q to quit\n", 21);
  while ((n = io::readin(input, 4))) {
    if (n == io::fatal) {
      io::writeerr("fatal: io\n", 10);
      std::terminate();
    }
    if (n == io::again) {
      std::this_thread::yield();
      continue;
    }
    if (*input == 17)
      break;
    io::writeout("You typed ", 10);
    io::writeout(input, n);
    io::writeout("\n", 1);
  }
  io::writeerr("\nEOF\n", 5);
}
