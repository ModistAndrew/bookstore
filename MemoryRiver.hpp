#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>
#include <iostream>
#include <filesystem>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

template<class T, class INFO>
class MemoryRiver {
private:
  fstream file;
  string file_name;
  static constexpr int T_SIZE = sizeof(T);
  static constexpr int INFO_LEN = sizeof(INFO);
  static constexpr int INT_SIZE = sizeof(int);
  int getEmpty() {
    file.seekg(INFO_LEN * INT_SIZE);
    int tmp;
    file.read(reinterpret_cast<char *>(&tmp), INT_SIZE);
    return tmp;
  }
  void putEmpty(int x) {
    file.seekp(INFO_LEN * INT_SIZE);
    file.write(reinterpret_cast<char *>(&x), INT_SIZE);
  }
  //store pointer to first empty just after info len.
  //empty except end has pointer to next empty; check EOF to determine whether at end. (so don't store anything after this)
public:
  MemoryRiver() = default;

  MemoryRiver(const string& file_name) : file_name(file_name) {
    initialise();
    file.open(file_name, std::ios::in | std::ios::out);
  }

  ~MemoryRiver() {
    file.close();
  }

  void initialise(string FN = "") {
    if (FN != "") file_name = FN;
    if(std::filesystem::exists(file_name)) return;
    file.open(file_name, std::ios::out);
    INFO tmp{};
      file.write(reinterpret_cast<char *>(&tmp), INT_SIZE);
    tmp = (INFO_LEN + 1) * INT_SIZE;
    file.write(reinterpret_cast<char *>(&tmp), INT_SIZE);
    file.close();
  }

  INFO get_info() {
    file.seekg(0);
    INFO ret;
    file.read(reinterpret_cast<char *>(&ret), INFO_LEN);
    return ret;
  }

  void write_info(const INFO &info) {
    file.seekp(0);
    file.write(reinterpret_cast<char *>(&info), INFO_LEN);
  }

  //在文件合适位置写入类对象t，并返回写入的位置索引index
  //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
  //位置索引index可以取为对象写入的起始位置
  int write(const T &t) {
    int index = getEmpty();
    file.seekg(index);
    int nxt;
    if(file.peek()==EOF) {
      nxt = index + T_SIZE;
      file.clear();
    } else {
      file.read(reinterpret_cast<char *>(&nxt), INT_SIZE);
    }
    file.seekp(index);
    file.write(reinterpret_cast<char *>(&t), T_SIZE);
    putEmpty(nxt);
    return index;
  }

  //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
  void update(T &t, int index) {
    file.seekp(index);
    file.write(reinterpret_cast<char *>(&t), T_SIZE);
  }

  //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
  T read(int index) {
    file.seekg(index);
    T ret;
    file.read(reinterpret_cast<char *>(&ret), T_SIZE);
    return ret;
  }

  //删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
  //you should never delete twice!
  void Delete(int index) {
    int nxt = getEmpty();
    putEmpty(index);
    file.seekp(index);
    file.write(reinterpret_cast<char *>(&nxt), INT_SIZE);
  }

  void print() {
    puts("info");
    for(int i=1; i <= INFO_LEN; i++) {
      int tmp;
      get_info(tmp, i);
      std::cout << tmp << " ";
    }
    puts("");
    puts("last");
    std::cout << getEmpty() << '\n';
    puts("");
    puts("content");
    file.tellg(); //why I have to call it again?
    while(file.peek()!=EOF) {
      T *tmp;
      file.read(reinterpret_cast<char *>(tmp), T_SIZE);
      int* begin = reinterpret_cast<int *>(tmp);
      std::cout << *begin << '\n';
      std::cout << *tmp << '\n';
    }
    file.clear();
    int start = getEmpty();
    puts("chain");
    while(file.peek()!=EOF) {
      file.seekg(start);
      std::cout << start << '\n';
      file.read(reinterpret_cast<char *>(&start), INT_SIZE);
    }
  }
};


#endif //BPT_MEMORYRIVER_HPP