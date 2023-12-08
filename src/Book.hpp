//
// Created by zjx on 2023/12/6.
//

#ifndef BOOKSTORE_BOOK_HPP
#define BOOKSTORE_BOOK_HPP

#include "Error.hpp"
#include "DataTypes.hpp"
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

  static const Book MIN;
  static const Book MAX;
};

const Book Book::MIN = Book{String20::min()};
const Book Book::MAX = Book{String20::max()};
namespace Books {
  namespace {
    template<typename T>
    using BookMap = PersistentMap<T, Book, 100>;
    std::tuple<BookMap<ISBNType>,
      BookMap<NameType>,
      BookMap<AuthorType>,
      BookMap<KeywordType>> bookMaps = std::make_tuple(BookMap<ISBNType>(false, "isbn"),
                                                       BookMap<NameType>(true, "name"),
                                                       BookMap<AuthorType>(true, "author"),
                                                       BookMap<KeywordType>(true, "keyword"));
    Book currentBook;
  }

  void search(const BookDataSearch &data) {
    std::visit([]<typename T>(const T &datum) -> void {
      std::get<BookMap<T>>(bookMaps).iterate(datum, [](const Book &book) {
      });
    }, data);
  }

  Book get(const String20 &isbn);

  bool modify(const String20 &isbn, const BookDataModify &data);

  bool remove(const String20 &isbn);
}
#endif //BOOKSTORE_BOOK_HPP
