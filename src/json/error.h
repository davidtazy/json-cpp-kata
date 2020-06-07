#pragma once
#include <map>
#include <sstream>
namespace json {

struct Error {
  enum Code { NoError, FileNotExists, FileCannotBeOpened, LiteralElementParse };
  Code code{NoError};
  std::string message;
  Error() = default;
  Error(Code code, std::string str = "") : code(code), message(str) {}
  Error(Code code, const std::ostringstream& str) : code(code), message(str.str()) {}

  operator bool() const { return code != NoError; }

  std::string to_string() const;

  static Error make_ok() { return Error{}; }
  static Error make(Code code_p, const std::string msg = "") { return Error{code_p, msg}; }
};

std::ostream& operator<<(std::ostream& out, const Error& error);
}  // namespace json