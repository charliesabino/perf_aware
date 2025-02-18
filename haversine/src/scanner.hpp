#pragma once

#include <cstdint>
#include <string>
#include <sys/_types/_u_int32_t.h>
#include <variant>
#include <vector>
#undef EOF

struct Token {
  enum Type {
    LeftBracket,
    RightBracket,
    LeftBrace,
    RightBrace,
    Comma,
    Colon,
    String,
    Double,
    Int,
    Bool,
    Null,
    EOF,
  };
  Type type;
  std::variant<std::monostate, std::string, double, int32_t, bool> value{};
};

class Scanner {
  std::vector<char> contents{};
  std::vector<Token> tokens{};
  u_int32_t idx{};
  auto scan_token() -> void;
  auto advance() -> char { return contents[idx++]; }
  auto peek() const -> char { return contents[idx]; }

public:
  Scanner(std::string &path);
  auto scan() -> std::vector<Token> &;
};
