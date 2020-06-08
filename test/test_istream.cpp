#include <json/string_stream.h>
#include <catch2/catch.hpp>
#include <sstream>
#include <utility>
using namespace json;

TEST_CASE("StringStream buildable from string or std::stringstream") {
  StringStream in{"aaa\nbbb\nccc\n"};

  std::stringstream stream("aaa\nbbb\nccc\n");

  StringStream in2{std::move(stream)};
}

TEST_CASE("StringStream maintain line and column during parse") {
  {
    StringStream in{"a\nb\r\nc\rdef\n"};

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

    INFO(Position(4, 1));

    std::string last_string;
    in >> last_string;
    REQUIRE(last_string == "def");
    REQUIRE(in.Pos() == Position(4, 4));
  }
  {
    StringStream in{"1.1 1 1.2e3 -3 -3.2\n"};
    double number;

    in >> number;
    REQUIRE(number == 1.1);
    REQUIRE(in.Pos() == Position(1, 4));
    in.ignore();

    in >> number;
    REQUIRE(number == 1);
    REQUIRE(in.Pos() == Position(1, 6));
    in.ignore();

    in >> number;
    REQUIRE(number == 1.2e3);
    REQUIRE(in.Pos() == Position(1, 12));
    in.ignore();

    in >> number;
    REQUIRE(number == -3);
    REQUIRE(in.Pos() == Position(1, 15));
    in.ignore();

    in >> number;
    REQUIRE(number == -3.2);
    REQUIRE(in.Pos() == Position(1, 20));
    in.ignore();
  }
}