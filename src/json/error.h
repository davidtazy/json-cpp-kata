#pragma once
#include <map>
namespace json {

struct Error {
  enum Code { NoError, FileNotExists, FileCannotBeOpened };
  Code code{NoError};

  operator bool() const { return code != NoError; }

  std::string to_string() const;

  static Error make_ok() { return Error{}; }
  static Error make(Code code_p) { return Error{code_p}; }
};

std::ostream& operator<<(std::ostream& out, const Error& error);
}  // namespace json