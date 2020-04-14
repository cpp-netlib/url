# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [1.7.0] - 2020-04-16

### Added

- A function to convert an encoded domain to Unicode
- Functions to convert a query string to and from a JSON object
- A lot more documentation

### Changed

- Code style changes

## [1.6.0] - 2020-04-12

### Added

- IPv4 and IPv6 address hostname accessors

### Changed

- Simplified functions for percent encoding and decoding
- ``url_record`` has moved to skyr/core
- Moved web platform tests out of this repository

### Fixed

- Fixed tests for https and file special schemes (#50)

## [1.5.2] - 2020-04-07

### Changed

- Version number because of failure when integrating with vcpkg

## [1.5.1] - 2020-04-04

### Fixed

- Configuration script to allow the library to be used as a
  dependency with vcpkg

## [1.5] - 2020-04-04

### Changed

- Rewrote install steps
- Added .clang-tidy and applied changes

## [1.4.5] - 2020-03-30

### Changed

- Added support for GCC 7 on Linux

## [1.4.4] - 2020-03-29

### Fixed

- Minor fix to the parser (#37)

### Changed

- Updated the coverage of the web platform tests (#39)
- String literal operators throw exceptions now (#42)

## [1.4.3] - 2020-03-18

### Fixed

- Minimum CMake version is 3.14

## [1.4.2] - 2020-03-18

### Fixed

- Some build script bugs that were introduced in the last releases

## [1.4.1] - 2020-03-15

### Fixed

- Filesystem conversions tests
- Upgraded build scripts

## [1.4] - 2020-01-01

### Added

- URLs can be constructed using string literals, e.g.  `auto url = "http://example.com"_url;`

### Changed

- Forced CMake to choose only C++17 standard library (i.e. no extensions)
- To build with LLVM libcxx, enable it explicitly using `cmake -DSkyr_BUILD_WITH_LLVM_LIBCXX=ON`

### Fixed

- Added explicit copy assignment and move assignment operators

## [1.3] - 2019-11-05

### Added

- The possibility to build the library with exceptions disabled
  (``-fexception``)
- ``FindSkyr.cmake``, which allows the library to be used as a project
  dependency more easily
- Examples and better documentation
  
### Changed

- Uses [vcpkg](https://vcpkg.readthedocs.io/en/latest/) for
  dependencies

### Fixed

- Bugs when using ``url_search_parameters``

## [1.2] - 2019-08-16

### Added
- Changed CMake scripts to make the library usable by downstream
  projects

### Changed
- Reordered some of the internal headers
- Exposed Unicode functions in the public interface

### Fixed
- Fixed bug when setting the port from a string type

## [1.1] - 2019-07-22

### Changed
- Updated optional and expected to use v1.0 of each
- Changes to minor API to process percent encoding

### Fixed
- Fixed bug when using ``url_search_parameters``
- Fixes to ``CMakeLists.txt``

## [1.0] - 2018-10-13
### Added
- Conversion functions between ``skyr::url`` and ``std::filesystem::path`` 

### Changed
- Improved license attributions
- Minor API improvements
- Improved build and installation instructions in README.md

## [0.5] - 2018-09-30

### Added
- A ``skyr::url`` class that implements a generic URL parser,
  compatible with the [WhatWG URL specification](https://url.spec.whatwg.org/#url-class)
- URL serialization and comparison
- Percent encoding and decoding functions
- IDNA and Punycode functions for domain name parsing
