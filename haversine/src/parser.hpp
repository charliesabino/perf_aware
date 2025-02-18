
#include "scanner.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

struct JsonValue;
using JsonObject = std::unordered_map<std::string, JsonValue>;

struct JsonValue
    : std::variant<std::nullptr_t, bool, double, int32_t, std::string,
                   std::vector<JsonValue>, JsonObject> {
  using variant::variant;
  std::variant<std::nullptr_t, bool, double, int32_t, std::string,
               std::vector<JsonValue>, JsonObject>
      value;
};
auto parse(std::vector<Token> &tokens) -> JsonObject;
