#include "error.h"

namespace json {

std::string Error::to_string() const {
  static const std::map<Code, std::string> tstr{
      {NoError, "no error"},
      {FileNotExists, "file not exists"},
      {FileCannotBeOpened, "file cannot be opened"},
      {LiteralElementParse, "literal element parse failed"},
      {UnfinishedString, "unfinished string"},
      {MultiLineString, "multi string"},
      {UnfinishedObject, "unfinished object"},
      {ObjectMemberExpected, "object member expected"},
      {ObjectSemiColonExpected, " object semi column expected"},
      {ObjectCommaExpected, " object comma expected"},
  };
  std::string s;
  if (message.size()) {
    using namespace std::literals::string_literals;
    s = ": "s + message;
  }
  std::ostringstream out;
  out << tstr.at(code) << pos << s;
  return out.str();
}

std::ostream& operator<<(std::ostream& out, const Error& error) {
  return out << error.to_string();
}

}  // namespace json