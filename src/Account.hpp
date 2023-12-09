//
// Created by zjx on 2023/12/6.
//
#ifndef BOOKSTORE_ACCOUNT_HPP
#define BOOKSTORE_ACCOUNT_HPP

#include "Error.hpp"
#include "Utils.hpp"
#include "PersistentSet.hpp"

struct Account {
  String30 userID;
  String30 password;
  String30 userName;
  Privilege privilege;

  auto operator<=>(const Account &rhs) const {
    return userID <=> rhs.userID;
  }

  bool operator==(const Account &rhs) const {
    return userID == rhs.userID;
  }

  static const Account MIN;
  static const Account MAX;

  [[nodiscard]] bool empty() const {
    return userID.empty();
  }
};

const Account Account::MIN = Account{String30::min()};
const Account Account::MAX = Account{String30::max()};
namespace Accounts {
  namespace {
    PersistentMap<String30, Account, 100> accountMap(false, "accounts");
  }

  bool add(const Account &account) {
    return accountMap.put(account.userID, account);
  }

  Account get(const String30 &userID) {
    return accountMap.get(userID);
  }

  bool modify(const String30 &userID, const String30 &password) {
    Account account = get(userID);
    if (account.empty()) {
      return false;
    }
    account.password = password;
    accountMap.remove(userID);
    return accountMap.put(userID, account);
  }

  bool remove(const String30 &userID) {
    return accountMap.remove(userID);
  }

  void init() {
    add(Account{String30{"root"}, String30{"sjtu"}, String30{"root"}, ADMIN}); //try to store root if not exists
  }
}
#endif //BOOKSTORE_ACCOUNT_HPP
