#include "fpga_ticker_client_wx_app.h"
#include "fpga_ticker_client_wx_frame.h"

using namespace fpga_ticker_client;

bool fpga_ticker_client_wx_app::OnInit()
{
    (new fpga_ticker_client_wx_frame())->Show(true);
    return true;
}
