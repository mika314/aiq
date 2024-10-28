#include "local-llm.hpp"
#include <algorithm>
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
static auto strip(std::string) -> std::string;

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

  const auto data = LocalLlm{}.chatCompletions(
    {{Role::user,
      R"(Convert the following natural language instruction into the appropriate command-line command. Output only the command without any additional formatting or enclosing characters like quotes or backticks.

## Examples

Here are some examples of how to use `aiq`:

aiq find my tax report pdf documents
find ~ -type f -iname "*tax*.pdf"

aiq list ~/bin directory and show symlinks paths
ls -l ~/bin

aiq pick randomly from Alice, Bob or Charlie
shuf -n 1 -e Alice Bob Charlie | head -1

aiq calculate 69 + 420
bc -l <<< "69 + 420"

aiq current time in human readable format
date +"%A, %B %d, %Y %I:%M:%S %p"

aiq get the list of the files in the libglu1-mesa-dev package
dpkg -L libglu1-mesa-dev

aiq find all .cpp files containing "main"
grep -rl "main" *.cpp

aiq show disk usage of the /home directory
du -h /home

aiq compress all .log files in the current directory
tar -czvf logs.tar.gz *.log

aiq display the last 20 lines of syslog
tail -n 20 /var/log/syslog

aiq monitor network traffic in real-time
iftop

aiq check if port 8080 is open
netstat -tuln | grep :8080

aiq ping google.com five times
ping -c 5 google.com

aiq show all running Docker containers
docker ps

aiq clone a Git repository from https://github.com/example/repo.git
git clone --recursive https://github.com/example/repo.git

aiq convert file.txt from DOS to Unix format
dos2unix file.txt

aiq create a Python virtual environment in the current directory
python3 -m venv venv

aiq find and delete all .tmp files
find . -type f -name "*.tmp" -delete

aiq list all installed packages on Ubuntu
dpkg --get-selections

aiq show current Git branch
git branch --show-current

aiq check disk space usage
df -h

aiq pick randomly from Chris H., Chris A. or Paul
shuf -n 1 -e "Chris H." "Chris A." Paul | head -1

aiq show me last modified 10 files
ls -lt | head -n 10

aiq what bitrate of aiq-demo-0001-1482.mp4
ffmpeg -i aiq-demo-0001-1482.mp4 2>&1 | grep -i "bitrate"

aiq unmangle the symbol _ZSt8_DestroyIPN4json3ValES1_EvT_S3_RSaIT0_E
echo '_ZSt8_DestroyIPN4json3ValES1_EvT_S3_RSaIT0_E' | c++filt

## User Input

)" + userInput}},
    500,
    0.0f);

  try
  {
    const auto escapedCmd = escapeForXdotool(strip(data));

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

auto strip(std::string r) -> std::string
{
  r.erase(0, r.find_first_not_of(" \t\n\r`"));
  r.erase(r.find_last_not_of(" \t\n\r`") + 1);
  for (auto pos = r.find('\n'); pos != std::string::npos; pos = r.find('\n', pos))
  {
    r.replace(pos, 1, "; ");
    pos += 2;
  }
  return r;
}
