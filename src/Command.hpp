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
#include "Log.hpp"
#include "StringReader.hpp"

namespace {
  typedef std::function<void()> Runnable;
  StringReader currentCommand;
  struct Command {
    std::string name;
    Privilege minPrivilege = GUEST;
    Runnable getArgs;
    Runnable execute;
  }; //always check before execute

  std::map<std::string, Command> commands;

  void addCommand(const std::string &name, Privilege minPrivilege, const Runnable &getArgs, const Runnable &execute) {
    commands[name] = {name, minPrivilege, getArgs, execute};
  }

  template<typename T>
  class Scanner { //needn't reset if you just visit those evaluated
    std::regex reg;
  public:
    std::optional<T> value; //init with empty

    explicit Scanner(std::regex pattern) : reg(std::move(pattern)) {}

    T toT(const std::string &s) {
      std::smatch match;
      if (!std::regex_match(s, match, reg)) {
        throw SyntaxError();
      }
      return fromString<T>(match[1]);
    }

    void require() {
      std::string s = currentCommand.get();
      if (s.empty()) {
        throw SyntaxError();
      }
      value.emplace(toT(s));
    }

    void optional() {
      std::string s = currentCommand.get();
      if (!s.empty()) {
        value.emplace(toT(s));
      } else {
        value = std::nullopt; //reset
      }
    }

    [[nodiscard]] T get() const {
      if (value == std::nullopt) {
        throw Error("No value");
      }
      return value.value();
    }

    [[nodiscard]] bool present() const {
      return value != std::nullopt;
    }
  };

  Scanner USER_ID = Scanner<String30>(USER_ID_PATTERN);
  Scanner PASSWORD = Scanner<String30>(PASSWORD_PATTERN);
  Scanner PASSWORD2 = Scanner<String30>(PASSWORD_PATTERN);
  Scanner USER_NAME = Scanner<String30>(USER_NAME_PATTERN);
  Scanner PRIVILEGE = Scanner<Privilege>(PRIVILEGE_PATTERN);
  Scanner ISBN = Scanner<String20>(ISBN_PATTERN);
  Scanner NAME = Scanner<String60Chinese>(NAME_PATTERN);
  Scanner AUTHOR = Scanner<String60Chinese>(AUTHOR_PATTERN);
  Scanner KEYWORD = Scanner<String60Chinese>(KEYWORD_PATTERN);
  Scanner COUNT = Scanner<int>(COUNT_PATTERN);
  Scanner PRICE = Scanner<Double>(PRICE_PATTERN);
  std::set<BookDataID> BOOK_DATA_IDS; //similar to Scanner, call scanArgs() to assign value

  void scanBookArgs(bool search) { //search is used to check
    BOOK_DATA_IDS.clear();
    std::string s;
    BookDataID type;
    while (!currentCommand.empty()) {
      s = currentCommand.get();
      std::smatch result;
      if(!std::regex_match(s, result, ARGS_PATTERN)) {
        throw SyntaxError();
      }
      type = fromString<BookDataID>(result[1]);
      if (!BOOK_DATA_IDS.insert(type).second) {
        throw Error("Duplicate argument");
      }
      currentCommand.store(result[2]);
      switch (type) {
        case ISBN_TYPE:
          ISBN.require();
          if (!search && !Books::isbnMap.get(ISBN.get()).empty()) {
            throw Error("ISBN already exists");
          }
          break;
        case NAME_TYPE:
          NAME.require();
          break;
        case AUTHOR_TYPE:
          AUTHOR.require();
          break;
        case PRICE_TYPE:
          if (search) {
            throw Error("Cannot search by price");
          }
          PRICE.require();
          break;
        case KEYWORD_TYPE:
          KEYWORD.require();
          auto v = KEYWORD.get().split(); //check valid
          if (search && v.size() > 1) {
            throw Error("Too many keywords for search");
          }
          break;
      }
    }
    if (search && BOOK_DATA_IDS.size() > 1) {
      throw Error("Too many arguments for search");
    }
    if (!search && BOOK_DATA_IDS.empty()) {
      throw Error("No argument for modify");
    }
  }
}

namespace Commands {

