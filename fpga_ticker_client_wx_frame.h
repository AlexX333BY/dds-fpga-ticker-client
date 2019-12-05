#ifndef DDS_FPGA_TICKER_CLIENT_FPGA_TICKER_CLIENT_WX_FRAME_H
#define DDS_FPGA_TICKER_CLIENT_FPGA_TICKER_CLIENT_WX_FRAME_H


#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <thread>
#include <memory>
#include <string>
#include <chrono>
#include "send_event.h"
#include "fpga_sender.h"

namespace fpga_ticker_client {
    class fpga_ticker_client_wx_frame : public wxFrame {
    public:
        fpga_ticker_client_wx_frame();
        ~fpga_ticker_client_wx_frame() final;

    private:
        void on_start_sending(wxCommandEvent& event);
        void on_stop_sending(wxCommandEvent& event);
        void on_device_open_failure(send_event& event);
        void on_data_send_error(send_event& event);
        void on_send_stop(send_event& event);
        void sending_routine(const std::string& device_name, const std::string& text, const std::chrono::milliseconds& period);
        void enable_inputs(const bool enable = true);

        wxPanel* panel;
        wxTextCtrl* device_input, * text_input, * period_input;
        wxButton* start_button, * stop_button;
        std::unique_ptr<std::thread> sending_thread;
        std::unique_ptr<fpga_sender> sender;

        static const wxWindowID start_button_id = wxID_OK, stop_button_id = wxID_STOP;
    };
}


#endif //DDS_FPGA_TICKER_CLIENT_FPGA_TICKER_CLIENT_WX_FRAME_H
