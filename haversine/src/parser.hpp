
#include "scanner.hpp"
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

struct JsonValue;
using JsonObject = std::unordered_map<std::string, JsonValue>;

struct JsonValue : std::variant<std::nullptr_t, bool, double, std::string,
                                std::vector<JsonValue>, JsonObject> {
  using variant::variant;
  std::variant<std::nullptr_t, bool, double, std::string,
               std::vector<JsonValue>, JsonObject>
      value;
};

class Parser {
  u_int32_t start{};
  u_int32_t current{};

public:
  Parser() = default;
  auto parse(std::vector<Token> &tokens) -> JsonObject;
};
