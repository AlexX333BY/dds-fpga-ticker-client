#include "fpga_sender.h"
#include <stdexcept>
#include <mutex>
#include <unordered_map>

using namespace fpga_ticker_client;

fpga_sender::fpga_sender(const std::shared_ptr<serial_device>& fpga_device)
    : fpga_device(fpga_device), should_send(false)
{ }

void fpga_sender::send(const std::string& text, const std::chrono::system_clock::duration& ticker_period)
{
    if (!fpga_device->is_opened()) {
        throw std::logic_error("FPGA device was not opened");
    }

    const std::vector<std::uint8_t> seven_segment_characters = transform_text(text);
    size_t current_character = 0;
    std::mutex send_mx;
    should_send = true;
    while (should_send) {
        fpga_device->write_byte(seven_segment_characters[current_character]);
        {
            std::unique_lock<std::mutex> lock(send_mx);
            send_cv.wait_for(lock, ticker_period);
        }
        current_character = (current_character + 1) % seven_segment_characters.size();
    }
}

std::vector<std::uint8_t> fpga_sender::transform_text(const std::string &text) const
{
    /*
     * 7-segment register segments numbers:
     *
     * Segment encoding
     *      0
     *     ---
     *  5 |   | 1
     *     ---   <- 6
     *  4 |   | 2
     *     ---
     *      3
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
            { 'a', { generate_seven_segment_symbol({ 0, 1, 2, 4, 5, 6 }) } },
            { 'b', { generate_seven_segment_symbol({ 2, 3, 4, 5 }) } },
            { 'c', { generate_seven_segment_symbol({ 0, 3, 4, 5 }) } },
            { 'd', { generate_seven_segment_symbol({ 1, 2, 3, 4, 6 }) } },
            { 'e', { generate_seven_segment_symbol({ 0, 3, 4, 5, 6 }) } },
            { 'f', { generate_seven_segment_symbol({ 0, 4, 5, 6 }) } },
            { 'g', { generate_seven_segment_symbol({ 0, 2, 3, 4, 5, 6 }) } },
            { 'h', { generate_seven_segment_symbol({ 1, 2, 4, 5, 6 }) } },
            { 'i', { generate_seven_segment_symbol({ 1, 2 }) } },
            { 'j', { generate_seven_segment_symbol({ 1, 2, 3 }) } },
            { 'k', { generate_seven_segment_symbol({ 1, 2, 4, 5, 6 }), generate_seven_segment_symbol({ 0, 3 }) } },
            { 'l', { generate_seven_segment_symbol({ 3, 4, 5 }) } },
            { 'm', { generate_seven_segment_symbol({ 0, 1, 2, 4, 5 }), generate_seven_segment_symbol({ 0, 1, 2 }) } },
            { 'n', { generate_seven_segment_symbol({ 2, 4, 6 }) } },
            { 'o', { generate_seven_segment_symbol({ 0, 1, 2, 3, 4, 5 }) } },
            { 'p', { generate_seven_segment_symbol({ 0, 1, 4, 5, 6 }) } },
            { 'q', { generate_seven_segment_symbol({ 0, 1, 2, 5, 6 }) } },
            { 'r', { generate_seven_segment_symbol({ 4, 6 }) } },
            { 's', { generate_seven_segment_symbol({ 0, 2, 3, 5, 6 }) } },
            { 't', { generate_seven_segment_symbol({ 0, 1, 2 }), generate_seven_segment_symbol({ 0 }) } },
            { 'u', { generate_seven_segment_symbol({ 1, 2, 3, 4, 5 }) } },
            { 'w', { generate_seven_segment_symbol({ 1, 2, 3, 4, 5 }), generate_seven_segment_symbol({ 1, 2, 3 }) } },
            { 'x', { generate_seven_segment_symbol({ 0, 1, 2, 3 }), generate_seven_segment_symbol({ 0, 3 }) } },
            { '0', { generate_seven_segment_symbol({ 0, 1, 2, 3, 4, 5 }) } },
            { '1', { generate_seven_segment_symbol({ 1, 2 }) } },
            { '2', { generate_seven_segment_symbol({ 0, 1, 3, 4, 6 }) } },
            { '3', { generate_seven_segment_symbol({ 0, 1, 2, 3, 6 }) } },
            { '4', { generate_seven_segment_symbol({ 1, 2, 5, 6 }) } },
            { '5', { generate_seven_segment_symbol({ 0, 2, 3, 5, 6 }) } },
            { '6', { generate_seven_segment_symbol({ 0, 2, 3, 4, 5, 6 }) } },
            { '7', { generate_seven_segment_symbol({ 0, 1, 2 }) } },
            { '8', { generate_seven_segment_symbol({ 0, 1, 2, 3, 4, 5, 6 }) } },
            { '9', { generate_seven_segment_symbol({ 0, 1, 2, 3, 5, 6 }) } },
            { ' ', { generate_seven_segment_symbol({ }) } }
    };

    std::vector<std::uint8_t> result;
    for (const std::string::value_type text_character : text) {
        try {
            const std::vector<std::uint8_t>& seven_segment_symbols = seven_segment_characters.at(std::tolower(text_character));
            for (const uint8_t symbol : seven_segment_symbols) {
                result.push_back(symbol);
            }
        } catch (const std::out_of_range&) {
            throw std::runtime_error(std::string("Symbol ") + text_character + " is not supported");
        }
    }
    result.shrink_to_fit();
    return result;
}

void fpga_sender::stop()
{
    should_send = false;
    send_cv.notify_all();
}
