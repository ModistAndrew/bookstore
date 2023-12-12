#include "Command.hpp"
#include "PersistentVector.hpp"

int main() {
  Accounts::init();
  Commands::init();
  std::string input;
  while (getline(std::cin, input)) {
    try {
      Commands::run(input);
    } catch (Error &ex) {
      std::cout << "Invalid" << '\n';
    }
  }
}
