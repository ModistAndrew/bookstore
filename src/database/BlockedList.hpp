//
// Created by zjx on 2023/11/30.
//

#ifndef BOOKSTORE_BLOCKEDLIST_HPP
#define BOOKSTORE_BLOCKEDLIST_HPP

#include <algorithm>
#include <cstring>
#include "FileStorage.hpp"

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

  bool insert(T t) {
    if (full()) {
      return false;
    }
    int p = std::lower_bound(data, data + size, t) - data;
    if (p < size && !(data[p] < t) && !(t < data[p])) {
      return false;
    }
    std::memmove(data + p + 1, data + p, (size - p) * sizeof(T));
    data[p] = t;
    size++;
    return true;
  }

  bool erase(T t) {
    int p = std::lower_bound(data, data + size, t) - data;
    if (p >= size || (data[p] < t || t < data[p])) {
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

template<class T, int SIZE> //SIZE is the max size of indexBlock and currentBlock
//TODO: maintain the size of each block more properly to optimize time and space usage
//TODO: wrap as a multimap
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

  bool insert(const T &t) {
    if (indexBlock.empty()) { //init
      currentBlock = {1, {t}};
      indexBlock.insert({t, storage.add(currentBlock)});
      return true;
    }
    int indexId = indexBlock.getLastNoGreater({t, 0}); //just for cmp. pos is useless
    Index &index = indexBlock.data[std::max(0, indexId)];
    currentBlock = storage.get(index.pos);
    if(!currentBlock.insert(t)) {
      return false;
    }
    storage.set(currentBlock, index.pos);
    index.min = currentBlock.min();
    if (currentBlock.full()) { //split
      auto newBlock = currentBlock.split();
      storage.set(currentBlock, index.pos); //overwrite as currentBlock has changed
      indexBlock.insert({newBlock.min(), storage.add(newBlock)});
    }
    return true;
  }

  bool erase(const T &t) {
    if (indexBlock.empty()) {
      return false;
    }
    int indexId = indexBlock.getLastNoGreater({t, 0}); //just for cmp. pos is useless
    if(indexId < 0) {
      return false;
    }
    Index &index = indexBlock.data[indexId];
    currentBlock = storage.get(index.pos);
    if (currentBlock.erase(t)) {
      if (currentBlock.empty()) { //I think delete when empty is the best, as you needn't concern about merging
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
    int startIndexGlobal = std::max(0, indexBlock.getLastNoGreater({min, 0})); //just for cmp. pos is useless
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

#endif //BOOKSTORE_BLOCKEDLIST_HPP
