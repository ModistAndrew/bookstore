//
// Created by zjx on 2023/12/1.
//
#include <unordered_set>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

std::vector<std::string> ops, names;
std::unordered_set<std::string> lines;

char nextChar() {
  return 'a' + rand() % 26;
}

void dataGen() {
  ops.push_back("insert");
  ops.push_back("delete");
  ops.push_back("find");
  srand(time(nullptr));
  for(int i = 0; i < 1000; i++) {
    int cnt = rand() % 64 + 1;
    std::string s;
    for(int j = 0; j < cnt; j++) {
      s.push_back(nextChar());
    }
    if(s.length()<=64) {
      names.push_back(s);
    }
  }
  while(lines.size()<100000) {
    std::stringstream ss;
    int op = rand() % 3;
    ss << ops[op] << " " << names[rand() % 1000];
    if(op!=2) {
      ss << " " << rand();
    }
    lines.insert(ss.str());
  }
  std::cout << lines.size() << std::endl;
  for(const std::string &s: lines) {
    std::cout << s << std::endl;
  }
}