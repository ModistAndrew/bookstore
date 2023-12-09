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

  [[nodiscard]] int len() const {
    return strnlen(key, L);
  }

  [[nodiscard]] char* begin() const {
    return key;
  }

  [[nodiscard]] char* end() const {
    return key + len();
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

enum Privilege {
  GUEST = 0, CUSTOMER = 1, CLERK = 3, ADMIN = 7
};

enum BookDataSearchID {
  ISBN_SEARCH, NAME_SEARCH, AUTHOR_SEARCH, KEYWORD_SEARCH
};

enum BookDataModifyID {
  ISBN_MODIFY, NAME_MODIFY, AUTHOR_MODIFY, KEYWORD_MODIFY, PRICE_MODIFY
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
  if(s=="7") {
    return ADMIN;
  }
  if(s=="3") {
    return CLERK;
  }
  if(s=="1") {
    return CUSTOMER;
  }
  if(s=="0") {
    return GUEST;
  }
  throw Error("Invalid privilege!");
}

template<>
BookDataSearchID fromString(const std::string &s) {
  if(s=="-ISBN") {
    return ISBN_SEARCH;
  }
  if(s=="-name") {
    return NAME_SEARCH;
  }
  if(s=="-author") {
    return AUTHOR_SEARCH;
  }
  if(s=="-keyword") {
    return KEYWORD_SEARCH;
  }
  throw Error("Invalid search type!");
}

template<>
BookDataModifyID fromString(const std::string &s) {
  if(s=="-ISBN") {
    return ISBN_MODIFY;
  }
  if(s=="-name") {
    return NAME_MODIFY;
  }
  if(s=="-author") {
    return AUTHOR_MODIFY;
  }
  if(s=="-keyword") {
    return KEYWORD_MODIFY;
  }
  if(s=="-price") {
    return PRICE_MODIFY;
  }
  throw Error("Invalid modify type!");
}

const std::string VISIBLE = R"([\x20-\x7E])";
const std::string AZ = "[a-zA-Z0-9_]";
const std::string DIGIT = "[0-9]";
const std::string DIGIT_DOT = "[0-9.]";
const std::string NO_QUOTIENT = R"([\x20-\x21\x23-\x7E])";

std::regex merge(const std::string &c, int len) {
  return std::regex("^(" + c + "{1," + std::to_string(len) + "})$");
}

std::regex mergeWithQuotient(const std::string &c, int len) {
  return std::regex("^\"(" + c + "{1," + std::to_string(len) + "})\"$");
}

std::regex options(const std::initializer_list<std::string>& strings) {
  std::string pattern = "^(";
  for (const auto& s : strings) {
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
const std::regex BOOK_DATA_SEARCH_PATTERN = options({"-ISBN", "-name", "-author", "-keyword"});
const std::regex BOOK_DATA_MODIFY_PATTERN = options({"-ISBN", "-name", "-author", "-keyword", "-price"});

template<int L>
std::vector<std::string> splitString(FixedString<L> str) {
  std::regex re("\\|"); // Matches '|'
  std::sregex_token_iterator first{str.begin(), str.begin() + str.len(), re, -1}, last;
  return {first, last};
}
#endif //BOOKSTORE_DATA_TYPES_HPP
