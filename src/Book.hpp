//
// Created by zjx on 2023/12/6.
//

#ifndef BOOKSTORE_BOOK_HPP
#define BOOKSTORE_BOOK_HPP

#include "Utils.hpp"
#include "PersistentSet.hpp"

struct Book {
  String20 isbn;
  String60 name;
  String60 author;
  String60 keyword;
  double price;
  int stock;

  auto operator<=>(const Book &rhs) const {
    return isbn <=> rhs.isbn;
  }

  bool operator==(const Book &rhs) const {
    return isbn == rhs.isbn;
  }

  [[nodiscard]] bool empty() const {
    return isbn.empty();
  }

  static const Book MIN;
  static const Book MAX;
};

std::ostream &operator<<(std::ostream &out, const Book &b) {
  return out << b.isbn << '\t' << b.name << '\t' << b.author << '\t' << b.keyword << '\t' << b.price << '\t' << b.stock;
}

const Book Book::MIN = Book{String20::min()};
const Book Book::MAX = Book{String20::max()};
namespace Books {
  namespace {
    template<typename T>
    using BookMap = PersistentMap<T, Book, 100>;
    BookMap<String20> isbnMap(false, "isbn");
    BookMap<String60> nameMap(true, "name");
    BookMap<String60> authorMap(true, "author");
    BookMap<String60> keywordMap(true, "keyword");

    struct PersistentBook : public Book { //TODO: what if ISBN is modified?
      ~PersistentBook() {
        if (!empty()) { //store the book into the maps
          isbnMap.put(isbn, *this);
          nameMap.put(name, *this);
          authorMap.put(author, *this);
          keywordMap.put(keyword, *this);
        }
      }
    };

    std::optional<PersistentBook> currentBook;
    //every time you emplace it with a new book, the destructor will be called and the old book will be saved
  }

  void searchISBN(const String20 &isbn) {
    isbnMap.iterate(isbn, [](const Book &b) {
      std::cout << b << std::endl;
    });
  }

  void searchName(const String60 &name) {
    nameMap.iterate(name, [](const Book &b) {
      std::cout << b << std::endl;
    });
  }

  void searchAuthor(const String60 &author) {
    authorMap.iterate(author, [](const Book &b) {
      std::cout << b << std::endl;
    });
  }

  void searchKeyword(const String60 &keyword) { //TODO: how to split the keyword?
    keywordMap.iterate(keyword, [](const Book &b) {
      std::cout << b << std::endl;
    });
  }

  void modifyISBN(const String20 &isbn) { //TODO: what if ISBN is modified?
    currentBook->isbn = isbn;
  }

  void modifyName(const String60 &name) {
    currentBook->name = name;
  }

  void modifyAuthor(const String60 &author) {
    currentBook->author = author;
  }

  void modifyKeyword(const String60 &keyword) { //TODO: how to split the keyword?
    currentBook->keyword = keyword;
  }

  void modifyPrice(double price) {
    currentBook->price = price;
  }

  void modifyStock(int stock) {
    currentBook->stock = stock;
  }

  bool present() {
    return currentBook.has_value();
  }

  PersistentBook getBook(const String20 &isbn) { //return a book with the given ISBN. You can modify it and it will be stored automatically
    return PersistentBook(isbnMap.get(isbn));
  }

  bool select(const String20 &isbn) {
    PersistentBook tmp = getBook(isbn); //may be empty
    currentBook.emplace(tmp);
    return true;
  }
}
#endif //BOOKSTORE_BOOK_HPP
