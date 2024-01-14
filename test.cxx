#include "io.hxx"
#include <exception>
#include <thread>
int main() {
  char input[4];
  size_t n;
  if (io::setin_nonblocking() != io::ok)
    io::writeerrs("error: nb\n");
  if (io::setin_cbreak() != io::ok)
    io::writeerrs("error: cb\n");
  if (io::setin_noecho() != io::ok)
    io::writeerrs("error: ne\n");
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
    // io::writeouts("You typed ");
    // io::writeout(input, n);
    // io::writeouts("\n");
    io::writeoutf("You typed %.*s\n", n, input);
  }
  io::writeerrs("\nEOF\n");
}
