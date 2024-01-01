//
// Created by zjx on 2024/1/1.
//

#ifndef BOOKSTORE_VALIDATOR_HPP
#define BOOKSTORE_VALIDATOR_HPP

#include "Error.hpp"
#include <regex>

template<typename T>
struct Validator {
  const T &t;
  bool positive;
  Validator& And;
  Validator& but;
  Validator& Or;

  Validator(const T &_t) : And(*this), but(*this), Or(*this), t(_t), positive(true) {
  }

  Validator &execute(bool passed) {
    if (passed ^ positive) {
      throw ValidationError();
    }
    return *this;
  }

  Validator &toBe(const T &b) {
    return execute(t == b);
  }

  template<typename U>
  Validator &toBe() {
    return execute(dynamic_cast<const U *>(&t));
  }

  template<typename... Args>
  Validator &toBeOneOf(const T &first, const Args &... ts) {
    if (t == first) {
      return execute(true);
    }
    return toBeOneOf(ts...);
  }

  Validator &toBeOneOf() {
    return execute(false);
  }

  Validator &le(const T &b) {
    return execute(t <= b);
  }

  Validator &ge(const T &b) {
    return execute(t >= b);
  }

  Validator &Not() {
    positive = !positive;
    return *this;
  }

  Validator &consistedOf(const T &b) {
    for (int i = 0; i < t.size(); i++) {
      if (b.find(t[i]) == -1) {
        return execute(false);
      }
    }
    return execute(true);
  }

  Validator &toMatch(const T &b) {
    return execute(std::regex_match(t, std::basic_regex(b)));
  }
};

template<typename T>
Validator<T> expect(const T &t) {
  return Validator(t);
}

#endif //BOOKSTORE_VALIDATOR_HPP
