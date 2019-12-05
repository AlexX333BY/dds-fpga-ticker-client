#ifndef DDS_FPGA_TICKER_CLIENT_FPGA_TICKER_CLIENT_WX_APP_H
#define DDS_FPGA_TICKER_CLIENT_FPGA_TICKER_CLIENT_WX_APP_H


#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace fpga_ticker_client {
    class fpga_ticker_client_wx_app : public wxApp {
    public:
        bool OnInit() final;
    };
}


#endif //DDS_FPGA_TICKER_CLIENT_FPGA_TICKER_CLIENT_WX_APP_H
