//
// Created by zjx on 2023/12/8.
//

#ifndef BOOKSTORE_LOG_HPP
#define BOOKSTORE_LOG_HPP

#include "PersistentVector.hpp"

struct FinanceLog {
  Double income;
  Double outcome;

  friend std::ostream &operator<<(std::ostream &out, const FinanceLog &log) {
    return out << "+ " << log.income << " - " << log.outcome;
  }
};

struct OpLog {
  Account account;
  String60 op;

  friend std::ostream &operator<<(std::ostream &out, const OpLog &log) {
    return out << log.account.userID << ": " << log.op;
  }
};

PersistentVector<FinanceLog> financeLog("finance");
PersistentVector<OpLog> opLog("op");

namespace Logs {
  void addFinanceLog(Double income, Double outcome) {
    financeLog.push_back({income, outcome});
  }

  void printFinanceLog(int cnt) {
    if (cnt == 0) {
      std::cout << '\n';
      return;
    }
    FinanceLog total{};
    financeLog.iterate(cnt, [&total](const FinanceLog &log) {
      total.income += log.income;
      total.outcome += log.outcome;
    });
    std::cout << total << '\n';
  }

  void reportFinance() {
    financeLog.iterate(-1, [](const FinanceLog &log) {
      std::cout << log << '\n';
    });
  }

  void addOpLog(const Account &account, const std::string &op) {
    opLog.push_back({account, String60(shorten(op, 60))});
  }

  void reportClerk() {
    opLog.iterate(-1, [](const OpLog &log) {
      if (log.account.privilege >= CLERK) {
        std::cout << log << '\n';
      }
    });
  }

  void reportOp() {
    opLog.iterate(-1, [](const OpLog &log) {
      std::cout << log << '\n';
    });
  }
}
#endif //BOOKSTORE_LOG_HPP
