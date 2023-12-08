//
// Created by zjx on 2023/12/7.
//

#ifndef BOOKSTORE_STATUS_HPP
#define BOOKSTORE_STATUS_HPP
#include "Account.hpp"
#include "Book.hpp"
#include <stack>
struct Status {
  Account account;
  Book book;
};
namespace Statuses {
  namespace {
    std::stack<Status> statusStack;
  }
  int currentPrivilege() {
    return statusStack.top().account.privilege;
  }
  void switchAccount(const Account& account) {
    statusStack.push({account, Book::MIN});
  }
  void logout() {
    statusStack.pop();
  }
  void select(const Book& book) {
    statusStack.top().book = book;
  }
}
#endif //BOOKSTORE_STATUS_HPP
