#include "../src/scanner.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <string>

class ScannerTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Create a temporary test file
    test_filename = "test.json";
  }

  void TearDown() override {
    // Clean up the test file
    std::remove(test_filename.c_str());
  }

  void writeToFile(const std::string &content) {
    std::ofstream file(test_filename);
    file << content;
    file.close();
  }

  std::string test_filename;
};
auto print_tokens(const std::vector<Token> &tokens) -> void {
  for (const auto &token : tokens) {
    std::cout << "Token(";
    switch (token.type) {
    case Token::LeftBracket:
      std::cout << "LeftBracket";
      break;
    case Token::RightBracket:
      std::cout << "RightBracket";
      break;
    case Token::LeftBrace:
      std::cout << "LeftBrace";
      break;
    case Token::RightBrace:
      std::cout << "RightBrace";
      break;
    case Token::Comma:
      std::cout << "Comma";
      break;
    case Token::Colon:
      std::cout << "Colon";
      break;
    case Token::String:
      std::cout << "String, \"" << std::get<std::string>(token.value) << "\"";
      break;
    case Token::Double:
      std::cout << "Double, " << std::get<double>(token.value);
      break;
    case Token::Int:
      std::cout << "Int, " << std::get<int32_t>(token.value);
      break;
    case Token::Bool:
      std::cout << "Bool, " << (std::get<bool>(token.value) ? "true" : "false");
      break;
    case Token::Null:
      std::cout << "Null";
      break;
    case Token::EOF:
      std::cout << "EOF";
      break;
    }
    std::cout << ")\n";
  }
}

TEST_F(ScannerTest, EmptyObject) {
  writeToFile("{}");
  Scanner scanner(test_filename);
  auto tokens = scanner.scan();

  ASSERT_EQ(tokens.size(), 2);
  EXPECT_EQ(tokens[0].type, Token::LeftBrace);
  EXPECT_EQ(tokens[1].type, Token::RightBrace);
}

TEST_F(ScannerTest, EmptyArray) {
  writeToFile("[]");
  Scanner scanner(test_filename);
  auto tokens = scanner.scan();

  ASSERT_EQ(tokens.size(), 2);
  EXPECT_EQ(tokens[0].type, Token::LeftBracket);
  EXPECT_EQ(tokens[1].type, Token::RightBracket);
}

TEST_F(ScannerTest, SimpleString) {
  writeToFile(R"({"key": "value"})");
  Scanner scanner(test_filename);
  auto tokens = scanner.scan();

  ASSERT_EQ(tokens.size(), 5);
  EXPECT_EQ(tokens[0].type, Token::LeftBrace);
  EXPECT_EQ(tokens[1].type, Token::String);
  EXPECT_EQ(std::get<std::string>(tokens[1].value), "key");
  EXPECT_EQ(tokens[2].type, Token::Colon);
  EXPECT_EQ(tokens[3].type, Token::String);
  EXPECT_EQ(std::get<std::string>(tokens[3].value), "value");
  EXPECT_EQ(tokens[4].type, Token::RightBrace);
}

TEST_F(ScannerTest, Numbers) {
  writeToFile(R"({"integer": 42, "float": 3.14})");
  Scanner scanner(test_filename);
  auto tokens = scanner.scan();

  ASSERT_EQ(tokens.size(), 9);
  EXPECT_EQ(tokens[3].type, Token::Int);
  EXPECT_EQ(std::get<int32_t>(tokens[3].value), 42);
  EXPECT_EQ(tokens[7].type, Token::Double);
  EXPECT_EQ(std::get<double>(tokens[7].value), 3.14);
}

TEST_F(ScannerTest, BooleanValues) {
  writeToFile(R"({"t": true, "f": false})");
  Scanner scanner(test_filename);
  auto tokens = scanner.scan();

  ASSERT_EQ(tokens.size(), 9);
  EXPECT_EQ(tokens[3].type, Token::Bool);
  EXPECT_EQ(std::get<bool>(tokens[3].value), true);
  EXPECT_EQ(tokens[7].type, Token::Bool);
  EXPECT_EQ(std::get<bool>(tokens[7].value), false);
}

TEST_F(ScannerTest, NestedStructures) {
  writeToFile(R"({"array": [1, 2, {"nested": true}]})");
  Scanner scanner(test_filename);
  auto tokens = scanner.scan();

  ASSERT_GT(tokens.size(), 0);
  EXPECT_EQ(tokens[0].type, Token::LeftBrace);
  EXPECT_EQ(tokens[1].type, Token::String);
  EXPECT_EQ(std::get<std::string>(tokens[1].value), "array");
  EXPECT_EQ(tokens[3].type, Token::LeftBracket);
}

TEST_F(ScannerTest, ComplexObject) {
  writeToFile(R"({
        "string": "hello",
        "number": 42,
        "float": 3.14,
        "boolean": true,
        "array": [1, 2, 3],
        "object": {"nested": "value"}
    })");
  Scanner scanner(test_filename);
  auto tokens = scanner.scan();

  ASSERT_GT(tokens.size(), 0);
  // Verify first few tokens
  EXPECT_EQ(tokens[0].type, Token::LeftBrace);
  EXPECT_EQ(tokens[1].type, Token::String);
  EXPECT_EQ(std::get<std::string>(tokens[1].value), "string");
}
