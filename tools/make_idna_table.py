# Copyright 2018-20 Glyn Matthews.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# This script parses the IDNA table from
# https://unicode.org/Public/idna/11.0.0/IdnaMappingTable.txt,
# and converts it to a C++ table.


import sys
import jinja2


def parse_line(line):
    line = line[0:line.find('#')]
    tokens = [token.strip() for token in line.split(';')] if line else []
    if len(tokens) == 3:
        tokens[2] = tokens[2].split(' ')[0]
    return tokens


status_keys = [
    'valid',
    'mapped',
    'disallowed',
    'disallowed_STD3_valid',
    'disallowed_STD3_mapped',
    'ignored',
    'deviation',
    ]


class CodePointRange(object):

    def __init__(self, range, status, mapped):
        if type(range) == str:
            range = range.split('..') if '..' in range else [range, range]
        if type(range[0]) == str:
            range = [int(range[0], 16), int(range[1], 16)]
        self.range = range
        self.status = status
        self.mapped = int(mapped, 16) if mapped else None

    @property
    def is_mapped(self):
        return self.status in ('mapped', 'disallowed_STD3_mapped')

    @property
    def is_valid(self):
        return self.status == 'valid'

    @property
    def can_be_16_bit(self):
        return self.range[0] <= 0xffff and self.mapped is not None and self.mapped <= 0xffff


def main():
    input, output = sys.argv[1], sys.argv[2]

    with open(input, 'r') as input_file, open(output, 'w+') as output_file:
        code_points = []
        for line in input_file.readlines():
            code_point = parse_line(line)
            if code_point:
                code_points.append(CodePointRange(
                    code_point[0], code_point[1], code_point[2] if len(code_point) > 2 else None))

        # Store code point mappings as std::char16_t if they can, otherwise store them as std::char32_t
        mapped_code_points_16, mapped_code_points_32 = [], []
        for code_point in filter(lambda cp: cp.is_mapped, code_points):
            (mapped_code_points_32, mapped_code_points_16)[code_point.can_be_16_bit].append(code_point)

        # Squeeze code points to reduce table size, and remove all valid code points as they will be handled by default
        code_points = [code_point for code_point in code_points if not code_point.is_valid]

        template = jinja2.Template(
            """// Auto-generated.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <iterator>
#include <array>
#include <skyr/v1/domain/idna.hpp>

namespace skyr {
inline namespace v1 {
namespace idna {
namespace {
struct code_point_range {
  char32_t first;
  char32_t last;
  idna_status status;
};

constexpr static auto statuses = std::array<code_point_range, {{ entries|length }}>{% raw %}{{{% endraw %}
{% for code_point in entries %}  { U'\\x{{ '%04x' % code_point.range[0] }}', U'\\x{{ '%04x' % code_point.range[1] }}', idna_status::{{ code_point.status.lower() }} },
{% endfor %}{% raw %}}}{% endraw %};
}  // namespace

auto code_point_status(char32_t code_point) -> idna_status {
  constexpr static auto less = [] (const auto &range, auto code_point) {
    return range.last < code_point;
  };

  auto first = std::begin(statuses), last = std::end(statuses);
  auto it = std::lower_bound(first, last, code_point, less);
  return (it == last) || !((code_point >= (*it).first) && (code_point <= (*it).last)) ? idna_status::valid : it->status;
}

namespace {
struct mapped_16_code_point {
  char16_t code_point;
  char16_t mapped;
};

constexpr static auto mapped_16 = std::array<mapped_16_code_point, {{ mapped_entries_16|length }}>{% raw %}{{{% endraw %}
{% for code_point in mapped_entries_16 %}  { U'\\x{{ '%04x' % code_point.range[0] }}', U'\\x{{ '%04x' % code_point.mapped }}' },
{% endfor %}{% raw %}}}{% endraw %};

auto map_code_point_16(char16_t code_point) -> char16_t {
  constexpr static auto less = [](const auto &lhs, auto rhs) {
    return lhs.code_point < rhs;
  };

  auto first = std::begin(mapped_16), last = std::end(mapped_16);
  auto it = std::lower_bound(first, last, code_point, less);
  return (it != last) ? it->mapped : code_point;
}

struct mapped_32_code_point {
  char32_t code_point;
  char32_t mapped;
};

constexpr static auto mapped_32 = std::array<mapped_32_code_point, {{ mapped_entries_32|length }}>{% raw %}{{{% endraw %}
{% for code_point in mapped_entries_32 %}  { U'\\x{{ '%04x' % code_point.range[0] }}', U'\\x{{ '%04x' % code_point.mapped }}' },
{% endfor %}{% raw %}}}{% endraw %};
}  // namespace

auto map_code_point(char32_t code_point) -> char32_t {
  constexpr static auto less = [](const auto &lhs, auto rhs) {
    return lhs.code_point < rhs;
  };
  
  if (code_point <= U'\\xffff') {
    return static_cast<char32_t>(map_code_point_16(static_cast<char16_t>(code_point)));
  }

  auto first = std::begin(mapped_32), last = std::end(mapped_32);
  auto it = std::lower_bound(first, last, code_point, less);
  return (it != last) ? it->mapped : code_point;
}
}  // namespace idna
}  // namespace v1
}  // namespace skyr

""")

        template.stream(
            entries=code_points,
            mapped_entries_16=mapped_code_points_16,
            mapped_entries_32=mapped_code_points_32
        ).dump(output_file)


if __name__ == '__main__':
    main()
