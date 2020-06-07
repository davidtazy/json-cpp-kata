#include "json.h"

namespace json {

std::stringstream& whitespace(std::stringstream& in) {
  while (std::isspace(static_cast<unsigned char>(in.peek())) != 0) {
    in.get();
  }

  return in;
}

}  // namespace json