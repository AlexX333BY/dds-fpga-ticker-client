#include "serial_device.h"
#include <stdexcept>

using namespace fpga_ticker_client;

#ifdef __unix__
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <unordered_map>

serial_device::serial_device(const std::string &path, const uint32_t speed) : device(-1)
{
    const std::unordered_map<uint32_t, speed_t> serial_speeds = {
        { 0, B0 },
        { 50, B50 },
        { 75, B75 },
        { 110, B110 },
        { 134, B134 },
        { 150, B150 },
        { 200, B200 },
        { 300, B300 },
        { 600, B600 },
        { 1200, B1200 },
        { 1800, B1800 },
        { 2400, B2400 },
        { 4800, B4800 },
        { 9600, B9600 },
        { 19200, B19200 },
        { 38400, B38400 },
        { 57600, B57600 },
        { 115200, B115200 },
        { 230400, B230400 }
    };

    try {
        const speed_t port_speed = serial_speeds.at(speed);

        device = open(path.c_str(), O_RDWR | O_NOCTTY);
        if (device != -1) {
            if(isatty(device)) {
                struct termios config = {};

                if (tcgetattr(device, &config) != -1) {
                    config.c_oflag = 0;
                    config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
                    config.c_cflag &= ~(CSIZE | PARENB);
                    config.c_cflag |= CS8;
                    config.c_cc[VTIME] = 0;

                    if ((cfsetospeed(&config, port_speed == -1))
                        || (tcsetattr(device, TCSANOW, &config) == -1)) {
                        close(device);
                        device = -1;
                    }
                } else {
                    close(device);
                    device = -1;
                }
            } else {
                close(device);
                device = -1;
            }
        }
    } catch (const std::out_of_range&) {
        if (device != -1) {
            close(device);
            device = -1;
        }
    }
}

serial_device::~serial_device()
{
    if (device != -1) {
        close(device);
    }
}

bool serial_device::is_opened() const
{
    return device != -1;
}

void serial_device::write_byte(const uint8_t byte) const
{
    if (!is_opened()) {
        throw std::logic_error("Device is not opened");
    }

    ssize_t write_result;
    do {
        write_result = write(device, &byte, sizeof(byte));
    } while ((write_result != -1) && (write_result != sizeof(byte)));

    if (write_result == sizeof(byte)) {
        tcflush(device, TCOFLUSH);
    } else {
        throw std::runtime_error("Error writing data to device: " + std::to_string(errno));
    }
}

#elif defined (_WIN32) || defined(_WIN64)
serial_device::serial_device(const std::string& path, const uint32_t speed) : device(INVALID_HANDLE_VALUE)
{

    device = CreateFileA(path.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (device != INVALID_HANDLE_VALUE) {
        DCB comm_state;
        if (GetCommState(device, &comm_state) == TRUE) {
            comm_state.BaudRate = speed;
            comm_state.ByteSize = sizeof(uint8_t) * 8;
            comm_state.DCBlength = sizeof(comm_state);
            comm_state.fBinary = TRUE;
            if (SetCommState(device, &comm_state) == FALSE) {
                CloseHandle(device);
                device = INVALID_HANDLE_VALUE;
            }
        } else {
            CloseHandle(device);
            device = INVALID_HANDLE_VALUE;
        }
    }
}

serial_device::~serial_device()
{
    if (device != INVALID_HANDLE_VALUE) {
        CloseHandle(device);
    }
}

bool serial_device::is_opened() const
{
    return device != INVALID_HANDLE_VALUE;
}

void serial_device::write_byte(const uint8_t byte) const
{
    if (!is_opened()) {
        throw std::logic_error("Device is not opened");
    }

    BOOL write_result;
    DWORD write_count;
    do {
        write_result = WriteFile(device, &byte, sizeof(byte), &write_count, NULL);
    } while ((write_result == TRUE) && (write_count != sizeof(byte)));

    if (write_result == TRUE) {
        FlushFileBuffers(device);
    } else {
        throw std::runtime_error("Error writing data to device: " + std::to_string(GetLastError()));
    }
}

#endif
