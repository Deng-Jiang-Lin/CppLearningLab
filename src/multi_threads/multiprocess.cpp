#include <iostream>
#include <unistd.h>

int main() {
  pid_t pid = fork();

  if (pid > 0) {
    std::cout << "parent process\n";
  } else if (pid == 0) {
    std::cout << "child process\n";
  } else {
    perror("fork");
  }
}