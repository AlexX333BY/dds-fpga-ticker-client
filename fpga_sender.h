#ifndef DDS_FPGA_TICKER_CLIENT_FPGA_SENDER_H
#define DDS_FPGA_TICKER_CLIENT_FPGA_SENDER_H


#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include "serial_device.h"

namespace fpga_ticker_client {
    class fpga_sender {
    public:
        explicit fpga_sender(const std::shared_ptr<serial_device>& fpga_device);

        void send(const std::string& text, const std::chrono::system_clock::duration& ticker_period);
        void stop();

    private:
        std::vector<std::uint8_t> transform_text(const std::string& text) const;
        std::atomic_bool should_send;
        std::condition_variable send_cv;
        const std::shared_ptr<serial_device> fpga_device;
    };
}


#endif //DDS_FPGA_TICKER_CLIENT_FPGA_SENDER_H
