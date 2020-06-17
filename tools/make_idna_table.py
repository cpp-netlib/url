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


def squeeze(code_points):
    code_points_copy = [code_points[0]]
    for code_point in code_points[1:]:
        if code_points_copy[-1].status == code_point.status:
            code_points_copy[-1].range[1] = code_point.range[1]
        else:
            code_points_copy.append(code_point)
    return code_points_copy


def main():
    input, output = sys.argv[1], sys.argv[2]

    with open(input, 'r') as input_file, open(output, 'w+') as output_file:
        code_points = []
        for line in input_file.readlines():
            code_point = parse_line(line)
            if code_point:
                code_points.append(CodePointRange(
                    code_point[0], code_point[1], code_point[2] if len(code_point) > 2 else None))

        mapped_code_points = [
            entry for entry in code_points if entry.status in ('mapped', 'disallowed_STD3_mapped')]
        code_points = squeeze(code_points)

        template = jinja2.Template(
            """// Auto-generated.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <iterator>
#include <cassert>
#include <array>
#include "idna.hpp"

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
  assert(it != last);
  return it->status;
}

namespace {
struct mapped_code_point {
  char32_t code_point;
  char32_t mapped;
};

constexpr static auto mapped = std::array<mapped_code_point, {{ mapped_entries|length }}>{% raw %}{{{% endraw %}
{% for code_point in mapped_entries %}{% if code_point.status in ('mapped', 'disallowed_STD3_mapped') %}  { U'\\x{{ '%04x' % code_point.range[0] }}', U'\\x{{ '%04x' % code_point.mapped }}' },
{% endif %}{% endfor %}{% raw %}}}{% endraw %};
}  // namespace

auto map_code_point(char32_t code_point) -> char32_t {
  constexpr static auto less = [](const auto &mapped, auto code_point) {
    return mapped.code_point < code_point;
  };

  auto first = std::begin(mapped), last = std::end(mapped);
  auto it = std::lower_bound(first, last, code_point, less);
  return (it != last) ? it->mapped : code_point;
}
}  // namespace idna
}  // namespace v1
}  // namespace skyr
""")

        template.stream(
            entries=code_points,
            mapped_entries=mapped_code_points).dump(output_file)


if __name__ == '__main__':
    main()
