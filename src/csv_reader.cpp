#include "csv_reader.h"

#include <charconv>
#include <fstream>
#include <string>
#include <string_view>

namespace llt {

namespace {

template <typename T>
bool parse_integer(std::string_view token, T& value) {
    const char* begin = token.data();
    const char* end = token.data() + token.size();
    const auto [ptr, ec] = std::from_chars(begin, end, value);
    return ec == std::errc{} && ptr == end;
}

}  // namespace

std::vector<MarketTick> CsvReader::read_all(const char* path) const {
    std::ifstream input(path);
    std::vector<MarketTick> ticks;
    if (!input.is_open()) {
        return ticks;
    }

    std::string line;
    if (!std::getline(input, line)) {
        return ticks;
    }

    ticks.reserve(count_data_lines(path));
    while (std::getline(input, line)) {
        MarketTick tick{};
        if (parse_line(line.data(), line.data() + line.size(), tick)) {
            ticks.push_back(tick);
        }
    }

    return ticks;
}

std::size_t CsvReader::count_data_lines(const char* path) {
    std::ifstream input(path);
    if (!input.is_open()) {
        return 0;
    }

    std::string line;
    if (!std::getline(input, line)) {
        return 0;
    }

    std::size_t count = 0;
    while (std::getline(input, line)) {
        ++count;
    }

    return count;
}

bool CsvReader::parse_line(const char* begin, const char* end, MarketTick& tick) {
    std::string_view fields[7];
    std::size_t field_index = 0;
    const char* field_begin = begin;

    for (const char* cursor = begin; cursor <= end; ++cursor) {
        if (cursor == end || *cursor == ',') {
            if (field_index >= 7) {
                return false;
            }
            fields[field_index++] = std::string_view(field_begin, static_cast<std::size_t>(cursor - field_begin));
            field_begin = cursor + 1;
        }
    }

    if (field_index != 7) {
        return false;
    }

    return parse_integer(fields[0], tick.exchange_ts_ns) &&
        parse_integer(fields[1], tick.receive_ts_ns) &&
        parse_integer(fields[2], tick.symbol_id) &&
        parse_integer(fields[3], tick.bid_price) &&
        parse_integer(fields[4], tick.ask_price) &&
        parse_integer(fields[5], tick.bid_qty) &&
        parse_integer(fields[6], tick.ask_qty);
}

}  // namespace llt
