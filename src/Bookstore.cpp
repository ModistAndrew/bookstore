#include "Command.hpp"

int main() {
  Accounts::init();
  Commands::init();
  while (true) {
    try {
      std::string input;
      getline(std::cin, input);
      Commands::run(input);
    } catch (Error &ex) {
      std::cout << ex.getMessage() << std::endl;
    }
  }
}
