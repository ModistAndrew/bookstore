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
#include <utility>
#include "Error.hpp"
#include "Utils.hpp"
#include "Account.hpp"
#include "Status.hpp"

namespace Commands {
  namespace {
    std::stringstream currentCommand;
    struct Command {
      std::string name;
      Privilege minPrivilege = GUEST;
      std::function<void()> runnable;
    };

    std::map<std::string, Command> commands;

    void addCommand(const std::string &name, Privilege minPrivilege, const std::function<void()> &func) {
      commands[name] = {name, minPrivilege, func};
    }

    template<typename T>
    class Scanner {
    protected:
      std::function<T(const std::string &)> toT; //try to cast the string to T. may throw error
    public:
      explicit Scanner(std::function<T(const std::string &)> func) : toT(std::move(func)) {}

      explicit Scanner(const std::regex &pattern) : toT([pattern](const std::string &s) -> T {
        std::smatch match;
        if (!std::regex_match(s, match, pattern)) {
          throw SyntaxError();
        }
        return fromString<T>(match[1]);
      }) {}

      [[nodiscard]] virtual T eval() const {
        std::string s;
        currentCommand >> s;
        if (s.empty()) {
          throw SyntaxError();
        }
        return toT(s);
      }

      [[nodiscard]] std::vector<T> evalAll() const {
        std::vector<T> ret;
        std::string s;
        while (currentCommand >> s) {
          if (!s.empty()) {
            ret.emplace_back(toT(s));
          }
        }
        return ret;
      }

      [[nodiscard]] std::optional<T> evalOnce() const {
        std::optional<T> ret;
        std::string s;
        currentCommand >> s;
        if (!s.empty()) {
          ret.emplace(toT(s));
        }
        return ret;
      }
    };

    template<typename T>
    class ArgScanner : public Scanner<T> {
    public:
      explicit ArgScanner(const std::regex &pattern) : Scanner<T>(pattern) {}
      [[nodiscard]] T eval() const override {
        std::string s;
        std::getline(currentCommand, s, '=');
        if (s.empty()) {
          throw SyntaxError();
        }
        return this->toT(s);
      }
    };

    const Scanner USER_ID = Scanner<String30>(USER_ID_PATTERN);
    const Scanner PASSWORD = Scanner<String30>(PASSWORD_PATTERN);
    const Scanner USER_NAME = Scanner<String30>(USER_NAME_PATTERN);
    const Scanner PRIVILEGE = Scanner<Privilege>(PRIVILEGE_PATTERN);
    const Scanner ISBN = Scanner<String20>(ISBN_PATTERN);
    const Scanner NAME = Scanner<String60>(NAME_PATTERN);
    const Scanner AUTHOR = Scanner<String60>(AUTHOR_PATTERN);
    const Scanner KEYWORD = Scanner<String60>(KEYWORD_PATTERN);
    const Scanner COUNT = Scanner<int>(COUNT_PATTERN);
    const Scanner PRICE = Scanner<double>(PRICE_PATTERN);
    const ArgScanner BOOK_DATA_SEARCH = ArgScanner<BookDataSearchID>(BOOK_DATA_SEARCH_PATTERN);
    const ArgScanner BOOK_DATA_MODIFY = ArgScanner<BookDataModifyID>(BOOK_DATA_MODIFY_PATTERN);

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
    addCommand("show", CUSTOMER, []() {
      end();
      std::cout << account.userID << " " << account.userName << " " << account.privilege << std::endl;
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
    if (Statuses::currentPrivilege() < commands[name].minPrivilege) {
      throw PermissionDenied();
    }
    commands[name].runnable();
  }
}
#endif //BOOKSTORE_COMMAND_HPP