#include "local-llm.hpp"
#include <curlpp/curl.hpp>
#include <fstream>
#include <json-ser/json-ser.hpp>
#include <json/json.hpp>
#include <log/log.hpp>
#include <ser/macro.hpp>

struct Payload
{
  int n_predict;
  float temperature;
  std::vector<std::string> stop;
  std::string prompt;
  SER_PROPS(n_predict, temperature, stop, prompt);
};

struct Rsp
{
  std::string content;
  SER_PROPS(content);
};

static auto toStr(Role r)
{
  switch (r)
  {
  case Role::user: return "user";
  case Role::system: return "system";
  case Role::assistant: return "assistant";
  }
}

auto LocalLlm::chatCompletions(ChatCompletionsQuery query,
                               int nPredict,
                               float temperature,
                               std::vector<std::string> stopTokens) -> std::string
{
  if (stopTokens.empty())
    stopTokens = {
      "<|eot_id|>",
      "<|eom_id|>",
    };
  auto prompt = std::string{"<|begin_of_text|>"};
  for (const auto &msg : query)
    prompt += std::string{"\n<|start_header_id|>"} + toStr(msg.first) + "<|end_header_id|>" +
              msg.second + "<|eot_id|>";
  prompt += "\n<|start_header_id|>assistant<|end_header_id|>";

  auto payload =
    Payload{.n_predict = nPredict, .temperature = temperature, .stop = stopTokens, .prompt = prompt};

  auto curl = Curl{};
  curl.setUrl("http://localhost:8080/completion");
  auto payloadStr = [payload]() {
    auto ss = std::ostringstream{};
    jsonSer(ss, payload);
    return ss.str();
  }();
  curl.setPostFields(payloadStr);
  if (true)
  {
    auto debug = std::ofstream("/tmp/debug-local-llm.txt");
    debug << "request:\n" << prompt << "\n";
  }
  auto rsp = std::string{};
  curl.setWriteFunc([&](const char *data, size_t sz) {
    rsp += std::string_view{data, sz};
    return sz;
  });
  curl.setHeaders({"Content-Type: application/json"});
  if (const auto r = curl.perform(); r != CURLE_OK)
  {
    LOG("Error:", r, curl_easy_strerror(r));
    LOG(payloadStr);
    throw std::runtime_error("Error quering local LLM");
  }

  auto jRsp = [&rsp]() {
    auto r = Rsp{};
    const auto tmp = json::Root{rsp};
    jsonDeser(tmp.root(), r);
    return r;
  }();
  if (true)
  {
    auto debug = std::ofstream("/tmp/debug-local-llm.txt", std::ios::app);
    debug << "response:\n" << jRsp.content << "\n";
  }
  return jRsp.content;
}
