
#include "parser.hpp"
#include <algorithm>
#include <string>
#include <utility>

auto Parser::parse(std::vector<Token> &tokens) -> JsonObject {
  start = current;
  JsonObject obj{};
  current++;
  std::string key{std::move(std::get<std::string>(tokens[current].value))};
  switch (tokens[current].type) {
  case Token::LeftBracket:
    break;
  case Token::LeftBrace:
    break;
  }
}
