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
  auto error = Error{};
  REQUIRE_FALSE(error);

  error = Error{Error::FileNotExists, Position(0, 0)};
  REQUIRE(error);
  REQUIRE(error.code == Error::FileNotExists);
  std::ostringstream stream;
  stream << error;
  REQUIRE(stream.str() == "file not exists");
  REQUIRE(error.to_string() == "file not exists");

  error = Error{Error::FileNotExists, Position(0, 0), "filename.json"};
  REQUIRE(error.to_string() == "file not exists: filename.json");
}

TEST_CASE("should consume whitespaces: space, linefeed, carriage return, formfeed") {
  {
    StringStream in("  \n\t\r\f   chars");

    whitespace(in);

    REQUIRE(in.peek() == 'c');
  }
  {
    StringStream in("cd");

    whitespace(in);

    REQUIRE(in.peek() == 'c');
  }
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
    REQUIRE(error.to_string() == "literal element parse failed 1:7: read falsee instead of false");
  }
}

TEST_CASE("should parse string") {
  SECTION("simple string") {
    auto [value, error] = Parse(R"("string")");
    REQUIRE_FALSE(error);

    REQUIRE(value.IsString());
    REQUIRE(value.ToString() == "string");
  }
  SECTION("unfinished string is error") {
    auto [value, error] = Parse(R"("string)");
    REQUIRE(error);
    REQUIRE(error.code == Error::UnfinishedString);
  }
  SECTION("multiline string is error") {
    auto [value, error] = Parse(
        R"("multi
    line
    string")");
    REQUIRE(error);
    REQUIRE(error.code == Error::MultiLineString);
  }

  SECTION("escaped multiline string ") {
    auto [value, error] = Parse(R"("multi\nline\nstring",)");
    REQUIRE_FALSE(error);
    REQUIRE(value.IsString());
    REQUIRE(value.ToString() == "multi\nline\nstring");
  }

  SECTION("escaped quote string ") {
    auto [value, error] = Parse(R"("with\"quotes")");
    REQUIRE_FALSE(error);
    REQUIRE(value.IsString());
    REQUIRE(value.ToString() == R"(with"quotes)");
  }

  SECTION("escaped tab string ") {
    auto [value, error] = Parse(R"("with\tquotes")");
    REQUIRE_FALSE(error);
    REQUIRE(value.IsString());
    REQUIRE(value.ToString() == "with\tquotes");
  }
  SECTION("escaped antislash string ") {
    auto [value, error] = Parse(R"("with\\quotes")");
    REQUIRE_FALSE(error);
    REQUIRE(value.IsString());
    REQUIRE(value.ToString() == R"(with\quotes)");
  }
  SECTION("escaped slash string ") {
    auto [value, error] = Parse(R"("with\/quotes")");
    REQUIRE_FALSE(error);
    REQUIRE(value.IsString());
    REQUIRE(value.ToString() == R"(with/quotes)");
  }
  SECTION(R"(escaped \b \f  \r string )") {
    auto [value, error] = Parse(R"("with \b \f  \r quotes")");
    REQUIRE_FALSE(error);
    REQUIRE(value.IsString());
    REQUIRE(value.ToString() == "with \b \f  \r quotes");
  }

  SECTION("consume last \"") {
    auto [value, error] = Parse(R"("name")");
    REQUIRE_FALSE(error);
    REQUIRE(error.pos.column == 7);
  }
}

TEST_CASE("escaped unicode string ", "[.][not implemented]") {
  auto [value, error] = Parse(R"("with\u91ABquotes")");
  REQUIRE_FALSE(error);
  REQUIRE(value.IsString());
  REQUIRE(value.ToString() == "with\u91ABquotes");
}

TEST_CASE("should parse numbers") {
  SECTION("integer") {
    auto [value, error] = Parse("42");
    REQUIRE_FALSE(error);

    REQUIRE(value.IsNumber());
    REQUIRE(value.ToNumber() == 42);
  }
  SECTION("double") {
    auto [value, error] = Parse("42.42");
    REQUIRE_FALSE(error);

    REQUIRE(value.IsNumber());
    REQUIRE(value.ToNumber() == 42.42);
  }

  SECTION("f notation") {
    auto [value, error] = Parse("2.3e2");
    REQUIRE_FALSE(error);

    REQUIRE(value.IsNumber());
    REQUIRE(value.ToNumber() == 230);
  }

  SECTION("negative value") {
    auto [value, error] = Parse("-2.3e2");
    REQUIRE_FALSE(error);

    REQUIRE(value.IsNumber());
    REQUIRE(value.ToNumber() == -230);
  }
}

TEST_CASE("should parse object") {
  SECTION("should parse an empty object") {
    auto [value, error] = Parse("{}");
    REQUIRE_FALSE(error);
    REQUIRE(value.IsObject());
    REQUIRE(value.ToObject().empty());
  }
  SECTION("should report error if not starting with member name") {
    auto [value, error] = Parse("{ 123}");
    REQUIRE(error);
    REQUIRE(error.code == Error::ObjectMemberExpected);
  }

  SECTION("should parse object with 1 member") {
    auto [value, error] = Parse(R"__({"name":"value"})__");
    REQUIRE_FALSE(error);
    REQUIRE(value.IsObject());
    auto& obj = value.ToObject();
    REQUIRE(obj.contains("name"));
    REQUIRE(obj.at("name").IsString());
    REQUIRE(obj.at("name").ToString() == "value");
  }
}