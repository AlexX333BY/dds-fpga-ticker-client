#ifndef DDS_FPGA_TICKER_CLIENT_SERIAL_DEVICE_H
#define DDS_FPGA_TICKER_CLIENT_SERIAL_DEVICE_H


#include <string>
#include <cstdint>

#if defined (_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

namespace fpga_ticker_client {
    class serial_device {
    public:
        serial_device(const std::string& path, const uint32_t speed);
        ~serial_device();

        bool is_opened() const;
        void write_byte(const uint8_t byte) const;

    private:
#ifdef __unix__
        int device;
#elif defined (_WIN32) || defined(_WIN64)
        HANDLE device;
#endif
    };
}


#endif //DDS_FPGA_TICKER_CLIENT_SERIAL_DEVICE_H
