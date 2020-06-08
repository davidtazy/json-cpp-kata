#pragma once

#include <cassert>
#include <memory>
#include "json/error.h"
#include "json/string_stream.h"

namespace json {
struct Value;
struct Array {
  Array();
  ~Array();
  Array(const Array& object);
  Array& operator=(const Array& other);

  Array(Array&& object);
  Array& operator=(Array&& object);

  bool begin_with(StringStream& in) const;

  Error Parse(StringStream& in);

  int size() const;

  const Value& at(int index) const;

 private:
  struct ArrayPrivate;
  std::unique_ptr<ArrayPrivate> p;
};

}  // namespace json
