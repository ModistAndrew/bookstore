#ifndef CODE_ERROR_HPP
#define CODE_ERROR_HPP

#include <string>
#include <exception>
#include <utility>

/*
 * Class: ErrorException
 * ---------------------
 * This exception is thrown by calls to the <code>error</code>
 * function.  Typical code for catching errors looks like this:
 *
 *<pre>
 *    try {
 *       ... code in which an error might occur ...
 *    } catch (ErrorException & ex) {
 *       ... code to handle the error condition ...
 *    }
 *</pre>
 *
 * If an <code>ErrorException</code> is thrown at any point in the
 * range of the <code>try</code> (including in functions called from
 * that code), control will jump immediately to the error handler.
 */

class ErrorException : public std::exception {
public:
  explicit ErrorException(std::string message) : message(std::move(message)) {
  }

  [[nodiscard]] std::string getMessage() const {
    return message;
  }

private:
  std::string message;
};

//------------------------------------------------------------------------------------------------

void error(const std::string &message) {
  throw ErrorException(message);
}

#endif //CODE_ERROR_HPP
