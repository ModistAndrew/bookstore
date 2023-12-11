//
// Created by zjx on 2023/12/11.
//

#ifndef BOOKSTORE_PERSISTENT_VECTOR_HPP
#define BOOKSTORE_PERSISTENT_VECTOR_HPP

#include "FileStorage.hpp"
#include "Error.hpp"
#include <functional>

template<typename T>
class PersistentVector {
  struct Info {
    int last;
    int size;
  };
  static constexpr int STEP = sizeof(T);
  FileStorage<T, Info> storage;
  Info info;
public:
  explicit PersistentVector(const string &file_name) : storage(file_name) {
    info = storage.getInfo();
  }

  ~PersistentVector() {
    storage.setInfo(info);
  }

  void push_back(const T &t) {
    info.size++;
    info.last = storage.add(t);
  }

  void iterate(int cnt, std::function<void(const T &)> f) { //backwards
    if (cnt > info.size) {
      throw Error("cnt > size");
    }
    if(cnt == -1) {
      cnt = info.size;
    }
    for (int i = 0; i < cnt; i++) {
      f(storage.get(info.last - i * STEP)); //as we never remove, this is safe
    }
  }
};

#endif //BOOKSTORE_PERSISTENT_VECTOR_HPP
