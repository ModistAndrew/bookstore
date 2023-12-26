#include "Command.hpp"
#include "PersistentVector.hpp"

int main() {
  Accounts::init();
  Commands::init();
  std::string input;
  std::string label;
  while (getline(std::cin, input)) {
    getline(std::cin, label);
    std::cout << label << '\n';
    try {
      Commands::run(input);
      Logs::addOpLog(Statuses::top(), input);
    } catch (Error &ex) {
      std::cerr << ex.getMessage() << '\n';
    }
  }
}
