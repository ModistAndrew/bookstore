//
// Created by zjx on 2023/12/6.
//

#ifndef BOOKSTORE_DATA_TYPES_HPP
#define BOOKSTORE_DATA_TYPES_HPP

#include <cstring>
#include <map>
#include <variant>
#include "Error.hpp"
#include <set>
#include <iostream>

template<int L>
class FixedString { // Fixed length string with max length L
  char key[L];
public:
  explicit FixedString(const std::string &s) : key{} {
    if (s.length() > L) {
      throw Error("String too long, this should never happen!");
    }
    FixedString &fx = *this;
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

  [[nodiscard]] int len() const {
    return strnlen(key, L);
  }

  [[nodiscard]] char *begin() const {
    return key;
  }

  [[nodiscard]] char *end() const {
    return key + len();
  }

  friend std::ostream &operator<<(std::ostream &out, const FixedString &rhs) {
    for (int i = 0; i < L; i++) {
      if (rhs.key[i] == '\0') {
        break;
      }
      out << rhs.key[i];
    }
    return out;
  }

  static constexpr FixedString min() {
    FixedString ret;
    memset(ret.key, 0, sizeof(ret.key));
    return ret;
  }

  static constexpr FixedString max() {
    FixedString ret;
    memset(ret.key, 0xFF, sizeof(ret.key));
    return ret;
  }

  [[nodiscard]] bool empty() const {
    return key[0] == '\0';
  }

  std::set<FixedString> split() const {
    std::set<FixedString> ret;
    if (empty()) {
      return ret;
    }
    char tmp[60]{};
    int tmpLength = 0;
    for (int i = 0; i < L && key[i] != '\0'; i++) {
      if (key[i] != '|') {
        tmp[tmpLength++] = key[i];
      } else {
        if (tmpLength == 0) {
          throw Error("Empty keyword!");
        }
        if (!ret.insert(FixedString{std::string(tmp, tmpLength)}).second) {
          throw Error("Duplicate keyword!");
        }
        tmpLength = 0; //needn't reset tmp
      }
    }
    if (tmpLength == 0) {
      throw Error("Empty keyword!");
    }
    if (!ret.insert(FixedString{std::string(tmp, tmpLength)}).second) {
      throw Error("Duplicate keyword!");
    }
    return ret;
  }

};

typedef FixedString<20> String20;
typedef FixedString<30> String30;
typedef FixedString<60> String60;

enum Privilege {
  GUEST = 0, CUSTOMER = 1, CLERK = 3, ADMIN = 7
};

enum BookDataID {
  ISBN_TYPE, NAME_TYPE, AUTHOR_TYPE, KEYWORD_TYPE, PRICE_TYPE
};

std::map<std::string, Privilege> privilegeMap = {
  {"7", ADMIN},
  {"3", CLERK},
  {"1", CUSTOMER},
  {"0", GUEST}
};

std::map<std::string, BookDataID> bookDataMap = {
  {"ISBN_TYPE", ISBN_TYPE},
  {"name",      NAME_TYPE},
  {"author",    AUTHOR_TYPE},
  {"keyword",   KEYWORD_TYPE},
  {"price", PRICE_TYPE}
};

template<typename T>
T fromString(const std::string &s) {
  return T(s);
}

template<>
int fromString(const std::string &s) {
  return stoi(s);
}

template<>
double fromString(const std::string &s) {
  return stod(s);
}

template<>
Privilege fromString(const std::string &s) {
  if (privilegeMap.find(s) == privilegeMap.end()) {
    throw Error("Invalid privilege!");
  }
  return privilegeMap[s];
}

template<>
BookDataID fromString(const std::string &s) {
  if (bookDataMap.find(s) == bookDataMap.end()) {
    throw Error("Invalid modify type!");
  }
  return bookDataMap[s];
}

const std::string VISIBLE = R"([\x20-\x7E])";
const std::string AZ = "[a-zA-Z0-9_]";
const std::string DIGIT = "[0-9]";
const std::string DIGIT_DOT = "[0-9.]";
const std::string NO_QUOTIENT = R"([\x20-\x21\x23-\x7E])";

std::regex merge(const std::string &c, int len) { //shouldn't be empty
  return std::regex("^(" + c + "{1," + std::to_string(len) + "})$");
}

std::regex mergeWithQuotient(const std::string &c, int len) { //shouldn't be empty
  return std::regex("^\"(" + c + "{1," + std::to_string(len) + "})\"$");
}

std::regex options(const std::initializer_list<std::string> &strings) {
  std::string pattern = "^(";
  for (const auto &s: strings) {
    pattern += s;
    pattern += "|";
  }
  // Remove the last '|'
  pattern.pop_back();
  pattern += ")$";
  return std::regex(pattern);
}

const std::regex USER_ID_PATTERN = merge(AZ, 30);
const std::regex PASSWORD_PATTERN = merge(AZ, 30);
const std::regex USER_NAME_PATTERN = merge(VISIBLE, 30);
const std::regex PRIVILEGE_PATTERN = options({"0", "1", "3", "7"});
const std::regex ISBN_PATTERN = merge(VISIBLE, 20);
const std::regex NAME_PATTERN = mergeWithQuotient(NO_QUOTIENT, 60);
const std::regex AUTHOR_PATTERN = mergeWithQuotient(NO_QUOTIENT, 60);
const std::regex KEYWORD_PATTERN = mergeWithQuotient(NO_QUOTIENT, 60);
const std::regex COUNT_PATTERN = merge(DIGIT, 10);
const std::regex PRICE_PATTERN = merge(DIGIT_DOT, 13);
const std::regex BOOK_DATA_PATTERN = options({"ISBN", "name", "author", "keyword", "price"});

#endif //BOOKSTORE_DATA_TYPES_HPP
