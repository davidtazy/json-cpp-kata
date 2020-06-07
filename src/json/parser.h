#pragma once
#include <sstream>
#include <string>
#include <utility>
#include "json/error.h"
#include "json/json.h"
namespace json {

struct File {
  std::string filename;

  explicit File(std::string filename_p) : filename(filename_p) {}

  bool Exists() const;
};

std::pair<Value, Error> Parse(File file);

std::pair<Value, Error> Parse(std::string str);

std::pair<Value, Error> Parse(std::stringstream&& str);

std::pair<std::stringstream, Error> ToStringStream(File file);

}  // namespace json