#include "error.h"

namespace json {

std::string Error::to_string() const {
  static const std::map<Code, std::string> tstr{
      {NoError, "no error"},
      {FileNotExists, "file not exists"},
      {FileCannotBeOpened, "file cannot be opened"},
      {LiteralElementParse, "literal element parse failed"}

  };
  std::string s;
  if (message.size()) {
    using namespace std::literals::string_literals;
    s = ": "s + message;
  }
  return tstr.at(code) + s;
}

std::ostream& operator<<(std::ostream& out, const Error& error) {
  return out << error.to_string();
}

}  // namespace json