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

namespace {
  typedef std::function<void()> Runnable;
  std::stringstream currentCommand;
  struct Command {
    std::string name;
    Privilege minPrivilege = GUEST;
    Runnable getArgs;
    Runnable execute;
  };

  std::map<std::string, Command> commands;

  void addCommand(const std::string &name, Privilege minPrivilege, const Runnable &getArgs, const Runnable &execute) {
    commands[name] = {name, minPrivilege, getArgs, execute};
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

    [[nodiscard]] virtual std::optional<T> evalOnce() const {
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
      char c;
      currentCommand >> c; //skip the '-'
      std::getline(currentCommand, s, '=');
      if (s.empty()) {
        throw SyntaxError();
      }
      return this->toT(s);
    }

    [[nodiscard]] std::optional<T> evalOnce() const override {
      std::optional<T> ret;
      std::string s;
      char c;
      currentCommand >> c; //skip the '-'
      std::getline(currentCommand, s, '=');
      if (!s.empty()) {
        ret.emplace(this->toT(s));
      }
      return ret;
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
  const ArgScanner BOOK_DATA = ArgScanner<BookDataID>(BOOK_DATA_PATTERN);
  //notice the order!!!

  Account checkAccount() {
    Account account = Accounts::get(USER_ID.eval());
    if (account.empty()) {
      throw Error("Invalid user id");
    }
    return account;
  }

  void end() { //you should call it after every arg is extracted but before the command is executed.
    std::string tmp;
    currentCommand >> tmp;
    if (!tmp.empty()) {
      throw SyntaxError();
    }
  }
}

namespace Commands {

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
      std::optional<BookDataID> type = BOOK_DATA.evalOnce();
      if (type == std::nullopt) {
        Books::isbnMap.iterateAll(String20::min(), String20::max(), [](const Book &b) {
          std::cout << b << std::endl;
        }, []() { std::cout << std::endl; });
      } else
        switch (type.value()) {
        case ISBN_TYPE:
            Books::isbnMap.iterate(ISBN_TYPE.eval(), [](const Book &b) {
              std::cout << b << std::endl;
            }, []() { std::cout << std::endl; });
            break;
          case NAME_SEARCH:
            Books::nameMap.iterate(NAME_TYPE.eval(), [](const Book &b) {
              std::cout << b << std::endl;
            }, []() { std::cout << std::endl; });
            break;
          case AUTHOR_SEARCH:
            Books::authorMap.iterate(AUTHOR_TYPE.eval(), [](const Book &b) {
              std::cout << b << std::endl;
            }, []() { std::cout << std::endl; });
            break;
          case KEYWORD_SEARCH:
            String60 keyword = KEYWORD_TYPE.eval();
            if (keyword.split().size() != 1) {
              throw Error("Search keyword should be single");
            }
            Books::keywordMap.iterate(keyword, [](const Book &b) {
              std::cout << b << std::endl;
            }, []() { std::cout << std::endl; });
            break;
        }
      end();
    });
    addCommand("buy", CUSTOMER, []() {
      String20 isbn = ISBN_TYPE.eval();
      int count = COUNT.eval();
      end();
      auto book = Books::getBook(isbn);
      if (book.empty()) {
        throw Error("Invalid ISBN_TYPE");
      }
      if (book.stock < count) {
        throw Error("Not enough stock");
      }
      book.stock -= count;
      book.store = true; //TODO: store the price in log and print total price
    });
    addCommand("select", CLERK, []() {
      Books::select(ISBN_TYPE.eval());
      end();
    });
    addCommand("modify", CLERK, []() {
      std::optional<Books::PersistentBook>& book = Books::currentBook; //use reference to avoid copy
      if (book == std::nullopt) {
        throw Error("No book is selected");
      }
      bool present[5]{};
      bool empty = true;
      while (true) {
        std::optional<BookDataModifyID> type = BOOK_DATA_MODIFY.evalOnce();
        if (type == std::nullopt) {
          if (empty) {
            throw Error("No modification");
          }
          break;
        }
        empty = false;
        if (present[type.value()]) {
          throw Error("Duplicate modify");
        }
        present[type.value()] = true;
        switch (type.value()) {
          case ISBN_MODIFY: //TODO: what if ISBN_TYPE is modified?
            book->isbn = ISBN_TYPE.eval();
            break;
          case NAME_MODIFY:
            book->name = NAME_TYPE.eval();
            break;
          case AUTHOR_MODIFY:
            book->author = AUTHOR_TYPE.eval();
            break;
          case PRICE_MODIFY:
            book->price = PRICE_TYPE.eval();
            break;
          case KEYWORD_MODIFY:
            String60 keyword = KEYWORD_TYPE.eval();
            keyword.split(); //just to check if the keyword is valid
            book->keyword = keyword;
            break;
        }
      }
      end();
    });
    addCommand("import", CLERK, []() {
      std::optional<Books::PersistentBook>& book = Books::currentBook;
      if (book == std::nullopt) {
        throw Error("No book is selected");
      }
      int count = COUNT.eval();
      double totalCost = PRICE_TYPE.eval(); //TODO: store the price in log
      end();
      book->stock += count;
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
    commands[name].getArgs();
    commands[name].execute();
  }

}
#endif //BOOKSTORE_COMMAND_HPP