#include "BlockedList.hpp"

int N;
class Node {
  char key[64];
  int value;
public:
  Node (const string &s, int _value): value(_value) {
    for(int i=0; i<64; i++) {
      key[i] = (i < s.size()) ? s[i] : 0;
    }
  }
  bool operator < (const Node &rhs) const {
    return strcmp(key, rhs.key) < 0 || (strcmp(key, rhs.key) == 0 && value < rhs.value);
  }

  Node& operator = (const Node &rhs)  = default;
};

int main() {
  int value;
  char tmp[64];
  BlockedList<Node, 20> test("test.txt");
  std::cin >> N;
  std::string s;
  for (int i = 1; i <= N; i++) {
    std::cin >> s;
    if (s[0] == 'i') {
      std::cin >> s;
      std::cin >> value;

    } else if (s[0] == 'f') {

    } else {

    }
  }
  return 0;
}
