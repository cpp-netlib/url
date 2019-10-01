// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_PERCENT_ENCODE_RANGE_HPP
#define SKYR_PERCENT_ENCODE_RANGE_HPP

#include <iterator>
#include <cassert>
#include <tl/expected.hpp>
#include <skyr/url/percent_encode.hpp>

namespace skyr::percent_encoding {
class encoded_character {
 private:

  char char_[4];

};

template <class OctetIterator>
class percent_encoding_iterator {
 public:

  using iterator_category = std::forward_iterator_tag;
  using value_type = tl::expected<encoded_character, std::error_code>;
  using reference = value_type;
  using pointer = typename std::add_pointer<value_type>::type;
  using difference_type = std::ptrdiff_t;

  percent_encoding_iterator() noexcept = default;
  percent_encoding_iterator(OctetIterator it, OctetIterator last)
  : it_(it)
  , last_(last) {}
  percent_encoding_iterator(const percent_encoding_iterator &) = default;
  percent_encoding_iterator(percent_encoding_iterator &&) noexcept = default;
  percent_encoding_iterator &operator=(const percent_encoding_iterator &) = default;
  percent_encoding_iterator &operator=(percent_encoding_iterator &&) noexcept = default;
  ~percent_encoding_iterator() = default;

  percent_encoding_iterator &operator++() {
    assert(it_);
    ++it_.value();
    return *this;
  }

  percent_encoding_iterator operator++(int) {
    assert(it_);
    auto result = *this;
    ++it_.value();
    return result;
  }

  reference operator*() const noexcept {
    assert(it_);
    return {};
  }

 private:

  std::optional<OctetIterator> it_, last_;

};
}  // namespace skyr::percent_encoding

#endif //SKYR_PERCENT_ENCODE_RANGE_HPP