  void init() {
    addCommand("exit", GUEST, []() {}, []() {
      exit(0);
    });
    addCommand("quit", GUEST, []() {}, []() {
      exit(0);
    });
    addCommand("su", GUEST, []() {
      USER_ID.require();
      PASSWORD.optional();
    }, []() {
      Account account = Accounts::require(USER_ID.get());
      if (!PASSWORD.present()) {
        if (Statuses::currentPrivilege() <= account.privilege) {
          throw PermissionDenied();
        }
        Statuses::login(account);
      } else {
        if (account.password != PASSWORD.get()) {
          throw Error("Wrong password");
        }
        Statuses::login(account);
      }
    });
    addCommand("logout", CUSTOMER, []() {},
               []() {
                 if (!Statuses::logout()) {
                   throw Error("Not login yet");
                 }
               }
    );
    addCommand("register", GUEST, []() {
      USER_ID.require();
      PASSWORD.require();
      USER_NAME.require();
    }, []() {
      if (!Accounts::add({USER_ID.get(), PASSWORD.get(), USER_NAME.get(), CUSTOMER})) {
        throw Error("User Id already exists");
      }
    });
    addCommand("passwd", CUSTOMER, []() {
      USER_ID.require();
      PASSWORD.require();
      PASSWORD2.optional();
    }, []() {
      Account account = Accounts::require(USER_ID.get());
      if (!PASSWORD2.present()) {
        if (Statuses::currentPrivilege() != ADMIN) {
          throw PermissionDenied();
        }
        Accounts::modify(account.userID, PASSWORD.get());
      } else {
        if (PASSWORD.get() != account.password) {
          throw Error("Wrong password");
        }
        Accounts::modify(account.userID, PASSWORD2.get());
      }
    });
    addCommand("useradd", CLERK, []() {
      USER_ID.require();
      PASSWORD.require();
      PRIVILEGE.require();
      USER_NAME.require();
    }, []() {
      if (PRIVILEGE.get() >= Statuses::currentPrivilege()) {
        throw PermissionDenied();
      }
      if (!Accounts::add({USER_ID.get(), PASSWORD.get(), USER_NAME.get(), PRIVILEGE.get()})) {
        throw Error("User Id already exists");
      }
    });
    addCommand("delete", ADMIN, []() {
      USER_ID.require();
    }, []() {
      Account account = Accounts::require(USER_ID.get());
      if (Statuses::logged(account)) {
        throw Error("Cannot remove current user");
      }
      Accounts::remove(account.userID);
    });
    addCommand("show", CUSTOMER, []() {
      scanBookArgs(true);
    }, []() {
      if (BOOK_DATA_IDS.empty()) {
        Books::isbnMap.printAll(String20::min(), String20::max());
        return;
      }
      BookDataID id = *BOOK_DATA_IDS.begin(); //the only one
      switch (id) {
        case ISBN_TYPE:
          Books::isbnMap.print(ISBN.get());
          break;
        case NAME_TYPE:
          Books::nameMap.print(NAME.get());
          break;
        case AUTHOR_TYPE:
          Books::authorMap.print(AUTHOR.get());
          break;
        case KEYWORD_TYPE:
          Books::keywordMap.print(KEYWORD.get());
          break;
        case PRICE_TYPE:
          throw Error("Cannot search by price, this should not happen!");
      }
    });
    addCommand("buy", CUSTOMER, []() {
      ISBN.require();
      COUNT.require();
    }, []() {
      auto book = Books::extract(ISBN.get());
      book.save = true;
      if (book.stock < COUNT.get()) {
        throw Error("Not enough stock");
      }
      if(COUNT.get() <= 0) {
        throw Error("Invalid count");
      }
      Double cost = book.price * COUNT.get();
      book.stock -= COUNT.get();
      std::cout << cost << '\n';
      Logs::addFinanceLog(cost, Double::min());
    });
    addCommand("select", CLERK, []() {
      ISBN.require();
    }, []() {
      String20 isbn = ISBN.get();
      Book tmp = Books::isbnMap.get(isbn);
      if (tmp.empty()) {
        tmp.isbn = isbn; //may be empty, create a new one and store it
        Books::store(tmp);
      }
      Statuses::select(isbn);
    });
    addCommand("modify", CLERK, []() {
      scanBookArgs(false);
    }, []() {
      auto book = Books::extract(Statuses::currentISBN());
      book.save = true;
      for (BookDataID id: BOOK_DATA_IDS) {
        switch (id) {
          case ISBN_TYPE:
            Statuses::remapISBN(book.isbn, ISBN.get());
            book.isbn = ISBN.get();
            break;
          case NAME_TYPE:
            book.name = NAME.get();
            break;
          case AUTHOR_TYPE:
            book.author = AUTHOR.get();
            break;
          case KEYWORD_TYPE:
            book.keyword = KEYWORD.get();
            break;
          case PRICE_TYPE:
            book.price = PRICE.get();
            break;
        }
      }
    });
    addCommand("import", CLERK, []() {
      COUNT.require();
      PRICE.require();
    }, []() {
      if(COUNT.get() <= 0) {
        throw Error("Count must be positive");
      }
      if(PRICE.get() <= Double::min()) {
        throw Error("Price must be positive");
      }
      auto book = Books::extract(Statuses::currentISBN());
      book.save = true;
      book.stock += COUNT.get();
      Logs::addFinanceLog(Double::min(), PRICE.get());
    });
    addCommand("show finance", ADMIN, []() {
      COUNT.optional();
    }, []() {
      Logs::printFinanceLog(COUNT.present() ? COUNT.get() : -1);
    });
    addCommand("report finance", ADMIN, []() {}, []() {
      Logs::reportFinance();
    });
    addCommand("report employee", ADMIN, []() {}, []() {
      Logs::reportEmployee();
    });
    addCommand("log", ADMIN, []() {}, []() {
      Logs::reportFull();
    });
    addCommand("addBook", CLERK, []() {
      ISBN.require();
    }, []() {
      String20 isbn = ISBN.get();
      Book tmp = Books::isbnMap.get(isbn);
      if (tmp.empty()) {
        tmp.isbn = isbn; //create a new one and store it
        Books::store(tmp);
      } else {
        throw Error("ISBN already exists");
      }
    });
    addCommand("showUser", GUEST, []() {}, []() {
      Statuses::printAccounts();
    });
  }

  void run(const std::string &command) {
    currentCommand = StringReader(command); //set the currentCommand every time
    if(currentCommand.empty()){
      return; //skip empty line
    }
    std::string name = currentCommand.get();
    std::string name2 = currentCommand.touch();
    std::string newName = name + " " + name2;
    if(commands.find(newName) != commands.end()) { //first try to find the command with two words
      currentCommand.get(); //skip the second word
      name = newName; //set the name
    }
    if(commands.find(name) == commands.end()) {
      throw Error("Invalid command");
    }
    const Command &cmd = commands[name];
    if (Statuses::currentPrivilege() < cmd.minPrivilege) {
      throw PermissionDenied();
    }
    cmd.getArgs();
    if (!currentCommand.empty()) {
      throw SyntaxError(); //check whether there are redundant arguments
    }
    cmd.execute();
  }
}
#endif //BOOKSTORE_COMMAND_HPP