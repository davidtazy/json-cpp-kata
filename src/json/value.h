#pragma once
#include <variant>
#include "json/object.h"
#include "json/types.h"

namespace json {
struct Value {
  using Variant = std::variant<Undefined, Null, True, False, String, Number, Object>;

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

  bool IsObject() const { return std::holds_alternative<Object>(variant); }

  const Object& ToObject() const { return std::get<Object>(variant); }

  Error Parse(StringStream& in);

 private:
  Variant variant{Undefined{}};
};

}  // namespace json