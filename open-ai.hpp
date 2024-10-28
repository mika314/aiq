#pragma once
#include "chat-completions-query.hpp"
#include <ser/macro.hpp>
#include <string>
#include <vector>

namespace OpenAi
{
  class Client
  {
  public:
    Client(const char *apiKey);
    struct Message
    {
      std::string content;
      SER_PROPS(content);
    };
    struct Choice
    {
      Message message;
      SER_PROPS(message);
    };
    struct ChatCompletionsRsp
    {
      std::vector<Choice> choices;
      SER_PROPS(choices);
    };
    auto chatCompletions(std::string model, ChatCompletionsQuery query) -> ChatCompletionsRsp;

  private:
    std::string apiKey;
  };
} // namespace OpenAi
