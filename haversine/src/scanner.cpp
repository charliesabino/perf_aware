#include "scanner.hpp"
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

Scanner::Scanner(std::string &path) {
  std::ifstream file(path);
  contents = std::vector(std::istreambuf_iterator<char>(file),
                         std::istreambuf_iterator<char>());
}

auto Scanner::scan() -> std::vector<Token> & {
  while (idx < contents.size()) {
    scan_token();
  }
  return tokens;
}

auto Scanner::scan_token() -> void {
  char c = advance();
  if (c == '[') {
    tokens.emplace_back(Token::LeftBracket);
  } else if (c == ']') {
    tokens.emplace_back(Token::RightBracket);
  } else if (c == '{') {
    tokens.emplace_back(Token::LeftBrace);
  } else if (c == '}') {
    tokens.emplace_back(Token::RightBrace);
  } else if (c == ':') {
    tokens.emplace_back(Token::Colon);
  } else if (c == ',') {
    tokens.emplace_back(Token::Comma);
  } else if (c == '"') {
    std::string s;
    while ((c = advance()) != '"') {
      s.push_back(c);
    }
    tokens.emplace_back(Token::String, s);
  } else if (c == 't' || c == 'f') {
    tokens.emplace_back(Token::Bool, c == 't');
    while (std::isalpha(peek())) {
      advance();
    }
  } else if (std::isdigit(c)) {
    std::string digits{c};
    bool is_float = false;
    while (std::isdigit(peek()) || peek() == '.') {
      is_float = peek() == '.' || is_float;
      digits.push_back(advance());
    }
    if (is_float) {
      tokens.emplace_back(Token::Double, std::stod(digits));
    } else {
      tokens.emplace_back(Token::Int, std::stoi(digits));
    }
  }
}
