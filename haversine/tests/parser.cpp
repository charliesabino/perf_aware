#include "../src/parser.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <vector>

class ParserTest : public ::testing::Test {
protected:
  void SetUp() override { test_filename = "test.json"; }

  void TearDown() override { std::remove(test_filename.c_str()); }

  void writeToFile(const std::string &content) {
    std::ofstream file(test_filename);
    file << content;
    file.close();
  }

  JsonObject parseJson(const std::string &content) {
    writeToFile(content);
    Scanner scanner(test_filename);
    auto tokens = scanner.scan();
    return parse(tokens);
  }

  std::string test_filename;
};

TEST_F(ParserTest, EmptyObject) {
  auto json = parseJson("{}");
  EXPECT_EQ(json.size(), 0);
}

TEST_F(ParserTest, SimpleString) {
  auto json = parseJson(R"({"key": "value"})");
  ASSERT_EQ(json.size(), 1);
  ASSERT_TRUE(std::holds_alternative<std::string>(json["key"]));
  EXPECT_EQ(std::get<std::string>(json["key"]), "value");
}

TEST_F(ParserTest, Numbers) {
  auto json = parseJson(R"({
        "integer": 42,
        "float": 3.14
    })");

  ASSERT_EQ(json.size(), 2);
  ASSERT_TRUE(std::holds_alternative<double>(json["integer"]));
  ASSERT_TRUE(std::holds_alternative<double>(json["float"]));
  EXPECT_EQ(std::get<double>(json["integer"]), 42.0);
  EXPECT_EQ(std::get<double>(json["float"]), 3.14);
}

TEST_F(ParserTest, Array) {
  auto json = parseJson(R"({
        "array": [1, 2, 3, "string", true]
    })");

  ASSERT_EQ(json.size(), 1);
  ASSERT_TRUE(std::holds_alternative<std::vector<JsonValue>>(json["array"]));

  auto &array = std::get<std::vector<JsonValue>>(json["array"]);
  ASSERT_EQ(array.size(), 5);

  EXPECT_EQ(std::get<int32_t>(array[0]), 1);
  EXPECT_EQ(std::get<int32_t>(array[1]), 2);
  EXPECT_EQ(std::get<int32_t>(array[2]), 3);
  EXPECT_EQ(std::get<std::string>(array[3]), "string");
  EXPECT_TRUE(std::get<bool>(array[4]));
}

TEST_F(ParserTest, NestedObject) {
  auto json = parseJson(R"({
        "nested": {
            "key": "value",
            "number": 42
        }
    })");

  ASSERT_EQ(json.size(), 1);
  ASSERT_TRUE(std::holds_alternative<JsonObject>(json["nested"]));

  auto &nested = std::get<JsonObject>(json["nested"]);
  ASSERT_EQ(nested.size(), 2);
  EXPECT_EQ(std::get<std::string>(nested["key"]), "value");
  EXPECT_EQ(std::get<double>(nested["number"]), 42.0);
}

TEST_F(ParserTest, ComplexStructure) {
  auto json = parseJson(R"({
        "string": "hello",
        "number": 42,
        "float": 3.14,
        "boolean": true,
        "array": [1, "two", true],
        "object": {
            "nested": "value",
            "nested_array": [{"key": "value"}]
        }
    })");

  ASSERT_EQ(json.size(), 6);
  EXPECT_EQ(std::get<std::string>(json["string"]), "hello");
  EXPECT_EQ(std::get<double>(json["number"]), 42.0);
  EXPECT_EQ(std::get<double>(json["float"]), 3.14);
  EXPECT_TRUE(std::get<bool>(json["boolean"]));

  auto &array = std::get<std::vector<JsonValue>>(json["array"]);
  ASSERT_EQ(array.size(), 3);

  auto &object = std::get<JsonObject>(json["object"]);
  ASSERT_EQ(object.size(), 2);
  EXPECT_EQ(std::get<std::string>(object["nested"]), "value");
}

TEST_F(ParserTest, InvalidJson) {
  EXPECT_THROW(parseJson("{"), std::runtime_error);
  EXPECT_THROW(parseJson(R"({"key")"), std::runtime_error);
  EXPECT_THROW(parseJson(R"({"key": })"), std::runtime_error);
  EXPECT_THROW(parseJson(R"({42: "value"})"), std::runtime_error);
}

TEST_F(ParserTest, EmptyArray) {
  auto json = parseJson(R"({"array": []})");
  ASSERT_EQ(json.size(), 1);
  ASSERT_TRUE(std::holds_alternative<std::vector<JsonValue>>(json["array"]));
  EXPECT_EQ(std::get<std::vector<JsonValue>>(json["array"]).size(), 0);
}

TEST_F(ParserTest, NestedArrays) {
  auto json = parseJson(R"({
        "nested_arrays": [[1, 2], [3, 4]]
    })");

  ASSERT_EQ(json.size(), 1);
  auto &outer_array = std::get<std::vector<JsonValue>>(json["nested_arrays"]);
  ASSERT_EQ(outer_array.size(), 2);

  auto &first_inner = std::get<std::vector<JsonValue>>(outer_array[0]);
  auto &second_inner = std::get<std::vector<JsonValue>>(outer_array[1]);

  ASSERT_EQ(first_inner.size(), 2);
  ASSERT_EQ(second_inner.size(), 2);

  EXPECT_EQ(std::get<double>(first_inner[0]), 1.0);
  EXPECT_EQ(std::get<double>(first_inner[1]), 2.0);
  EXPECT_EQ(std::get<double>(second_inner[0]), 3.0);
  EXPECT_EQ(std::get<double>(second_inner[1]), 4.0);
}
