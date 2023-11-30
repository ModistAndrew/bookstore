#include "BlockedList.hpp"

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

int main() {
  int value;
  char tmp[64];
  BlockedList<Node, 4> test("test.txt");
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
  return 0;
}
