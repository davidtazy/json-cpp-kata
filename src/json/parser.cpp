#include "parser.h"
#include <sys/stat.h>
#include <fstream>

namespace json {

bool File::Exists() const {
  struct stat fileInfo;
  return stat(filename.c_str(), &fileInfo) == 0;
}

std::pair<Value, Error> Parse(std::stringstream&& str) {
  std::stringstream in{std::move(str)};
  return std::pair(Value{}, Error{});
}

std::pair<std::stringstream, Error> ToStringStream(File file) {
  std::stringstream buffer;
  if (!file.Exists()) {
    return {std::move(buffer), {Error::FileNotExists}};
  }
  std::ifstream in(file.filename, std::ifstream::in);

  if (in.fail()) {
    return {std::move(buffer), {Error::FileCannotBeOpened}};
  }

  buffer << in.rdbuf();

  return std::pair{std::move(buffer), Error::make_ok()};
}

std::pair<Value, Error> Parse(File file) {
  auto [stream, error] = ToStringStream(file);
  if (error) {
    return {{}, error};
  }

  return Parse(std::move(stream));
}

std::pair<Value, Error> Parse(std::string str) {
  return std::pair(Value{}, Error{});
}

}  // namespace json