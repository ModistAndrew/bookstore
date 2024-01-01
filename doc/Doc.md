# Account
a struct for account information containing id, name, password, privilege
# Accounts
manage accounts stored in the file
# Book
a struct for book information containing isbn, name, author, keyword, price, quantity
# Books
manage books stored in the file
# Bookstore
main class of the program, handle input and output
# Command
a struct for command containing command name, minimum privilege, and how it reads arguments and executes
# Scanner
a class for reading input. Used when reading arguments for commands
# Commands
initialize all commands and provide method to execute a command from a string
# Error
a simple error class
# FileStorage
a class for basic file storage
# Logs
manage logs stored in the file
# PersistentSet
behave like std::set but provide persistence
# PersistentMap
a wrapper of PersistentSet, behave like std::map but provide persistence
# PersistentVector
a simple vector that provide persistence
# Statuses
managing the login stack and their selected books
# StringReader
a class for reading input and split it into words with only space as delimiter
# Utils
some useful data structures and string manipulation functions