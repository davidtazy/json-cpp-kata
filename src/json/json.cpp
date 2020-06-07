#include "json.h"

namespace json {

StringStream& whitespace(StringStream& in) {
  while (std::isspace(static_cast<unsigned char>(in.peek())) != 0) {
    in.get();
  }

  return in;
}

}  // namespace json