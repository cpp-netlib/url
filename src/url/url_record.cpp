// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <skyr/url/url_record.hpp>
#include "url_schemes.hpp"

namespace skyr {
inline namespace v1 {
bool url_record::is_special() const noexcept {
  return skyr::details::is_special(std::string_view(scheme));
}

bool url_record::includes_credentials() const noexcept {
  return !username.empty() || !password.empty();
}

bool url_record::cannot_have_a_username_password_or_port() const noexcept {
  return
      (!host || host.value().empty()) ||
          cannot_be_a_base_url ||
          (scheme == "file");
}

void url_record::swap(url_record &other) noexcept {
  using std::swap;
  swap(scheme, other.scheme);
  swap(username, other.username);
  swap(password, other.password);
  swap(host, other.host);
  swap(port, other.port);
  swap(path, other.path);
  swap(query, other.query);
  swap(fragment, other.fragment);
  swap(cannot_be_a_base_url, other.cannot_be_a_base_url);
  swap(validation_error, other.validation_error);
}
}  // namespace v1
}  // namespace skyr
