#pragma once
#include "chat-completions-query.hpp"

class LocalLlm
{
public:
  auto chatCompletions(ChatCompletionsQuery query,
                       int nPredict = 2048,
                       float temperature = 0.7f,
                       std::vector<std::string> stopTokens = {}) -> std::string;
};
