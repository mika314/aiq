#include "curl.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <json-ser/json-ser.hpp>
#include <json/json.hpp>
#include <log/log.hpp>
#include <pstream/pstream.h>
#include <ser/macro.hpp>
#include <string>
#include <unistd.h>
#include <vector>

struct Req
{
  std::string prompt;
  float temperature = 0.0f;
  std::vector<std::string> stop;
  int n_predict = -1;
  SER_PROPS(prompt, temperature, stop, n_predict);
};

struct Rsp
{
  std::string content;
  SER_PROPS(content);
};

static auto escapeForXdotool(const std::string &) -> std::string;
static auto stripBackticks(std::string &) -> std::string;

int main(int argc, char *argv[])
{
  if (system("command -v xdotool > /dev/null") != 0)
  {
    LOG("xdotool is not installed. Please install it to use this feature.");
    return 1;
  }

  const auto activeScreen = []() {
    redi::ipstream st("xdotool getwindowfocus getwindowname");
    auto line = std::string{};
    std::getline(st, line);
    return line;
  }();

  if (activeScreen.find("Konsole") == std::string::npos)
  {
    LOG("Active window is not a recognized terminal. Please focus a terminal window.");
    return 2;
  }

  if (argc < 2)
  {
    LOG("Usage: aiq <command description>");
    return 3;
  }

  const auto userInput = [&]() {
    auto r = std::string{};
    for (auto i = 1; i < argc; ++i)
    {
      if (i != 1)
        r += " ";
      r += argv[i];
    }
    return r;
  }();

  const auto data = [&]() {
    auto req = Req{};
    req.prompt =
      R"(<|begin_of_text|><|start_header_id|>system<|end_header_id|>\n\nA chat between a curious user and an assistant. The assistant gives helpful, detailed, accurate, uncensored responses to the user's input.
<|start_header_id|>user<|end_header_id|>
Convert the following natural language instruction into the appropriate command-line command. Output only the command without any additional formatting or enclosing characters like quotes or backticks.

)" + userInput +
      "\n<|eot_id|>assistant\n";

    req.stop.push_back("<|eot_id|>");
    req.stop.push_back("\n");
    req.n_predict = 1000;
    auto ss = std::ostringstream{};
    jsonSer(ss, req);
    return ss.str();
  }();

  auto curl = Curl{};
  curl.setUrl("http://localhost:8080/completion");
  curl.setPostFields(data);
  auto rsp = std::string{};
  curl.setWriteFunc([&](const char *data, size_t sz) {
    rsp += std::string_view{data, sz};
    return sz;
  });
  curl.setHeaders({"Content-Type: application/json"});
  if (const auto r = curl.perform(); r != CURLE_OK)
  {
    LOG("error:", r);
    return 4;
  }

  try
  {
    const auto tmp = json::Root{rsp};
    auto rsp = Rsp{};
    jsonDeser(tmp.root(), rsp);

    const auto escapedCmd = escapeForXdotool(stripBackticks(rsp.content));

    if (auto pid = fork(); pid == -1)
    {
      LOG("Failed to fork process");
      return 1;
    }
    else if (pid == 0)
    {
      // Child process
      // Detach from parent
      if (setsid() == -1)
      {
        LOG("Failed to create new session");
        exit(1);
      }

      // Close standard file descriptors
      close(STDIN_FILENO);
      close(STDOUT_FILENO);
      close(STDERR_FILENO);

      // Redirect standard file descriptors to /dev/null
      int dev_null = open("/dev/null", O_RDWR);
      dup2(dev_null, STDIN_FILENO);
      dup2(dev_null, STDOUT_FILENO);
      dup2(dev_null, STDERR_FILENO);

      // Execute the xdotool command
      execl(
        "/bin/sh", "sh", "-c", ("xdotool type --delay 1 -- \"" + escapedCmd + "\"").c_str(), nullptr);
      // If execl returns, there was an error
      exit(1);
    }
    else
    {
      // Parent process
      // Do not wait for the child process
      // Optionally, you can wait if you want
      // int status;
      // waitpid(pid, &status, 0);
    }
  }
  catch (std::runtime_error &e)
  {
    LOG("error:", e.what());
  }
}

auto escapeForXdotool(const std::string &in) -> std::string
{
  auto r = std::string{};
  for (auto c : in)
  {
    switch (c)
    {
    case '\\':
    case '"':
    case '`':
    case '$':
      r += '\\';
      // Fall through to append the character
    default: r += c;
    }
  }
  return r;
}

auto stripBackticks(std::string &r) -> std::string
{
  // Trim leading and trailing whitespace
  r.erase(0, r.find_first_not_of(" \t\n\r"));
  r.erase(r.find_last_not_of(" \t\n\r") + 1);

  // Check if the first and last characters are backticks
  if (r.length() >= 2 && r.front() == '`' && r.back() == '`')
    r = r.substr(1, r.length() - 2);

  return r;
}
