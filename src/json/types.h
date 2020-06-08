#pragma once
#include <cassert>
#include <map>
#include <sstream>
#include "json/error.h"
#include "json/string_stream.h"
namespace json {

StringStream& whitespace(StringStream& in);

struct Literal {
  std::string id{""};

  bool begin_with(StringStream& in) const { return in.peek() == id.at(0); }

  Error Parse(StringStream& in) {
    std::string n;
    in >> n;
    if (n != id) {
      std::ostringstream msg;
      msg << "read " << n << " instead of " << id;
      return {Error::LiteralElementParse, in.Pos(), msg};
    }
    return {};
  }

 protected:
  Literal(std::string id) : id(id){};
};

struct Null : public Literal {
  Null() : Literal{"null"} {}
};

struct True : public Literal {
  True() : Literal{"true"} {}
};
struct False : public Literal {
  False() : Literal{"false"} {}
};
struct Undefined {};

struct String {
  std::string value;

  bool begin_with(StringStream& in) const { return in.peek() == '"'; }

  Error Parse(StringStream& in) {
    assert(begin_with(in) && "should call Parse if sure that its a string");

    value.clear();

    in.ignore();
    for (int c = in.peek();; c = in.peek()) {
      if (c == '\n' || c == '\r') {
        return {Error::MultiLineString, in.Pos()};
      }
      if (c == EOF) {
        return {Error::UnfinishedString, in.Pos()};
      }
      if (c == '"' && value.back() != '\\') {
        in.ignore();  // consume last ""
        break;
      }
      value += in.get();
    }
    Escape();

    return {Error::NoError, in.Pos()};
  }

  void Escape();
};

struct Number {
  double value{};
  bool begin_with(StringStream& in) const {
    std::string id{"0123456789-"};
    return id.find(static_cast<char>(in.peek())) != std::string::npos;
  }

  Error Parse(StringStream& in) {
    in >> value;
    return {};
  }
};

}  // namespace json
