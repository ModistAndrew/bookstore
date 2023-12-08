//
// Created by zjx on 2023/12/6.
//

#ifndef BOOKSTORE_DATA_TYPES_HPP
#define BOOKSTORE_DATA_TYPES_HPP

#include <cstring>
#include <map>
#include <variant>
#include "Error.hpp"
#include "StringUtil.hpp"

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

  friend std::ostream &operator<<(std::ostream &out, const FixedString<L> &rhs);

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
template<int L>
std::ostream &operator<<(std::ostream &out, const FixedString<L> &rhs) {
  for(int i = 0; i < L; i++) {
    if(rhs.key[i]=='\0') {
      break;
    }
    out << rhs.key[i];
  }
  return out;
}

typedef FixedString<20> String20;
typedef FixedString<30> String30;
typedef FixedString<60> String60;

enum BookDataID {
  ISBN, NAME, AUTHOR, KEYWORD, PRICE, STOCK
};

enum Privilege {
  GUEST = 0, CUSTOMER = 1, CLERK = 3, ADMIN = 7
};

namespace {

  template<typename T, BookDataID ID>
  T fromString(const std::string &s) {
    return T(s);
  }

  template<>
  String20 fromString<String20, ISBN>(const std::string &s) {
    if(!std::regex_match(s, ISBN_PATTERN)) {
      throw SyntaxError();
    }
    return String20(s);
  }

  template<>
  String60 fromString<String60, NAME>(const std::string &s) {
    std::smatch match;
    if(!std::regex_match(s, match, NAME_PATTERN)) {
      throw SyntaxError();
    }
    return String60(match[1]);
  }

  template<>
  String60 fromString<String60, AUTHOR>(const std::string &s) {
    std::smatch match;
    if(!std::regex_match(s, match, AUTHOR_PATTERN)) {
      throw SyntaxError();
    }
    return String60(match[1]);
  }

  template<>
  String60 fromString<String60, KEYWORD>(const std::string &s) {
    std::smatch match;
    if(!std::regex_match(s, match, KEYWORD_PATTERN)) {
      throw SyntaxError();
    }
    return String60(match[1]);
  }

  template<>
  double fromString<double, PRICE>(const std::string &s) {
    if(!std::regex_match(s, PRICE_PATTERN)) {
      throw SyntaxError();
    }
    return std::stod(s);
  }
}


template<typename T, BookDataID ID>
struct BookDataType {
  explicit BookDataType(const std::string &s) : value(fromString<T, ID>(s)) {}
  explicit BookDataType(const T &value) : value(value) {}
  BookDataType() = default;
  T value;
  auto operator<=>(const BookDataType &rhs) const = default;
  bool operator==(const BookDataType &rhs) const = default;
};
template<typename T, BookDataID ID>
std::ostream &operator<<(std::ostream &out, const BookDataType<T, ID> &rhs) {
  return out << rhs.value;
}

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
    return ISBNType(value);
  }
  if (name == "name") {
    return NameType(value);
  }
  if (name == "author") {
    return AuthorType(value);
  }
  if (name == "keyword") {
    return KeywordType(value);
  }
  throw Error("Invalid book value name");
}
BookDataModify parseModify(const std::string &name, const std::string &value) {
  if (name == "ISBN") {
    return ISBNType(value);
  }
  if (name == "name") {
    return NameType(value);
  }
  if (name == "author") {
    return AuthorType(value);
  }
  if (name == "keyword") {
    return KeywordType(value);
  }
  if (name == "price") {
    return PriceType(value);
  }
  throw Error("Invalid book value name");
}
#endif //BOOKSTORE_DATA_TYPES_HPP
