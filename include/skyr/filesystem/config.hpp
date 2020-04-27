// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_FILESYSTEM_CONFIG_HPP
#define SKYR_URL_FILESYSTEM_CONFIG_HPP

#if !defined(SKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM)
#if !defined(__clang__) && (defined(__GNUC__) && __GNUC__ < 8)
#define SKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM
#endif  // !defined(__clang__) && (defined(__GNUC__) && __GNUC__ < 8)
#endif  // !defined(SKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM)

#endif  // SKYR_URL_FILESYSTEM_CONFIG_HPP
