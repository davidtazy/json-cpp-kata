#include <json/error.h>
#include <json/parser.h>
#include <sys/stat.h>
#include <catch2/catch.hpp>
#include <fstream>
#include <sstream>
#include <string>

const std::string TestDir{UNITEST_DIR};
const std::string DataDir{TestDir + "/data"};
using namespace json;

TEST_CASE("should parse json from file") {
  auto [value, error] = Parse(File{DataDir + "/null.json"});

  REQUIRE(error == false);

  SECTION(" error if file not exists") {
    auto [value, error] = Parse(File{"not_exists.json"});
    REQUIRE(error);
    REQUIRE(error.code == Error::FileNotExists);
  }
}

TEST_CASE("should parse json from string") {
  std::string str = R"({})";
  auto [value, error] = Parse(str);
}

TEST_CASE("should have error system") {
  auto error = Error::make_ok();
  REQUIRE_FALSE(error);

  error = Error::make(Error::FileNotExists);
  REQUIRE(error);
  REQUIRE(error.code == Error::FileNotExists);
  std::stringstream stream;
  stream << error;
  REQUIRE(stream.str() == "file not exists");
  REQUIRE(error.to_string() == "file not exists");

  error = Error::make(Error::FileNotExists, "filename.json");
  REQUIRE(error.to_string() == "file not exists: filename.json");
}

TEST_CASE("should consume whitespaces: space, linefeed, carriage return, formfeed") {
  std::stringstream in("  \n\t\r\f   chars");

  whitespace(in);

  REQUIRE(in.peek() == 'c');
}

TEST_CASE("should read all file content") {
  auto [in_stream, error] = ToStringStream(File{DataDir + "/multiline.txt"});
  REQUIRE_FALSE(error);
  std::string a, b, c;
  in_stream >> a >> b >> c;
  REQUIRE(a == "aaa");
  REQUIRE(b == "bbb");
  REQUIRE(c == "ccc");
}

TEST_CASE("should parse Literal types") {
  SECTION("null value") {
    auto [value, error] = Parse("null");
    REQUIRE_FALSE(error);
    REQUIRE(value.IsNull());
  }
  SECTION("true value") {
    auto [value, error] = Parse("true");
    REQUIRE_FALSE(error);
    REQUIRE(value.IsTrue());
  }
  SECTION("false value") {
    auto [value, error] = Parse("false");
    REQUIRE_FALSE(error);
    REQUIRE(value.IsFalse());
  }

  SECTION("literal value preceded with spaces") {
    auto [value, error] = Parse("    false");
    REQUIRE_FALSE(error);
    REQUIRE(value.IsFalse());
  }
  SECTION("literal value suffixed with spaces") {
    auto [value, error] = Parse("false   ");
    REQUIRE_FALSE(error);
    REQUIRE(value.IsFalse());
  }

  SECTION("literal error value") {
    auto [value, error] = Parse("falsee");
    REQUIRE(error);
    REQUIRE(error.code == Error::LiteralElementParse);
    REQUIRE(error.to_string() == "literal element parse failed: read falsee instead of false");
  }
}
