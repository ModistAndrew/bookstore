//
// Created by zjx on 2023/12/6.
//

#ifndef BOOKSTORE_DATA_TYPES_HPP
#define BOOKSTORE_DATA_TYPES_HPP

#include <cstring>
#include <map>
#include <variant>
#include "Error.hpp"

template<int L>
class FixedString { // Fixed length string with max length L
  char key[L];
public:
  explicit FixedString(const std::string &s) : key{} {
    if (s.length() > L) {
      throw Error("String too long, this should never happen!");
    }
    strncpy(key, s.c_str(), L);
  }

  FixedString() = default;

  auto operator<=>(const FixedString &rhs) const {
    int result = std::strncmp(key, rhs.key, L);
    return result < 0 ? std::strong_ordering::less :
    result > 0 ? std::strong_ordering::greater : std::strong_ordering::equal;
  }

  bool operator==(const FixedString &rhs) const {
    return std::strncmp(key, rhs.key, L) == 0;
  }

  static FixedString min() {
    FixedString ret;
    memset(ret.key, 0, sizeof(ret.key));
    return ret;
  }

  static FixedString max() {
    FixedString ret;
    memset(ret.key, 0xFF, sizeof(ret.key));
    return ret;
  }

  [[nodiscard]] bool empty() const {
    return key[0] == '\0';
  }
};

typedef FixedString<20> String20;
typedef FixedString<30> String30;
typedef FixedString<60> String60;

enum BookDataID {
  ISBN, NAME, AUTHOR, KEYWORD, PRICE, STOCK
};

enum Privilege {
  GUEST = 0, CUSTOMER = 1, CLERK = 3, ADMIN = 7
};

template<typename T, BookDataID ID>
struct BookDataType {
  T data;
  auto operator<=>(const BookDataType &rhs) const = default;
  bool operator==(const BookDataType &rhs) const = default;
};

typedef BookDataType<String20, ISBN> ISBNType;
typedef BookDataType<String60, NAME> NameType;
typedef BookDataType<String60, AUTHOR> AuthorType;
typedef BookDataType<String60, KEYWORD> KeywordType;
typedef BookDataType<double, PRICE> PriceType;
typedef BookDataType<int, STOCK> StockType;

typedef std::variant<
  ISBNType,
  NameType,
  AuthorType,
  KeywordType> BookDataSearch;
typedef std::variant<
  ISBNType,
  NameType,
  AuthorType,
  KeywordType,
  PriceType> BookDataModify;
typedef std::variant<
  ISBNType,
  NameType,
  AuthorType,
  KeywordType,
  PriceType,
  StockType> BookDataFull;

BookDataSearch parseSearch(const std::string &name, const std::string &value) {
  if (name == "ISBN") {
    return ISBNType{String20(value)};
  }
  if (name == "name") {
    return NameType{String60(value)};
  }
  if (name == "author") {
    return AuthorType{String60(value)};
  }
  if (name == "keyword") {
    return KeywordType{String60(value)};
  }
  throw Error("Invalid book data name");
}
BookDataModify parseModify(const std::string &name, const std::string &value) {
  if (name == "ISBN") {
    return ISBNType{String20(value)};
  }
  if (name == "name") {
    return NameType{String60(value)};
  }
  if (name == "author") {
    return AuthorType{String60(value)};
  }
  if (name == "keyword") {
    return KeywordType{String60(value)};
  }
  if (name == "price") {
    return PriceType{std::stod(value)};
  }
  throw Error("Invalid book data name");
}
#endif //BOOKSTORE_DATA_TYPES_HPP
