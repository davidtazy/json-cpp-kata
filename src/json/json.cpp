#include "json.h"

static std::string unescapeJSON(const std::string& input);

namespace json {

StringStream& whitespace(StringStream& in) {
  while (std::isspace(static_cast<unsigned char>(in.peek())) != 0) {
    in.get();
  }

  return in;
}

void String::Escape() {
  using namespace std::literals::string_literals;

  value = unescapeJSON(value);
}

}  // namespace json

// https://github.com/ATNoG/JSONObject/blob/nativeCpp/StringUtils.cpp
std::string unescapeJSON(const std::string& input) {
  enum State { ESCAPED, UNESCAPED };
  State s = UNESCAPED;
  std::string output;
  output.reserve(input.length());

  for (std::string::size_type i = 0; i < input.length(); ++i) {
    switch (s) {
      case ESCAPED: {
        switch (input[i]) {
          case '"':
            output += '\"';
            break;
          case '/':
            output += '/';
            break;
          case 'b':
            output += '\b';
            break;
          case 'f':
            output += '\f';
            break;
          case 'n':
            output += '\n';
            break;
          case 'r':
            output += '\r';
            break;
          case 't':
            output += '\t';
            break;
          case '\\':
            output += '\\';
            break;
          default:
            output += input[i];
            break;
        }

        s = UNESCAPED;
        break;
      }
      case UNESCAPED: {
        switch (input[i]) {
          case '\\':
            s = ESCAPED;
            break;
          default:
            output += input[i];
            break;
        }
      }
    }
  }
  return output;
}