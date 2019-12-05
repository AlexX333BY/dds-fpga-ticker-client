#ifndef DDS_FPGA_TICKER_CLIENT_SEND_EVENT_H
#define DDS_FPGA_TICKER_CLIENT_SEND_EVENT_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>

namespace fpga_ticker_client {
    class send_event : public wxEvent {
    public:
        explicit send_event(const std::string& message, int winId = 0, wxEventType eventType = wxEVT_NULL);

        wxEvent* Clone() const final;
        const std::string& get_message() const;

    private:
        const std::string message;
    };
}

wxDEFINE_EVENT(DEVICE_OPEN_ERROR_EVENT, fpga_ticker_client::send_event);
wxDEFINE_EVENT(DATA_SEND_ERROR_EVENT, fpga_ticker_client::send_event);
wxDEFINE_EVENT(SEND_STOPPED_EVENT, fpga_ticker_client::send_event);


#endif //DDS_FPGA_TICKER_CLIENT_SEND_EVENT_H
