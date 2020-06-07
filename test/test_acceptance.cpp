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
  REQUIRE(error.to_string() == "file not exists");
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