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
#include "Account.hpp"
#include "Status.hpp"

namespace Commands {
  namespace {
    std::stringstream currentCommand;
    std::map<std::string, std::pair<Privilege, std::function<void()>>> commands;

    void addCommand(const std::string &name, Privilege minPrivilege, const std::function<void()> &func) {
      commands[name] = std::make_pair(minPrivilege, func);
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

    template<>
    Privilege fromString(const std::string &s) {
      switch (stoi(s)) {
        case 0:
          return GUEST;
        case 1:
          return CUSTOMER;
        case 3:
          return CLERK;
        case 7:
          return ADMIN;
        default:
          throw Error("Invalid privilege");
      }
    }

    template<typename T>
    class Scanner {
      std::function<T(const std::string &)> func; //try to cast the string to T. may throw error
    public:
      explicit Scanner(std::function<T(const std::string &)> func) : func(func) {}

      explicit Scanner(const std::regex &pattern) : func([pattern](const std::string &s) -> T {
        if (!std::regex_match(s, pattern)) {
          throw SyntaxError();
        }
        return fromString<T>(s);
      }) {}

      [[nodiscard]] T eval() const {
        std::string s;
        currentCommand >> s;
        if (s.empty()) {
          throw SyntaxError();
        }
        return func(s);
      }

      [[nodiscard]] std::vector<T> evalAll() const {
        std::vector<T> ret;
        std::string s;
        while(currentCommand >> s) {
          if(!s.empty()) {
            ret.emplace_back(func(s));
          }
        }
        return ret;
      }

      [[nodiscard]] std::optional<T> evalOnce() const {
        std::optional<T> ret;
        std::string s;
        currentCommand >> s;
        if (!s.empty()) {
          ret.emplace(func(s));
        }
        return ret;
      }
    };

    const std::string VISIBLE = R"([\x20-\x7E])";
    const std::string AZ = "[a-zA-Z0-9_]";
    const std::string DIGIT = "[0-9]";
    const std::string DIGIT_DOT = "[0-9.]";
    const std::string NO_QUOTIENT = R"([\x20-\x21\x23-\x7E])";

    std::regex merge(const std::string &c, int len) {
      return std::regex("^" + c + "{1," + std::to_string(len) + "}$");
    }

    const Scanner USER_ID = Scanner<String30>(merge(AZ, 30));
    const Scanner PASSWORD = Scanner<String30>(merge(AZ, 30));
    const Scanner USER_NAME = Scanner<String30>(merge(VISIBLE, 30));
    const Scanner PRIVILEGE = Scanner<Privilege>(std::regex("^[0137]$"));
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
    Account checkAccount() {
      Account account = Accounts::get(USER_ID.eval());
      if (account.empty()) {
        throw Error("Invalid user id");
      }
      return account;
    }

    void end() { //you should call it after every arg is extracted but before the command is executed. notice the order!!!
      std::string tmp;
      currentCommand >> tmp;
      if (!tmp.empty()) {
        throw SyntaxError();
      }
    }
  }

  void init() {
    addCommand("exit", GUEST, []() {
      end();
      exit(0);
    });
    addCommand("quit", GUEST, []() {
      end();
      exit(0);
    });
    addCommand("su", GUEST, []() {
      Account account = checkAccount();
      std::optional<String30> password = PASSWORD.evalOnce();
      end();
      if (password == std::nullopt && Statuses::currentPrivilege() != ADMIN) {
        throw PermissionDenied();
      } else if (account.password != password.value()) {
        throw Error("Wrong password");
      }
      Statuses::login(account);
    });
    addCommand("logout", CUSTOMER, []() {
      end();
      if (!Statuses::logout()) {
        throw Error("Not login yet");
      }
    });
    addCommand("register", GUEST, []() {
      String30 userID = USER_ID.eval();
      String30 password = PASSWORD.eval();
      String30 userName = USER_NAME.eval();
      end();
      if (!Accounts::add({userID, password, userName, CUSTOMER})) {
        throw Error("Username already exists");
      }
    });
    addCommand("passwd", CUSTOMER, []() {
      Account account = checkAccount();
      String30 password1 = PASSWORD.eval();
      std::optional<String30> password2 = PASSWORD.evalOnce();
      end();
      //be careful here as the input format is weird
      if (password2 == std::nullopt) {
        if (Statuses::currentPrivilege() != ADMIN) {
          throw PermissionDenied();
        }
        Accounts::modify(account.userID, password1);
      } else {
        if (password1 != account.password) {
          throw Error("Wrong password");
        }
        Accounts::modify(account.userID, password2.value());
      }
    });
    addCommand("useradd", CLERK, []() {
      String30 userID = USER_ID.eval();
      String30 password = PASSWORD.eval();
      Privilege privilege = PRIVILEGE.eval();
      String30 userName = USER_NAME.eval();
      end();
      if (privilege >= Statuses::currentPrivilege()) {
        throw PermissionDenied();
      }
      if (!Accounts::add({userID, password, userName, privilege})) {
        throw Error("Username already exists");
      }
    });
    addCommand("delete", ADMIN, []() {
      Account account = checkAccount();
      end();
      if (Statuses::logged(account)) {
        throw Error("Cannot remove current user");
      }
      Accounts::remove(account.userID);
    });
  }

  void run(const std::string &command) {
    currentCommand = std::stringstream(command);
    //set the currentCommand every time
    std::string name;
    currentCommand >> name;
    if (commands.find(name) == commands.end()) {
      throw Error("Invalid command");
    }
    if (Statuses::currentPrivilege() < commands[name].first) {
      throw PermissionDenied();
    }
    commands[name].second();
  }
}
#endif //BOOKSTORE_COMMAND_HPP