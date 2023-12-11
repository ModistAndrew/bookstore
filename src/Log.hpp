//
// Created by zjx on 2023/12/8.
//

#ifndef BOOKSTORE_LOG_HPP
#define BOOKSTORE_LOG_HPP
#include "PersistentVector.hpp"

struct FinanceLog {
  Double income;
  Double outcome;
};
std::ostream &operator<<(std::ostream &out, const FinanceLog &log) {
  return out << "+ " << log.income << " - " << log.outcome;
}
PersistentVector<FinanceLog> financeLog("finance");

namespace Logs {
  void addFinanceLog(Double income, Double outcome) {
    financeLog.push_back({income, outcome});
  }

  void printFinanceLog(int cnt) {
    if(cnt==0) {
      std::cout << std::endl;
      return;
    }
    FinanceLog total{};
    financeLog.iterate(cnt, [&total](const FinanceLog &log) {
      total.income += log.income;
      total.outcome += log.outcome;
    });
    std::cout << total << std::endl;
  }
}
#endif //BOOKSTORE_LOG_HPP
