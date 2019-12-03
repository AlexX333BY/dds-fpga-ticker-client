#ifndef DDS_FPGA_TICKER_CLIENT_FPGA_SENDER_H
#define DDS_FPGA_TICKER_CLIENT_FPGA_SENDER_H


#include <string>
#include <fstream>
#include <unordered_map>
#include <memory>
#include <vector>
#include <chrono>

namespace fpga_ticker_client {
    class fpga_sender {
    public:
        explicit fpga_sender(const std::string& fpga_device_name);

        bool is_opened() const;
        void send(const std::string& text, const std::chrono::system_clock::duration& ticker_period);

    private:
        std::unique_ptr<std::vector<std::uint8_t>> transform_text(const std::string& text) const;

        std::ofstream fpga_device_stream;
    };
}


#endif //DDS_FPGA_TICKER_CLIENT_FPGA_SENDER_H
