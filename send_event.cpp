#include "send_event.h"

using namespace fpga_ticker_client;

send_event::send_event(const std::string& message, int winId, wxEventType eventType)
    : wxEvent(winId, eventType), message(message)
{ }

wxEvent* send_event::Clone() const
{
    return new send_event(*this);
}

const std::string& send_event::get_message() const
{
    return message;
}
