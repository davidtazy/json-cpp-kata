#include "value.h"

namespace json {

Error Value::Parse(StringStream& in) {
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

  if (auto v = Object{}; v.begin_with(in)) {
    auto error = v.Parse(in);
    variant = std::move(v);
    return error;
  }

  variant = Undefined{};
  return {};
}

}  // namespace json