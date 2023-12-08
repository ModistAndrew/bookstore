//
// Created by zjx on 2023/12/6.
//

#ifndef BOOKSTORE_BOOKSTORAGE_HPP
#define BOOKSTORE_BOOKSTORAGE_HPP

#include "Error.hpp"
#include "DataTypes.hpp"
#include "PersistentSet.hpp"

struct Book { //may throw error
  String20 isbn;
  String60 name;
  String60 author;
  String60 keyword;
  double price;
  int stock;

  bool operator<(const Book &rhs) const {
    return isbn < rhs.isbn;
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
    class Visitor {
    public:
      template<typename T>
      void operator()(const T &data) {
        std::get<BookMap<T>>(bookMaps).iterate(data, [](const Book &book) {
          std::cout << book << '\n';
        }
      }
    };
  }

  void search(BookData data) {
    std::visit(Visitor(), data);
  }

  Book get(const String20 &isbn);

  bool modify(const String20 &isbn, const BookData data);

  bool remove(const String20 &isbn);
}
#endif //BOOKSTORE_BOOKSTORAGE_HPP
