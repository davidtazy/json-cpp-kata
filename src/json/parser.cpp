#include "parser.h"
#include <sys/stat.h>
#include <fstream>
#include "json/string_stream.h"

namespace json {

bool File::Exists() const {
  struct stat fileInfo;
  return stat(filename.c_str(), &fileInfo) == 0;
}

std::pair<std::stringstream, Error> ToStringStream(File file) {
  std::stringstream buffer;
  if (!file.Exists()) {
    return {std::move(buffer), {Error::FileNotExists, Position{0, 0}}};
  }
  std::ifstream in(file.filename, std::ifstream::in);

  if (in.fail()) {
    return {std::move(buffer), {Error::FileCannotBeOpened, Position{0, 0}}};
  }

  buffer << in.rdbuf();

  return std::pair{std::move(buffer), Error{}};
}

std::pair<Value, Error> Parse(File file) {
  auto [stream, error] = ToStringStream(file);
  if (error) {
    return {{}, error};
  }

  return Parse(std::move(stream));
}

std::pair<Value, Error> Parse(std::string str) {
  std::stringstream stream(str);
  return Parse(std::move(stream));
}

std::pair<Value, Error> Parse(std::stringstream&& str) {
  StringStream in{std::move(str)};

  Value value;
  auto error = value.Parse(in);

  return std::pair(std::move(value), error);
}

}  // namespace json