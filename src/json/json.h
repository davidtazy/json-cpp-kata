#pragma once
#include <cassert>
#include <sstream>
#include <variant>
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
        break;
      }
      value += in.get();
    }
    Escape();

    return {};
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

struct Object {
  bool begin_with(StringStream& in) const { return in.peek() == '{'; }

  Error Parse(StringStream& in) {
    assert(begin_with(in) && "should call Parse if sure that its an object");
    in.ignore();
  }
};

struct Value {
  using Variant = std::variant<Undefined, Null, True, False, String, Number>;

  Value() = default;

  explicit Value(bool value) {
    if (value) {
      variant = True{};
    } else {
      variant = False{};
    }
  }

  explicit Value(std::string value) : variant{String{value}} {}

  explicit Value(nullptr_t t) : variant(Null{}) {}

  operator bool() const { return !IsUndefined(); }

  bool IsNull() const { return std::holds_alternative<Null>(variant); }
  bool IsUndefined() const { return std::holds_alternative<Undefined>(variant); }
  bool IsTrue() const { return std::holds_alternative<True>(variant); }
  bool IsFalse() const { return std::holds_alternative<False>(variant); }
  bool IsBool() const { return IsTrue() || IsFalse(); }
  bool ToBool() const {
    if (IsFalse())
      return false;
    if (IsTrue())
      return true;
    throw std::runtime_error("ToBool: value is not bool");
  }

  bool IsString() const { return std::holds_alternative<String>(variant); }

  const std::string& ToString() const { return std::get<String>(variant).value; }

  bool IsNumber() const { return std::holds_alternative<Number>(variant); }
  double ToNumber() const { return std::get<Number>(variant).value; }
  Error Parse(StringStream& in) {
    whitespace(in);

    if (auto v = True{}; v.begin_with(in)) {
      variant = v;
      return v.Parse(in);
    }
    if (auto v = False{}; v.begin_with(in)) {
      variant = v;
      return v.Parse(in);
    }
    if (auto v = Null{}; v.begin_with(in)) {
      variant = v;
      return v.Parse(in);
    }

    if (auto v = String{}; v.begin_with(in)) {
      auto error = v.Parse(in);
      variant = v;
      return error;
    }
    if (auto v = Number{}; v.begin_with(in)) {
      auto error = v.Parse(in);
      variant = v;
      return error;
    }

    variant = Undefined{};
    return {};
  }

 private:
  Variant variant{Undefined{}};
};

}  // namespace json