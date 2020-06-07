#pragma once
#include <sstream>
#include <variant>
#include "json/error.h"
namespace json {

struct Literal {
  std::string id{""};

  bool begin_with(std::stringstream& in) const { return in.peek() == id.at(0); }

  Error Parse(std::stringstream& in) {
    std::string n;
    in >> n;
    if (n != id) {
      std::stringstream msg;
      msg << "read " << n << " instead of " << id;
      return {Error::LiteralElementParse, msg};
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

struct Value {
  using Variant = std::variant<Undefined, Null, True, False>;

  Value() = default;

  explicit Value(bool value) {
    if (value) {
      variant = True{};
    } else {
      variant = False{};
    }
  }

  explicit Value(nullptr_t t) : variant(Null{}) {}

  operator bool() const { return !IsUndefined(); }

  bool IsNull() const { return std::holds_alternative<Null>(variant); }
  bool IsUndefined() const { return std::holds_alternative<Undefined>(variant); }
  bool IsTrue() const { return std::holds_alternative<True>(variant); }
  bool IsFalse() const { return std::holds_alternative<False>(variant); }
  bool IsBool() const { return IsTrue() || IsFalse(); }

  Error Parse(std::stringstream& in) {
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

    variant = Undefined{};
    return {};
  }

 private:
  Variant variant{Undefined{}};
};

}  // namespace json