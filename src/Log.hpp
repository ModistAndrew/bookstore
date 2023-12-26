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

PersistentVector<FinanceLog> financeLog("finance");
PersistentVector<String300> employeeLog("employee");
PersistentVector<String300> fullLog("full");

namespace Logs {

  void addFinanceLog(Double income, Double outcome) {
    FinanceLog f{income, outcome};
    financeLog.push_back(f);
    std::stringstream ss;
    ss << f;
    fullLog.push_back(String300(ss.str()));
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
    financeLog.iterateFromBegin([](const FinanceLog &log) {
      std::cout << log << '\n';
    });
  }

  void addLog(const Account &account, const std::string &op) {
    std::stringstream ss;
    ss << account << ": " << op;
    if(account.privilege >= CLERK) {
      employeeLog.push_back(String300(ss.str()));
    }
    fullLog.push_back(String300(ss.str()));
  }

  void reportEmployee() {
    employeeLog.iterateFromBegin([](const String300 &log) {
      std::cout << log << '\n';
    });
  }

  void reportFull() {
    fullLog.iterateFromBegin([](const String300 &log) {
      std::cout << log << '\n';
    });
  }
}
#endif //BOOKSTORE_LOG_HPP
