//
// Created by zjx on 2023/11/30.
//

#ifndef BOOKSTORE_BLOCKEDLIST_HPP
#define BOOKSTORE_BLOCKEDLIST_HPP

#include <algorithm>
#include <cstring>
#include "MemoryRiver.hpp"

template<class T, int BLOCK_SIZE>
struct Block {
  int size;
  T data[BLOCK_SIZE]; //sorted
  void insert(T t) {
    int p = std::upper_bound(data, data + size) - data;
    std::memmove(data + p + 1, data + p, (size - p) * sizeof(T));
    data[p] = t;
    size++;
  }

  void remove(T t) {
    int p = std::upper_bound(data, data + size) - data - 1;
    if (p < 0 || data[p] != t) {
      return;
    }
    std::memmove(data + p, data + p + 1, (size - p - 1) * sizeof(T));
    size--;
  }
};

template<class T, int BLOCK_SIZE>
class BlockedList {
  struct Index {
    T data;
    int index;

    bool operator<(const Index &rhs) const {
      return data < rhs.data;
    }
  };
  Block<Index, BLOCK_SIZE> index_block;
  Block<T, BLOCK_SIZE> blocks;
  MemoryRiver<Block<T, BLOCK_SIZE>, Block<Index, BLOCK_SIZE>> file;
};

#endif //BOOKSTORE_BLOCKEDLIST_HPP
