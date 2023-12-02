#include <set>
#include "../src/database/BlockedList.hpp"
int N;

struct Node {
  char key[65];
  int value;

  Node(const string &s, int _value) : value(_value) {
    for (int i = 0; i < 65; i++) {
      key[i] = (i < s.size()) ? s[i] : 0;
    }
  }

  bool operator<(const Node &rhs) const {
    return strcmp(key, rhs.key) < 0 || (strcmp(key, rhs.key) == 0 && value < rhs.value);
  }

  Node() = default;
};

std::set<Node> test;
fstream file;
Node tmp;

void myAns(){
  int value;
  BlockedList<Node, 500> test("test1.txt");
  std::cin >> N;
  std::string s;
  for (int i = 1; i <= N; i++) {
    std::cin >> s;
    if (s[0] == 'i') {
      std::cin >> s;
      std::cin >> value;
      test.insert(Node(s, value));
    } else if (s[0] == 'f') {
      std::cin >> s;
      bool flag = false;
      for (auto &n: test.search(Node(s, INT32_MIN), Node(s, INT32_MAX))) {
        flag = true;
        std::cout << n.value << " ";
      }
      std::cout << (flag ? "\n" : "null\n");
    } else {
      std::cin >> s;
      std::cin >> value;
      test.erase(Node(s, value));
      }
    }
};

void rightAns() {
  int value;
  std::cin >> N;
  std::string s;
  file.open("test.txt", std::ios::in);
  while(file.good()) {
    file.read(reinterpret_cast<char *>(&tmp), sizeof(Node));
    test.insert(tmp);
  }
  file.close();
  for (int i = 1; i <= N; i++) {
    std::cin >> s;
    if (s[0] == 'i') {
      std::cin >> s;
      std::cin >> value;
      test.insert(Node(s, value));
    } else if (s[0] == 'f') {
      std::cin >> s;
      bool flag = false;
      auto iterator1=test.lower_bound (Node(s, INT32_MIN));
      auto iterator2=test.upper_bound (Node(s, INT32_MAX));
      for (auto n = iterator1; n != iterator2; n++) {
        flag = true;
        std::cout << (*n).value << " ";
      }
      std::cout << (flag ? "\n" : "null\n");
    } else {
      std::cin >> s;
      std::cin >> value;
      test.erase(Node(s, value));
    }
  }
  file.open("test.txt", std::ios::out);
  for(const Node &n: test) {
    file.write(reinterpret_cast<const char *>(&n), sizeof(Node));
  }
  file.close();
}