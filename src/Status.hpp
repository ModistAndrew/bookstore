//
// Created by zjx on 2023/12/7.
//

#ifndef BOOKSTORE_STATUS_HPP
#define BOOKSTORE_STATUS_HPP

#include "Account.hpp"
#include "Book.hpp"
#include <stack>
#include <set>

struct Status {
  Account account;
  Book book;
};
namespace Statuses {
  namespace {
    std::stack<Status> statusStack;
    std::multiset<String30> loggedAccounts; //corresponding to statusStack
    Account &top() {
      return statusStack.top().account;
    }
    bool empty() {
      return statusStack.empty();
    }
  }

  Privilege currentPrivilege() {
    return empty() ? GUEST : top().privilege;
  }

  void login(const Account &account) {
    statusStack.push({account, Book::min()});
    loggedAccounts.insert(account.userID);
  }

  bool logout() {
    if (statusStack.empty()) {
      return false;
    }
    loggedAccounts.erase(loggedAccounts.find(top().userID));
    statusStack.pop();
    return true;
  }

  void select(const Book &book) {
    statusStack.top().book = book;
  }

  bool logged(const Account &account) {
    return loggedAccounts.count(account.userID) > 0;
  }
}
#endif //BOOKSTORE_STATUS_HPP
