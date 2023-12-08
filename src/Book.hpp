//
// Created by zjx on 2023/12/6.
//

#ifndef BOOKSTORE_BOOK_HPP
#define BOOKSTORE_BOOK_HPP

#include "Error.hpp"
#include "DataTypes.hpp"
#include "PersistentSet.hpp"

struct Book {
  std::tuple<ISBNType,
    NameType,
    AuthorType,
    KeywordType,
    PriceType,
    StockType> data;

  auto operator<=>(const Book &rhs) const {
    return std::get<ISBNType>(data) <=> std::get<ISBNType>(rhs.data);
  }

  bool operator==(const Book &rhs) const {
    return std::get<ISBNType>(data) == std::get<ISBNType>(rhs.data);
  }

  [[nodiscard]] bool empty() const {
    return std::get<ISBNType>(data).value.empty();
  }
  static const Book MIN;
  static const Book MAX;
};

std::ostream &operator<<(std::ostream &out, const Book &b) {
  return out << std::get<ISBNType>(b.data) << '\t'
             << std::get<NameType>(b.data) << '\t'
             << std::get<AuthorType>(b.data) << '\t'
             << std::get<KeywordType>(b.data) << '\t'
             << std::get<PriceType>(b.data) << '\t'
             << std::get<StockType>(b.data);
}

const Book Book::MIN = Book{{ISBNType(String20::min()),
                            NameType(),
                            AuthorType(),
                            KeywordType(),
                            PriceType(),
                            StockType()}};
const Book Book::MAX = Book{{ISBNType(String20::max()),
                             NameType(),
                             AuthorType(),
                             KeywordType(),
                             PriceType(),
                             StockType()}};
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
    struct PersistentBook: public Book {
      ~PersistentBook() {
        if(!empty()) { //store the book into the maps
          std::get<BookMap<ISBNType>>(bookMaps).put(std::get<ISBNType>(data), *this);
          std::get<BookMap<NameType>>(bookMaps).put(std::get<NameType>(data), *this);
          std::get<BookMap<AuthorType>>(bookMaps).put(std::get<AuthorType>(data), *this);
          std::get<BookMap<KeywordType>>(bookMaps).put(std::get<KeywordType>(data), *this);
        }
      }
    };
    std::optional<PersistentBook> currentBook;
    //every time you emplace it with a new book, the destructor will be called and the old book will be saved
  }

  void search(const BookDataSearch &data) {
    std::visit([]<typename T>(const T &datum) -> void {
      std::get<BookMap<T>>(bookMaps).iterate(datum, [](const Book &book) {
        std::cout << book << std::endl;
      });
    }, data);
  }

  bool modify(BookDataFull &data) { //TODO: check ISBN
    if(currentBook.has_value()) {
      std::visit([]<typename T>(const T &datum) -> void {
        std::get<T>(currentBook->data) = datum;
      }, data);
      return true;
    }
    return false;
  }

  bool select(const ISBNType &isbn) {
    Book tmp = std::get<BookMap<ISBNType>>(bookMaps).get(isbn);
    if(tmp.empty()) {
      return false;
    }
    currentBook.emplace(tmp);
    return true;
  }
}
#endif //BOOKSTORE_BOOK_HPP
