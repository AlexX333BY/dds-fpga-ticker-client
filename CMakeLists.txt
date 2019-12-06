cmake_minimum_required(VERSION 3.13)
project(dds_fpga_ticker_client)

find_package(wxWidgets REQUIRED COMPONENTS
    base
    core
)
include(${wxWidgets_USE_FILE})

set(HEADERS
    fpga_sender.h
    fpga_ticker_client_wx_app.h
    fpga_ticker_client_wx_frame.h
    send_event.h
    serial_device.h
)

set(SOURCES
    main.cpp
    fpga_sender.cpp
    fpga_ticker_client_wx_app.cpp
    fpga_ticker_client_wx_frame.cpp
    send_event.cpp
    serial_device.cpp
)

add_executable(dds_fpga_ticker_client ${HEADERS} ${SOURCES})
target_link_libraries(dds_fpga_ticker_client
    ${wxWidgets_LIBRARIES}
)
set_target_properties(dds_fpga_ticker_client PROPERTIES OUTPUT_NAME ticker-client)
