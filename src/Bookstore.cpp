#include "Command.hpp"
#include "PersistentVector.hpp"
#include "Validator.hpp"

int main() {
  Accounts::init();
  Commands::init();
  std::string input;
  std::string label;
  while (getline(std::cin, input)) {
    Account current = Statuses::empty() ? Account::min() : Statuses::top();
    getline(std::cin, label);
    std::cout << label << '\n';
    try {
      Commands::run(input);
      Logs::addLog(current, input);
    } catch (Error &ex) {
      std::cerr << ex.getMessage() << '\n';
    }
  }
}
