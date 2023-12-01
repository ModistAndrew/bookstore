#include <fstream>
#include <iostream>
#include <filesystem>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

template<class T, class INFO>
class FileStorage {
private:
  fstream file;
  string fileName;
  static constexpr int T_SIZE = sizeof(T);
  static constexpr int INFO_LEN = sizeof(INFO);
  static constexpr int INT_SIZE = sizeof(int);

  int getEmpty() {
    file.seekg(INFO_LEN);
    int tmp;
    file.read(reinterpret_cast<char *>(&tmp), INT_SIZE);
    return tmp;
  }

  void setEmpty(int x) {
    file.seekp(INFO_LEN);
    file.write(reinterpret_cast<const char *>(&x), INT_SIZE);
  }
  //store pointer to first empty just after info len.
  //empty except end has pointer to next empty; check EOF to determine whether at end. (so don't store anything after this)
public:
  FileStorage() = default;

  FileStorage(const string &file_name) : fileName(file_name) {
    init();
    file.open(file_name, std::ios::in | std::ios::out);
  }

  ~FileStorage() {
    file.close();
  }

  void init() {
    if (std::filesystem::exists(fileName)) {
      return;
    }
    file.open(fileName, std::ios::out);
    INFO tmp{};
    file.write(reinterpret_cast<const char *>(&tmp), INFO_LEN);
    int empty = INFO_LEN + INT_SIZE;
    file.write(reinterpret_cast<const char *>(&empty), INT_SIZE);
    file.close();
  }

  INFO getInfo() {
    file.seekg(0);
    INFO ret;
    file.read(reinterpret_cast<char *>(&ret), INFO_LEN);
    return ret;
  }

  void setInfo(const INFO &info) {
    file.seekp(0);
    file.write(reinterpret_cast<const char *>(&info), INFO_LEN);
  }

  //在文件合适位置写入类对象t，并返回写入的位置索引index
  //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
  //位置索引index可以取为对象写入的起始位置
  int add(const T &t) {
    int index = getEmpty();
    file.seekg(index);
    int nxt;
    if (file.peek() == EOF) {
      nxt = index + T_SIZE;
      file.clear();
    } else {
      file.read(reinterpret_cast<char *>(&nxt), INT_SIZE);
    }
    file.seekp(index);
    file.write(reinterpret_cast<const char *>(&t), T_SIZE);
    setEmpty(nxt);
    return index;
  }

  //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
  void set(T &t, int index) {
    file.seekp(index);
    file.write(reinterpret_cast<const char *>(&t), T_SIZE);
  }

  //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
  T get(int index) {
    file.seekg(index);
    T ret;
    file.read(reinterpret_cast<char *>(&ret), T_SIZE);
    return ret;
  }

  //删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
  //you should never remove twice!
  void remove(int index) {
    int nxt = getEmpty();
    setEmpty(index);
    file.seekp(index);
    file.write(reinterpret_cast<const char *>(&nxt), INT_SIZE);
  }

  void print() {
    puts("info");
    INFO tmp = getInfo();
    std::cout << tmp << " ";
    puts("");
    puts("last");
    std::cout << getEmpty() << '\n';
    puts("");
    puts("content");
    file.tellg(); //why I have to call it again?
    while (file.peek() != EOF) {
      T *tmp;
      file.read(reinterpret_cast<char *>(tmp), T_SIZE);
      int *begin = reinterpret_cast<int *>(tmp);
      std::cout << *begin << '\n';
      std::cout << *tmp << '\n';
    }
    file.clear();
    int start = getEmpty();
    puts("chain");
    while (file.peek() != EOF) {
      file.seekg(start);
      std::cout << start << '\n';
      file.read(reinterpret_cast<char *>(&start), INT_SIZE);
    }
  }
};

//
// Created by zjx on 2023/11/30.
//

#include <algorithm>
#include <cstring>
#include <iostream>
#include <set>

template<class T, int BLOCK_SIZE>
struct Block {
  int size;
  T data[BLOCK_SIZE]; //sorted
  bool empty() const {
    return size <= 0;
  }

  bool full() const {
    return size >= BLOCK_SIZE;
  }

  void insert(T t) {
    if (full()) {
      std::cerr << "Block full\n";
      return;
    }
    int p = std::lower_bound(data, data + size, t) - data;
    if (p < size && !(data[p] < t) && !(t < data[p])) {
//      std::cout << "Already exists\n";
      return;
    }
    std::memmove(data + p + 1, data + p, (size - p) * sizeof(T));
    data[p] = t;
    size++;
  }

