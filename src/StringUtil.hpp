//
// Created by zjx on 2023/12/9.
//

#ifndef BOOKSTORE_STRING_UTIL_HPP
#define BOOKSTORE_STRING_UTIL_HPP

#include <string>
#include <regex>
const std::string VISIBLE = R"([\x20-\x7E])";
const std::string AZ = "[a-zA-Z0-9_]";
const std::string DIGIT = "[0-9]";
const std::string PRIVILEGE_DIGIT = "[0137]";
const std::string DIGIT_DOT = "[0-9.]";
const std::string NO_QUOTIENT = R"([\x20-\x21\x23-\x7E])";

std::regex merge(const std::string &c, int len) {
  return std::regex("^" + c + "{1," + std::to_string(len) + "}$");
}

std::regex mergeWithQuotient(const std::string &c, int len) {
  return std::regex("^\"(" + c + "{1," + std::to_string(len) + "})\"$");
}

const std::regex USER_ID_PATTERN = merge(AZ, 30);
const std::regex PASSWORD_PATTERN = merge(AZ, 30);
const std::regex USER_NAME_PATTERN = merge(VISIBLE, 30);
const std::regex PRIVILEGE_PATTERN = merge(PRIVILEGE_DIGIT, 1);
const std::regex ISBN_PATTERN = merge(VISIBLE, 20);
const std::regex NAME_PATTERN = mergeWithQuotient(NO_QUOTIENT, 60);
const std::regex AUTHOR_PATTERN = mergeWithQuotient(NO_QUOTIENT, 60);
const std::regex KEYWORD_PATTERN = mergeWithQuotient(NO_QUOTIENT, 60);
const std::regex COUNT_PATTERN = merge(DIGIT, 10);
const std::regex PRICE_PATTERN = merge(DIGIT_DOT, 13);
#endif //BOOKSTORE_STRING_UTIL_HPP
