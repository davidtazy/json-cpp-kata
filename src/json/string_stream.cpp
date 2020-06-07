#include "string_stream.h"

namespace json {

std::ostream& operator<<(std::ostream& out, const Position& pos) {
  return out << "line: " << pos.line << " column:" << pos.column;
}
int StringStream::get() {
  auto c = in.get();

  if (c == '\r') {
    if (in.peek() == '\n') {  // deal with \r\n historical weirdness
      in.ignore();
    }
    pos.bump_line();
    return '\n';
  }
  if (c == '\n') {
    pos.bump_line();
    return c;
  }

  pos++;

  return c;
}

StringStream& StringStream::operator>>(std::string& str) {
  in >> str;
  pos.column += str.size();
  return *this;
}

}  // namespace json