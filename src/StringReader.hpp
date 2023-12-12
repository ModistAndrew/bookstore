//
// Created by zjx on 2023/12/12.
//

#ifndef BOOKSTORE_STRING_READER_HPP
#define BOOKSTORE_STRING_READER_HPP
#include <vector>
#include <string>
class StringReader { //simply use ' ' as delim and provide function to store token back
  std::vector<std::string> words;
public:
  StringReader() = default;

  explicit StringReader(const std::string& s) {
    bool space = true;
    std::string tmp;
    for (char c : s) {
      if (c == ' ') {
        if(!space) {
          words.push_back(tmp);
          tmp.clear(); //push back the previous word
        }
        space = true;
      } else {
        tmp.push_back(c);
        space = false;
      }
    }
    if(!tmp.empty()) {
      words.push_back(tmp);
    }
    std::reverse(words.begin(), words.end());
  }

  std::string get() {
    if(words.empty()) {
      return "";
    }
    std::string ret = words.back();
    words.pop_back();
    return ret;
  }

  std::string touch() {
    if(words.empty()) {
      return "";
    }
    return words.back();
  }

  bool empty() {
    return words.empty();
  }

  void store(const std::string& s) {
    words.push_back(s);
  }
};

#endif //BOOKSTORE_STRING_READER_HPP
