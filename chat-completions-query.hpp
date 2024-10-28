#pragma once

#include <string>
#include <vector>

enum class Role { user, system, assistant };

using ChatCompletionsQuery = std::vector<std::pair<Role, std::string>>;
