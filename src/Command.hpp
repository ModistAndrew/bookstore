//
// Created by zjx on 2023/12/6.
//

#ifndef BOOKSTORE_COMMAND_HPP
#define BOOKSTORE_COMMAND_HPP

#include <string>
#include <sstream>
#include <regex>
#include <functional>
#include <optional>
#include "Error.hpp"
#include "DataTypes.hpp"

namespace Commands {
  namespace {
    std::stringstream currentCommand;
    std::map<std::string, std::function<void()>> commands;

    void addCommand(const std::string &name, const std::function<void()> &func) {
      commands[name] = func;
    }

    template<typename T>
    T fromString(const std::string &s) {
      return T(s);
    }

    template<>
    int fromString(const std::string &s) {
      return stoi(s);
    }

    template<>
    double fromString(const std::string &s) {
      return stod(s);
    }

    template<typename T>
    class Scanner {
      std::function<T(const std::string &)> func; //try to cast the string to T. may throw error
    public:
      Scanner(std::function<T(const std::string &)> func) : func(func) {}

      Scanner(const std::regex &pattern) : func([pattern](const std::string &s) -> T {
        if (!std::regex_match(s, pattern)) {
          error("Invalid input");
        }
        return fromString<T>(s);
      }) {}

      T eval() const {
        std::string s;
        currentCommand >> s;
        return func(s);
      }

      std::vector<T> evalAll() const {
        std::vector<T> ret;
        std::string s;
        while (currentCommand >> s) {
          ret.push_back(func(s));
        }
        return ret;
      }

      std::optional<T> evalOnce() const {
        std::optional<T> ret;
        std::string s;
        if (currentCommand >> s) {
          ret.emplace(func(s));
        }
        return ret;
      }
    };

    const std::string VISIBLE = "[\\x20-\\x7E]";
    const std::string AZ = "[a-zA-Z0-9_]";
    const std::string DIGIT = "[0-9]";
    const std::string DIGIT_DOT = "[0-9.]";
    const std::string NO_QUOTIENT = "[\\x20-\\x21\\x23-\\x7E]";

    std::regex merge(std::string c, int len) {
      return std::regex("^" + c + "{1," + std::to_string(len) + "}$");
    }

    const Scanner USER_ID = Scanner<String30>(merge(AZ, 30));
    const Scanner PASSWORD = Scanner<String30>(merge(AZ, 30));
    const Scanner USER_NAME = Scanner<String30>(merge(VISIBLE, 30));
    const Scanner PRIVILEGE = Scanner<int>(merge(DIGIT, 1));
    const Scanner ISBN = Scanner<String20>(merge(VISIBLE, 20));
    const Scanner BOOK_NAME = Scanner<String60>(merge(NO_QUOTIENT, 60));
    const Scanner AUTHOR = Scanner<String60>(merge(NO_QUOTIENT, 60));
    const Scanner KEYWORD = Scanner<String60>(merge(NO_QUOTIENT, 60));
    const Scanner COUNT = Scanner<int>(merge(DIGIT, 10));
    const Scanner PRICE = Scanner<double>(merge(DIGIT_DOT, 13));
//    const Scanner BOOK_DATA = Scanner<BookData>([](const std::string &s) -> BookData {
//      std::stringstream ss(s);
//      std::string name, value;
//      std::getline(ss, name, '=');
//      ss >> value;
//      return parse(name, value);
//    });
  }

  void init() {
    addCommand("su", []() {
      String30 id = USER_ID.eval();
      std::optional<String30> password = PASSWORD.evalOnce();
    });
  }

  void run(const std::string &command) {
    currentCommand = std::stringstream(command);
    std::string name;
    currentCommand >> name;
    if (commands.find(name) == commands.end()) {
      error("Invalid command");
    }
    commands[name]();
    //check if there is any redundant input
    currentCommand >> name;
    if (!name.empty()) {
      error("Invalid command");
    }
  }
}
#endif //BOOKSTORE_COMMAND_HPP