#include "BlockedList.hpp"

int main() {
  m.write_info(10, 1);
  m.write_info(3, 7);
  Node n = {1, "123456789"};
  int index[10];
  for(int i=0; i<10; i++) {
    index[i] = m.write(n);
    n.a++;
  }
  m.Delete(index[3]);
  m.Delete(index[5]);
  m.Delete(index[7]);
  n.a = 114514;
  m.write(n);
  int tmp;
  for(int i=1; i<=10; i++) {
    m.get_info(tmp, i);
    std::cout << tmp << std::endl;
  }
  m.print();
  file.open(file_name, std::ios::in | std::ios::out);
  file.seekp(4);
  for (int i = 1; i < 5; i++) {
    file.write(reinterpret_cast<char *>(&i), sizeof(int));
  }
//  file.seekg(0);
//  file.seekp(0);
//  int tmp;
//  while (file.peek() != EOF) {
//    file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
//    std::cout << tmp << std::endl;
//  }
//  file.seekp(0);
//  tmp=114514;
//  for (int i = -10; i < 10; i++) {
//    file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
//  }
  file.close();
  return 0;
}
