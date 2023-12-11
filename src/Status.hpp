//
// Created by zjx on 2023/12/7.
//

#ifndef BOOKSTORE_STATUS_HPP
#define BOOKSTORE_STATUS_HPP

#include "Account.hpp"
#include "Book.hpp"
#include <vector>
#include <set>

struct Status {
  Account account;
  String20 currentISBN;
};

namespace Statuses {

  std::vector<Status> statusStack;
  std::multiset<String30> loggedAccounts; //corresponding to statusStack
  Account &top() {
    return statusStack.back().account;
  }

  bool empty() {
    return statusStack.empty();
  }

  String20 &currentISBN() {
    return statusStack.back().currentISBN;
  }

  Privilege currentPrivilege() {
    return empty() ? GUEST : top().privilege;
  }

  void login(const Account &account) {
    statusStack.push_back({account, String20::min()});
    loggedAccounts.insert(account.userID);
  }

  bool logout() {
    if (statusStack.empty()) {
      return false;
    }
    loggedAccounts.erase(loggedAccounts.find(top().userID));
    statusStack.pop_back();
    return true;
  }

  void select(const String20 &isbn) {
    currentISBN() = isbn;
  }

  bool logged(const Account &account) {
    return loggedAccounts.count(account.userID) > 0;
  }

  void remapISBN(const String20 &from, const String20 &to) {
    if(from.empty() || to.empty()) {
      throw Error("ISBN cannot be empty when remapping");
    }
    for(Status &s: statusStack) {
      if(s.currentISBN == from) {
        s.currentISBN = to;
      }
    }
  }
}
#endif //BOOKSTORE_STATUS_HPP
