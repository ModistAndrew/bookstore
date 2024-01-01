//
// Created by zjx on 2023/12/6.
//

#ifndef BOOKSTORE_BOOK_HPP
#define BOOKSTORE_BOOK_HPP

#include "Utils.hpp"
#include "PersistentSet.hpp"
#include <iostream>
#include <iomanip>

struct Book {
  String20 isbn;
  String60Chinese name;
  String60Chinese author;
  String60Chinese keyword; //full keywords
  Double price;
  int stock;

  auto operator<=>(const Book &rhs) const {
    return isbn <=> rhs.isbn;
  }

  bool operator==(const Book &rhs) const {
    return isbn == rhs.isbn;
  }

  friend std::ostream &operator<<(std::ostream &out, const Book &b) {
    return out << b.isbn << '\t' << b.name << '\t' << b.author << '\t' << b.keyword << '\t'
               << b.price << '\t' << b.stock;
  }

  [[nodiscard]] bool empty() const {
    return isbn.empty();
  }

  static constexpr Book min() {
    return Book{String20::min()};
  }

  static constexpr Book max() {
    return Book{String20::max()};
  }
};

namespace Books {
  template<typename T>
  using BookMap = PersistentMap<T, Book, 450>;
  BookMap<String20> isbnMap(false, "isbn");
  BookMap<String60Chinese> nameMap(true, "name");
  BookMap<String60Chinese> authorMap(true, "author");
  BookMap<String60Chinese> keywordMap(true, "keyword"); //key for each keyword

  void store(const Book &book) { //store the book into the maps. no empty book allowed
    if (book.empty()) {
      throw Error("Empty book!");
    }
    isbnMap.put(book.isbn, book);
    nameMap.put(book.name, book);
    authorMap.put(book.author, book);
    for (const String60Chinese &kw: book.keyword.split()) {
      keywordMap.put(kw, book);
    }
  }

  struct PersistentBook: public Book {
    bool save = false;

    explicit PersistentBook(const Book &book): Book(book) {}

    ~PersistentBook() {
      if (!this->empty() && save) {
        store(*this);
      }
    }
  };

  PersistentBook extract(const String20 &isbn) { //extract the book with the given ISBN_TYPE and remove it. set the save flag to save it
    PersistentBook ret(isbnMap.get(isbn));
    if (ret.empty()) {
      throw Error("ISBN empty when extracting");
    }
    isbnMap.remove(ret.isbn, ret);
    nameMap.remove(ret.name, ret);
    authorMap.remove(ret.author, ret);
    for (const String60Chinese &kw: ret.keyword.split()) {
      keywordMap.remove(kw, ret);
    }
    return ret;
  }
}
#endif //BOOKSTORE_BOOK_HPP
