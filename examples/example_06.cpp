// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <skyr/url.hpp>
#include <skyr/url/percent_encoding/percent_decode_range.hpp>

namespace {
template <class StringList>
tl::expected<StringList, std::error_code> split_pathname(const skyr::url &url)  {
  auto pathname = url.pathname();
  std::vector<std::string> path;
  auto it = begin(pathname), last = end(pathname);
  auto last_it = it;
  while (it != last) {
    if (*it == '/') {
      path.emplace_back(last_it, it);
      last_it = it;
      ++last_it;
    }

    ++it;
  }

  path.emplace_back(last_it, last);
  return path;
};

}  // namespace


int main(int argc, char *argv[]) {

  auto decode_poo = [] (const auto &path) {
    return skyr::percent_encoding::as<std::string>(
        path.back() | skyr::percent_encoding::view::decode);
  };

  auto print_poo = [] (const auto &poo) -> tl::expected<void, std::error_code> {
    std::cout << poo << std::endl;
    return {};
  };

  skyr::make_url("http://example.org/\xf0\x9f\x92\xa9")
      .and_then(split_pathname<std::vector<std::string>>)
      .and_then(decode_poo)
      .and_then(print_poo)
      ;
}
