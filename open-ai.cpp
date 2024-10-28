#include "open-ai.hpp"
#include <curlpp/curl.hpp>
#include <fstream>
#include <json-ser/json-ser.hpp>
#include <log/log.hpp>
#include <sstream>
#include <thread>

static auto toStr(Role r)
{
  switch (r)
  {
  case Role::user: return "user";
  case Role::system: return "system";
  case Role::assistant: return "assistant";
  }
}

namespace OpenAi
{
  struct Msg
  {
    std::string role;
    std::string content;
    SER_PROPS(role, content);
  };

  struct RspFmt
  {
    std::string type = "text";
    SER_PROPS(type);
  };

  struct ChatCompletionsReq
  {
    std::string model;
    std::vector<Msg> messages;
    float temperature = .75f;
    int max_tokens = 2048;
    float top_p = 1.f;
    float frequency_penalty = 0.0f;
    float presence_penalty = 0.0f;
    RspFmt response_format;
    SER_PROPS(model,
              messages,
              temperature,
              max_tokens,
              top_p,
              frequency_penalty,
              presence_penalty,
              response_format);
  };

  Client::Client(const char *apiKey) : apiKey(apiKey) {}

  auto Client::chatCompletions(std::string model, ChatCompletionsQuery query) -> ChatCompletionsRsp
  {
    auto req = ChatCompletionsReq{.model = model};
    if (true)
    {
      auto debug = std::ofstream("/tmp/debug-llm.txt");
      debug << "request:\n";
      for (const auto &m : query)
        debug << toStr(m.first) << ": `" << m.second << "`" << std::endl;
    }
    for (const auto &m : query)
    {
      auto &msg = req.messages.emplace_back();
      msg.role = toStr(m.first);
      msg.content = m.second;
    }

    const auto data = [&]() {
      auto ss = std::ostringstream{};
      jsonSer(ss, req);
      return ss.str();
    }();

    Curl curl;
    curl.setUrl("https://api.openai.com/v1/chat/completions");
    curl.setPostFields(data);
    std::string rsp;
    curl.setWriteFunc([&](const char *data, size_t sz) {
      rsp += std::string_view{data, sz};
      return sz;
    });
    curl.setHeaders({"Content-Type: application/json", "Authorization: Bearer " + std::string{apiKey}});

    if (const auto r = curl.perform(); r != CURLE_OK)
    {
      LOG("Error:", r);
      throw "Curl error";
    }

    ChatCompletionsRsp response;
    try
    {
      const auto tmp = json::Root{rsp};
      jsonDeser(tmp.root(), response);
    }
    catch (std::runtime_error &e)
    {
      LOG("Error parsing LLM response:", e.what());
      throw;
    }

    if (response.choices.empty())
    {
      LOG("Empty choices");
      LOG(rsp);
      throw "Empty choices";
    }

    if (true)
    {
      auto debug = std::ofstream("/tmp/debug-llm.txt", std::ios::app);
      debug << "response:\n";
      for (const auto &m : response.choices)
        debug << "`" << m.message.content << "`" << std::endl;
    }

    return response;
  }
} // namespace OpenAi
