#include "array.h"
#include <cassert>
#include <sstream>
#include <vector>
#include "types.h"
#include "value.h"

namespace json {
struct Array::ArrayPrivate {
  std::vector<Value> elements;
};

Array::Array() : p{std::make_unique<Array::ArrayPrivate>()} {}
Array::~Array() = default;

Array::Array(Array&& object) {
  std::swap(object.p, p);
}
Array& Array::operator=(const Array& other) {
  p->elements = other.p->elements;
  return *this;
}

Array::Array(const Array& object) : p{std::make_unique<Array::ArrayPrivate>()} {
  p->elements = object.p->elements;
}

Array& Array::operator=(Array&& object) {
  std::swap(object.p, p);
  return *this;
}

bool Array::begin_with(StringStream& in) const {
  return in.peek() == '[';
}

Error Array::Parse(StringStream& in) {
  assert(begin_with(in) && "should call Parse if sure that its an Array");

  in.ignore();  // consume  '['

  while (true) {
    whitespace(in);

    auto next_char = in.peek();

    if (next_char == EOF) {
      return {Error::UnfinishedArray, in.Pos()};
    }

    if (next_char == ']') {
      in.ignore();  // consume '}'
      break;        // object parse happy end
    }

    if (p->elements.size()) {  // no comma for the first member to parse
      if (next_char != ',') {
        return {Error::ArrayCommaExpected, in.Pos()};
      }
      in.ignore();  // consume ','
      whitespace(in);
    }

    Value value;

    if (auto error = value.Parse(in); error) {
      return error;
    }
    p->elements.emplace_back(std::move(value));
  }

  return {};
}

int Array::size() const {
  return p->elements.size();
}

const Value& Array::at(int index) const {
  return p->elements.at(index);
}

}  // namespace json