# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

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
