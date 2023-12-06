//
// Created by zjx on 2023/12/6.
//

#ifndef BOOKSTORE_DATATYPES_HPP
#define BOOKSTORE_DATATYPES_HPP

#include <string>
#include "../util/Error.hpp"

template<int L>
struct FixedString { // Fixed length string with max length L
  char key[L + 1];

  FixedString(const std::string &s) {
    if (s.length() > L) {
      error("String too long, this should never happen!");
    }
    strcpy(key, s.c_str());
  }
};

struct Integer {
  int value;

  Integer(const std::string &s) {
    value = std::stoi(s);
  }
};

struct Double {
  double value;

  Double(const std::string &s) {
    value = std::stod(s);
  }
};

enum BookDataType {
  ISBN, NAME, AUTHOR, KEYWORD, PRICE, STOCK
};

enum Privilege {
  VISITOR = 0, CUSTOMER = 1, CLERK = 3, MANAGER = 7
};

typedef FixedString<20> String20;
typedef FixedString<30> String30;
typedef FixedString<60> String60;

struct BookData {
  union {
    String20 isbn;
    String60 name;
    String60 author;
    String60 keyword;
    Double price;
    Integer stock;
  };
  BookDataType type;
};
#endif //BOOKSTORE_DATATYPES_HPP
