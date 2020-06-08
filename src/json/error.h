#pragma once
#include <map>
#include <sstream>
#include "json/string_stream.h"
namespace json {

struct Error {
  enum Code {
    NoError,
    FileNotExists,
    FileCannotBeOpened,
    LiteralElementParse,
    UnfinishedString,
    MultiLineString,
  };
  Code code{NoError};
  Position pos{0, 0};
  std::string message;

  Error() = default;
  Error(Code code, const Position& pos, std::string str = "")
      : code(code), pos{pos}, message(str) {}
  Error(Code code, const Position& pos, const std::ostringstream& str)
      : code(code), pos{pos}, message(str.str()) {}

  operator bool() const { return code != NoError; }

  std::string to_string() const;
};

std::ostream& operator<<(std::ostream& out, const Error& error);
}  // namespace json