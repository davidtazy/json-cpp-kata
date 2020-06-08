#include "object.h"
#include <cassert>
#include <map>
#include <sstream>
#include "types.h"
#include "value.h"

namespace json {
struct Object::ObjectPrivate {
  std::map<std::string, Value> elements;
};

Object::Object() : p{std::make_unique<Object::ObjectPrivate>()} {}
Object::~Object() = default;

Object::Object(Object&& object) {
  std::swap(object.p, p);
}

Object::Object(const Object& object) : p{std::make_unique<Object::ObjectPrivate>()} {
  p->elements = object.p->elements;
}
Object& Object::operator=(const Object& object) {
  p->elements = object.p->elements;
  return *this;
}

Object& Object::operator=(Object&& object) {
  std::swap(object.p, p);
  return *this;
}

bool Object::begin_with(StringStream& in) const {
  return in.peek() == '{';
}

Error Object::Parse(StringStream& in) {
  assert(begin_with(in) && "should call Parse if sure that its an object");

  in.ignore();  // consume  '{'

  while (true) {
    whitespace(in);

    auto next_char = in.peek();

    if (next_char == EOF) {
      return {Error::UnfinishedObject, in.Pos()};
    }

    if (next_char == '}') {
      in.ignore();  // consume '}'
      break;        // object parse happy end
    }

    if (p->elements.size()) {  // no comma for the first member to parse
      if (next_char != ',') {
        return {Error::ObjectCommaExpected, in.Pos()};
      }
      in.ignore();  // consume ','
      whitespace(in);
    }

    String name;
    if (!name.begin_with(in)) {
      return {Error::ObjectMemberExpected, in.Pos()};
    }

    if (auto error = name.Parse(in); error) {
      return error;
    }

    whitespace(in);

    if (in.peek() != ':') {
      std::ostringstream out;
      out << "read " << static_cast<char>(in.peek());
      return {Error::ObjectSemiColonExpected, in.Pos(), out};
    }
    in.ignore();
    whitespace(in);
    if (auto error = p->elements[name.value].Parse(in); error) {
      return error;
    }
  }

  return {};
}

bool Object::empty() const {
  return p->elements.empty();
}
bool Object::contains(const std::string& name) const {
  return p->elements.find(name) != p->elements.end();
}

const Value& Object::at(const std::string& name) const {
  return p->elements.at(name);
}

}  // namespace json