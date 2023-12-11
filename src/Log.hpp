//
// Created by zjx on 2023/12/8.
//

#ifndef BOOKSTORE_LOG_HPP
#define BOOKSTORE_LOG_HPP
#include "PersistentVector.hpp"

struct FinanceLog {
  double income;
  double outcome;
};
std::ostream &operator<<(std::ostream &out, const FinanceLog &log) {
  return out << "+ " << std::setiosflags(std::ios::fixed) << std::setprecision(2) << log.income << " - "
    << std::setiosflags(std::ios::fixed) << std::setprecision(2) << log.outcome;
}
PersistentVector<FinanceLog> financeLog("finance");

namespace Logs {
  void addFinanceLog(double income, double outcome) {
    financeLog.push_back({income, outcome});
  }

  void printFinanceLog(int cnt) {
    FinanceLog total{};
    financeLog.iterate(cnt, [&total](const FinanceLog &log) {
      total.income += log.income;
      total.outcome += log.outcome;
    });
    std::cout << total << std::endl;
  }
}
#endif //BOOKSTORE_LOG_HPP
