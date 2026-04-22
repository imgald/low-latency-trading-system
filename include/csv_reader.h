#pragma once

#include <cstddef>
#include <vector>

#include "types.h"

namespace llt {

class CsvReader {
public:
    [[nodiscard]] std::vector<MarketTick> read_all(const char* path) const;

private:
    [[nodiscard]] static std::size_t count_data_lines(const char* path);
    static bool parse_line(const char* begin, const char* end, MarketTick& tick);
};

}  // namespace llt
