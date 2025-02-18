
#include "parser.hpp"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>

auto parse(std::vector<Token> &tokens, int start) -> std::pair<int, JsonValue> {
  if (start >= tokens.size()) {
    throw std::runtime_error("Unexpected end of input");
  }

  int current = start;
  const auto &token = tokens[current];

  switch (token.type) {
  case Token::String:
    return {current + 1, std::get<std::string>(token.value)};

  case Token::Double:
    return {current + 1, std::get<double>(token.value)};

  case Token::Int:
    return {current + 1, static_cast<double>(std::get<int32_t>(token.value))};

  case Token::Bool:
    return {current + 1, std::get<bool>(token.value)};

  case Token::Null:
    return {current + 1, nullptr};

  case Token::LeftBracket: {
    std::vector<JsonValue> array;
    current++;

    while (current < tokens.size() &&
           tokens[current].type != Token::RightBracket) {
      auto [new_current, value] = parse(tokens, current);
      array.push_back(value);
      current = new_current;

      if (current < tokens.size() && tokens[current].type == Token::Comma) {
        current++;
      } else if (tokens[current].type != Token::RightBracket) {
        throw std::runtime_error("Expected ',' or ']'");
      }
    }

    if (current >= tokens.size() ||
        tokens[current].type != Token::RightBracket) {
      throw std::runtime_error("Expected ']'");
    }
    return {current + 1, array};
  }

  case Token::LeftBrace: {
    JsonObject object;
    current++;

    while (current < tokens.size() &&
           tokens[current].type != Token::RightBrace) {
      if (tokens[current].type != Token::String) {
        throw std::runtime_error("Expected string key");
      }
      std::string key = std::get<std::string>(tokens[current].value);
      current++;

      if (current >= tokens.size() || tokens[current].type != Token::Colon) {
        throw std::runtime_error("Expected ':'");
      }
      current++;

      auto [new_current, value] = parse(tokens, current);
      object[key] = value;
      current = new_current;

      if (current < tokens.size() && tokens[current].type == Token::Comma) {
        current++;
      } else if (tokens[current].type != Token::RightBrace) {
        throw std::runtime_error("Expected ',' or '}'");
      }
    }

    if (current >= tokens.size() || tokens[current].type != Token::RightBrace) {
      throw std::runtime_error("Expected '}'");
    }
    return {current + 1, object};
  }

  default:
    throw std::runtime_error("Unexpected token");
  }
}

auto parse(std::vector<Token> &tokens) -> JsonObject {
  if (tokens.empty()) {
    return JsonObject{};
  }

  auto [_, value] = parse(tokens, 0);
  if (!std::holds_alternative<JsonObject>(value)) {
    throw std::runtime_error("Top-level JSON must be an object");
  }
  return std::get<JsonObject>(value);
}
