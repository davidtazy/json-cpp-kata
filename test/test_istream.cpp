#include <catch2/catch.hpp>
#include <sstream>
#include <utility>

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

std::ostream& operator<<(std::ostream& out, const Position& pos) {
  return out << "line: " << pos.line << " column:" << pos.column;
}

struct StringStream {
  StringStream(const StringStream&) = delete;
  explicit StringStream(std::stringstream&& in_p) : in(std::move(in_p)) {}
  explicit StringStream(const std::string& str) : in(str) {}

  Position Pos() const { return pos; }

  int get() {
    auto c = in.get();

    if (c == '\r') {
      if (in.peek() == '\n') {  // deal with \r\n historical weirdness
        in.ignore();
      }
      pos.bump_line();
      return '\n';
    }
    if (c == '\n') {
      pos.bump_line();
      return c;
    }

    pos++;

    return c;
  }

  StringStream& ignore() {
    get();
    return *this;
  }

 private:
  std::stringstream in;
  Position pos;
};

}  // namespace json
using namespace json;

TEST_CASE("StringStream buildable from string or std::stringstream") {
  StringStream in{"aaa\nbbb\nccc\n"};

  std::stringstream stream("aaa\nbbb\nccc\n");

  StringStream in2{std::move(stream)};
}

TEST_CASE("StringStream maintain line and column during parse") {
  StringStream in{"a\nb\r\nc\rd"};

  REQUIRE(in.Pos() == Position(1, 1));

  // first line
  REQUIRE(in.get() == 'a');
  REQUIRE(in.Pos() == Position(1, 2));

  in.ignore();
  REQUIRE(in.Pos() == Position(2, 1));

  // second line
  REQUIRE(in.get() == 'b');
  REQUIRE(in.Pos() == Position(2, 2));

  in.ignore();  // \r\n count as one line
  REQUIRE(in.Pos() == Position(3, 1));

  // third line
  REQUIRE(in.get() == 'c');
  REQUIRE(in.Pos() == Position(3, 2));

  in.ignore();
  REQUIRE(in.Pos() == Position(4, 1));
}