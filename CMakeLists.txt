cmake_minimum_required(VERSION 3.13)
project(dds_fpga_ticker_client)

find_package(wxWidgets REQUIRED COMPONENTS
    base
    core
)
include(${wxWidgets_USE_FILE})

set(HEADERS
    fpga_sender.h
)

set(SOURCES
    main.cpp
    fpga_sender.cpp
)

add_executable(dds_fpga_ticker_client ${HEADERS} ${SOURCES})
target_link_libraries(dds_fpga_ticker_client
    ${wxWidgets_LIBRARIES}
)
set_target_properties(dds_fpga_ticker_client PROPERTIES OUTPUT_NAME ticker-client)
