#include "io.hxx"
#include <exception>
#include <thread>
int main() {
  char input[4];
  size_t n;
  io::setin_nonblocking();
  io::setin_cbreak();
  io::setin_noecho();
  io::writeouts("Enter Ctrl-Q to quit\n");
  while ((n = io::readins(input))) {
    if (n == io::fatal) {
      io::writeerrs("fatal: io\n");
      std::terminate();
    }
    if (n == io::again) {
      std::this_thread::yield();
      continue;
    }
    if (*input == 17 /*Ctrl-Q*/)
      break;
    io::writeouts("You typed ");
    io::writeout(input, n);
    io::writeouts("\n");
    // io::writeoutf("You typed %*s\n", n, input);
  }
  io::writeerrs("\nEOF\n");
}
