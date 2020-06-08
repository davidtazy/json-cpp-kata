#pragma once

#include <cassert>
#include <memory>
#include "json/error.h"
#include "json/string_stream.h"

namespace json {
struct Value;
struct Object {
  Object();
  ~Object();
  Object(const Object& object);
  Object& operator=(const Object& object);

  Object(Object&& object);
  Object& operator=(Object&& object);

  bool begin_with(StringStream& in) const;

  Error Parse(StringStream& in);

  bool empty() const;
  bool contains(const std::string& name) const;
  const Value& at(const std::string& name) const;

 private:
  struct ObjectPrivate;
  std::unique_ptr<ObjectPrivate> p;
};

}  // namespace json
