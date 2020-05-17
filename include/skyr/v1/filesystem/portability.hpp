// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_FILESYSTEM_PORTABILITY_HPP
#define SKYR_V1_FILESYSTEM_PORTABILITY_HPP

//#if !defined(SKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM)
//#if !defined(__clang__) && (defined(__GNUC__) && __GNUC__ < 8)
//#define SKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM
//#endif  // !defined(__clang__) && (defined(__GNUC__) && __GNUC__ < 8)
//#endif  // !defined(SKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM)


#if defined(SKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM)
#define _LIBCPP_NO_EXPERIMENTAL_DEPRECATION_WARNING_FILESYSTEM
#include <experimental/filesystem>
#define SKYR_DEFINE_FILESYSTEM_NAMESPACE_ALIAS(name) \
namespace name = std::experimental::filesystem;
#else
#include <filesystem>
#define SKYR_DEFINE_FILESYSTEM_NAMESPACE_ALIAS(name) \
namespace name = std::filesystem;
#endif // defined(SKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM)

#endif  // SKYR_V1_FILESYSTEM_PORTABILITY_HPP
