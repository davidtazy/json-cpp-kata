#pragma once
#include <sstream>

namespace json {

struct Position {
  int line{1};
  int column{1};

  Position() = default;
  Position(int line, int column) : line(line), column(column) {}
  bool operator==(const Position& pos) const { return line == pos.line && column == pos.column; }

  void bump_line() {
    line++;
    column = 1;
  }
  Position operator++(int) {
    auto temp = *this;
    ++*this;
    return temp;
  }
  Position& operator++() {
    column++;
    return *this;
  }
};

std::ostream& operator<<(std::ostream& out, const Position& pos);

struct StringStream {
  StringStream(const StringStream&) = delete;
  explicit StringStream(std::stringstream&& in_p) : in(std::move(in_p)) {}
  explicit StringStream(const std::string& str) : in(str) {}

  Position Pos() const { return pos; }

  int get();

  StringStream& operator>>(std::string& str);

  int peek() { return in.peek(); }

  StringStream& ignore() {
    get();
    return *this;
  }

 private:
  std::stringstream in;
  Position pos;
};

}  // namespace json
