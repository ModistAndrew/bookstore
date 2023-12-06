//
// Created by zjx on 2023/12/6.
//

#ifndef BOOKSTORE_COMMAND_HPP
#define BOOKSTORE_COMMAND_HPP

#include <string>
#include <sstream>
#include <regex>
#include <functional>
#include "../util/Error.hpp"
#include "../database/DataTypes.hpp"

template<typename T>
class Parser {
  const std::regex reg;

public:
  Parser(const std::string& pattern) : reg(std::regex(pattern)) {}

  T get(std::stringstream& ss) const {
    std::string s;
    ss >> s;
    if(!std::regex_match(s, reg)) {
      error("Invalid input");
    }
    return T(s);
  }
};

namespace Parsers {
  const std::string VISIBLE = "[\\x20-\\x7E]";
  const std::string AZ = "[a-zA-Z0-9_]";
  const std::string DIGIT = "[0-9]";
  const std::string DIGIT_DOT = "[0-9.]";
  const std::string NO_QUOTIENT = "[\\x20-\\x21\\x23-\\x7E]";
  std::string merge(std::string c, int len) {
    return "^" + c + "{1," + std::to_string(len) + "}$";
  }
  const Parser USER_ID = Parser<String30>(merge(AZ, 30));
  const Parser PASSWORD = Parser<String30>(merge(AZ, 30));
  const Parser USER_NAME = Parser<String30>(merge(VISIBLE, 30));
  const Parser PRIVILEGE = Parser<Integer>(merge(DIGIT, 1));
  const Parser ISBN = Parser<String20>(merge(VISIBLE, 20));
  const Parser BOOK_NAME = Parser<String60>(merge(NO_QUOTIENT, 60));
  const Parser AUTHOR = Parser<String60>(merge(NO_QUOTIENT, 60));
  const Parser KEYWORD = Parser<String60>(merge(NO_QUOTIENT, 60));
  const Parser COUNT = Parser<Integer>(merge(DIGIT, 10));
  const Parser PRICE = Parser<Double>(merge(DIGIT_DOT, 13));
}

class Command {
  const std::string name;
  const std::function<void(std::stringstream&)> func;
public:
  Command(std::string _name, std::function<void(std::stringstream&)> _func) : name(_name), func(_func) {}

  void execute(std::stringstream& ss) {
    func(ss);
  }
};

namespace Commands {
  const Command USER_ADD = Command("su", [](std::stringstream& ss) {
    String30 ID = Parsers::USER_ID.get(ss);
    std::string password = Parsers::PASSWORD.get(ss);
    std::string userName = Parsers::USER_NAME.get(ss);
    int privilege = Parsers::PRIVILEGE.get(ss).value;
    if(privilege < 0 || privilege > 7) {
      error("Invalid privilege");
    }
    if(!ss.eof()) {
      error("Invalid input");
    }
}
#endif //BOOKSTORE_COMMAND_HPP
