#include "curl.hpp"
#include <stdexcept>

Curl::Curl() : curl(curl_easy_init())
{
  if (!curl)
    throw std::runtime_error("Failed to initialize curl");
}

auto Curl::setUrl(const std::string &v) -> void
{
  curl_easy_setopt(curl, CURLOPT_URL, v.c_str());
}

Curl::~Curl()
{
  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
}

auto Curl::setPostFields(const std::string &v) -> void
{
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, v.c_str());
}

auto Curl::setHeaders(const std::vector<std::string> &v) -> void
{
  for (const auto &header : v)
    headers = curl_slist_append(headers, header.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
}

auto Curl::staticWriteFunc(void *content, size_t size, size_t nmemb, void *userp) -> size_t
{
  auto func = reinterpret_cast<WriteFunc *>(userp);
  return (*func)(static_cast<const char *>(content), size * nmemb);
}

auto Curl::setWriteFunc(WriteFunc v) -> void
{
  writeFunc = std::make_unique<WriteFunc>(std::move(v));
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, staticWriteFunc);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, writeFunc.get());
}

auto Curl::perform() -> CURLcode
{
  return curl_easy_perform(curl);
}