  bool erase(T t) {
    int p = std::lower_bound(data, data + size, t) - data;
    if (p >= size || (data[p] < t || t < data[p])) {
//      std::cout << "Not found\n";
      return false;
    }
    std::memmove(data + p, data + p + 1, (size - p) * sizeof(T));
    size--;
    return true;
  }

  int getLastNoGreater(const T &t) const { //may be -1
    if (empty()) {
      std::cerr << "Block empty\n";
    }
    return std::upper_bound(data, data + size, t) - data - 1;
  }

  int getFirstNoSmaller(const T &t) const { //may be size
    if (empty()) {
      std::cerr << "Block empty\n";
    }
    return std::lower_bound(data, data + size, t) - data;
  }

  T min() const {
    if (empty()) {
      std::cerr << "Block empty\n";
    }
    return data[0];
  }

  Block split() { //split out a new one and half self
    Block ret;
    ret.size = size / 2;
    size -= ret.size;
    std::memmove(ret.data, data + size, ret.size * sizeof(T));
    return ret;
  }
};

template<class T, int SIZE>
class BlockedList {
  //behave like std::set
  struct Index {
    T min;
    int pos;

    bool operator<(const Index &rhs) const {
      return min < rhs.min;
    }
  };

  Block<Index, SIZE> indexBlock; //can be cached and stored when destructed
  Block<T, SIZE> currentBlock; //temp
  FileStorage<decltype(currentBlock), decltype(indexBlock)> storage;

public:
  BlockedList() = default;

  BlockedList(const string &file_name) : storage(file_name) {
    indexBlock = storage.getInfo();
  }

  ~BlockedList() {
    storage.setInfo(indexBlock);
  }

  void insert(const T &t) {
    if (indexBlock.empty()) {
      currentBlock = {1, {t}};
      indexBlock.insert({t, storage.add(currentBlock)});
      return;
    }
    int indexId = indexBlock.getLastNoGreater({t, 0});
    Index &index = indexBlock.data[std::max(0, indexId)]; //just for cmp. pos is useless
    currentBlock = storage.get(index.pos);
    currentBlock.insert(t);
    storage.set(currentBlock, index.pos);
    index.min = currentBlock.min();
    if (currentBlock.full()) {
      Block<T, SIZE> newBlock = currentBlock.split();
      storage.set(currentBlock, index.pos); //overwrite as currentBlock has changed
      indexBlock.insert({newBlock.min(), storage.add(newBlock)});
    }
  }

  bool erase(const T &t) {
    if (indexBlock.empty()) {
      return false;
    }
    int indexId = indexBlock.getLastNoGreater({t, 0});
    if(indexId < 0) {
      return false;
    }
    Index &index = indexBlock.data[indexId]; //just for cmp. pos is useless
    currentBlock = storage.get(index.pos);
    if (currentBlock.erase(t)) {
      if (currentBlock.empty()) {
        storage.remove(index.pos);
        indexBlock.erase(index);
      } else {
        storage.set(currentBlock, index.pos);
        index.min = currentBlock.min();
      }
      return true;
    }
    return false;
  }

  std::vector<T> search(const T &min, const T &max) {
    std::vector<T> ret;
    T tmp;
    int startIndexGlobal = std::max(0, indexBlock.getLastNoGreater({min, 0}));
    currentBlock = storage.get(indexBlock.data[startIndexGlobal].pos);
    int startIndexLocal = currentBlock.getFirstNoSmaller(min);
    for(int i=startIndexLocal; i<currentBlock.size; i++) {
      tmp = currentBlock.data[i];
      if(max < tmp) {
        return ret;
      }
      ret.push_back(tmp);
    } //first eat the current block
    for(int i=startIndexGlobal+1; i<indexBlock.size; i++) {
      currentBlock = storage.get(indexBlock.data[i].pos);
      for(int j=0; j<currentBlock.size; j++) {
        tmp = currentBlock.data[j];
        if(max < tmp) {
          return ret;
        }
        ret.push_back(tmp);
      }
    }
    return ret;
  }
};

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

int main() {
//  freopen("out.txt", "w", stdout);
//  freopen("data.txt", "r", stdin);
  myAns();
  return 0;
}
