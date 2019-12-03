#include "fpga_sender.h"
#include <stdexcept>
#include <thread>

using namespace fpga_ticker_client;

fpga_sender::fpga_sender(const std::string &fpga_device_name)
    : fpga_device_stream("/dev/" + fpga_device_name, std::ofstream::out | std::ofstream::binary)
{ }

bool fpga_sender::is_opened() const
{
    return fpga_device_stream.good();
}

void fpga_sender::send(const std::string& text, const std::chrono::system_clock::duration& ticker_period)
{
    if (!is_opened()) {
        throw std::logic_error("FPGA file was not opened");
    }

    const std::unique_ptr<std::vector<std::uint8_t>> seven_segment_characters = transform_text(text);
    for (const uint8_t symbol : *seven_segment_characters) {
        fpga_device_stream << symbol;
        std::this_thread::sleep_for(ticker_period);
    }
    fpga_device_stream.flush();
}

std::unique_ptr<std::vector<std::uint8_t>> fpga_sender::transform_text(const std::string &text) const
{
    /*
     * 7-segment register segments numbers:
     *
     *    _3_
     *   |   |
     * 4 |_6_| 2
     *   |   |
     * 5 |___| 1
     *     0
     */

    const auto generate_seven_segment_symbol = [](const std::vector<uint8_t>& segments) {
        uint8_t symbol = 0;
        for (const uint8_t segment : segments)
        {
            symbol |= (uint8_t)(1u << segment);
        }
        return symbol;
    };
    const std::unordered_map<std::string::value_type, std::vector<uint8_t>> seven_segment_characters = {
            { 'a', { generate_seven_segment_symbol({ 1, 2, 3, 4, 5, 6 }) } },
            { 'b', { generate_seven_segment_symbol({ 0, 1, 4, 5, 6 }) } },
            { 'c', { generate_seven_segment_symbol({ 0, 3, 4, 5 }) } },
            { 'd', { generate_seven_segment_symbol({ 0, 1, 2, 5, 6 }) } },
            { 'e', { generate_seven_segment_symbol({ 0, 3, 4, 5, 6 }) } },
            { 'f', { generate_seven_segment_symbol({ 2, 3, 4, 5, 6 }) } },
            { 'g', { generate_seven_segment_symbol({ 0, 1, 3, 4, 5, 6 }) } },
            { 'h', { generate_seven_segment_symbol({ 1, 2, 4, 5, 6 }) } },
            { 'i', { generate_seven_segment_symbol({ 1, 2 }) } },
            { 'j', { generate_seven_segment_symbol({ 0, 1, 2 }) } },
            { 'k', { generate_seven_segment_symbol({ 1, 2, 4, 5, 6 }), generate_seven_segment_symbol({ 0,3 }) } },
            { 'l', { generate_seven_segment_symbol({ 0, 4, 5 }) } },
            { 'm', { generate_seven_segment_symbol({ 1, 2, 3, 4, 5 }), generate_seven_segment_symbol({ 1, 2, 3 }) } },
            { 'n', { generate_seven_segment_symbol({ 1, 5, 6 }) } },
            { 'o', { generate_seven_segment_symbol({ 0, 1, 2, 3, 4, 5 }) } },
            { 'p', { generate_seven_segment_symbol({ 2, 3, 4, 5, 6 }) } },
            { 'q', { generate_seven_segment_symbol({ 1, 2, 3, 4, 6 }) } },
            { 'r', { generate_seven_segment_symbol({ 5, 6 }) } },
            { 's', { generate_seven_segment_symbol({ 0, 1, 3, 4, 6 }) } },
            { 't', { generate_seven_segment_symbol({ 1, 2, 3 }), generate_seven_segment_symbol({ 3 }) } },
            { 'u', { generate_seven_segment_symbol({ 0, 1, 2, 4, 5 }) } },
            { 'w', { generate_seven_segment_symbol({ 0, 1, 4, 5 }), generate_seven_segment_symbol({ 0, 1, 2 }) } },
            { 'x', { generate_seven_segment_symbol({ 0, 1, 2, 3 }), generate_seven_segment_symbol({ 0, 3 }) } },
            { '0', { generate_seven_segment_symbol({ 0, 1, 2, 3, 4, 5 }) } },
            { '1', { generate_seven_segment_symbol({ 1, 2 }) } },
            { '2', { generate_seven_segment_symbol({ 0, 2, 3, 5, 6 }) } },
            { '3', { generate_seven_segment_symbol({ 0, 1, 2, 3, 6 }) } },
            { '4', { generate_seven_segment_symbol({ 1, 2, 4, 6 }) } },
            { '5', { generate_seven_segment_symbol({ 0, 1, 3, 4, 6 }) } },
            { '6', { generate_seven_segment_symbol({ 0, 1, 3, 4, 5, 6 }) } },
            { '7', { generate_seven_segment_symbol({ 1, 2, 3 }) } },
            { '8', { generate_seven_segment_symbol({ 0, 1, 2, 3, 4, 5, 6 }) } },
            { '9', { generate_seven_segment_symbol({ 0, 1, 2, 3, 4, 6 }) } },
            { ' ', { generate_seven_segment_symbol({ }) } }
    };

    auto result = std::make_unique<std::vector<std::uint8_t>>();
    for (const std::string::value_type text_character : text) {
        try {
            const std::vector<std::uint8_t>& seven_segment_symbols = seven_segment_characters.at(std::tolower(text_character));
            for (const uint8_t symbol : seven_segment_symbols) {
                result->push_back(symbol);
            }
        } catch (const std::out_of_range&) {
            throw std::runtime_error(std::string("Symbol ") + text_character + " is not supported");
        }
    }
    return result;
}
