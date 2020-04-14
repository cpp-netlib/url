// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_VERSION_INC
#define SKYR_VERSION_INC

/// \file skyr/version.hpp

#include <tuple>
#include <skyr/config.hpp>

#define SKYR_VERSION_MAJOR 1
#define SKYR_VERSION_MINOR 7
#define SKYR_VERSION_PATCH 0

#define SKYR_VERSION_STRING \
  SKYR_PREPROCESSOR_TO_STRING(SKYR_VERSION_MAJOR) "." \
  SKYR_PREPROCESSOR_TO_STRING(SKYR_VERSION_MINOR)

namespace skyr {
/// \returns The major, minor and patch version as a tuple
static constexpr std::tuple<int, int, int> version() noexcept {
  return {SKYR_VERSION_MAJOR, SKYR_VERSION_MINOR, SKYR_VERSION_PATCH};
}

/// \returns The version as a string in the form MAJOR.MINOR
static constexpr const char *version_string() noexcept {
  return SKYR_VERSION_STRING;
}
}  // namespace skyr

#endif // SKYR_VERSION_INC
