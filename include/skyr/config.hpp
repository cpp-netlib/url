// Copyright (c) Glyn Matthews 2012-2020.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/**
 * \file
 * \brief Contains macros to configure compiler or platform-specific
 *        workarounds
 */

#ifndef SKYR_CONFIG_HPP
#define SKYR_CONFIG_HPP

#if defined(_MSC_VER)
#define SKYR_PLATFORM_MSVC _MSC_VER
#endif  // defined(_MSC_VER)

#define SKYR_PREPROCESSOR_TO_STRING_X(arg) #arg
#define SKYR_PREPROCESSOR_TO_STRING(arg) SKYR_PREPROCESSOR_TO_STRING_X(arg)

#if __cpp_exceptions
#define SKYR_EXCEPTIONS_TRY() try
#define SKYR_EXCEPTIONS_CATCH(e) catch(e)
#define SKYR_EXCEPTIONS_THROW(e) throw e
#else
#define SKYR_EXCEPTIONS_TRY() if (true)
#define SKYR_EXCEPTIONS_CATCH(e) if (false)
#define SKYR_EXCEPTIONS_THROW(e) (void)(e)
#endif // __cpp_exceptions

#endif // SKYR_CONFIG_HPP
