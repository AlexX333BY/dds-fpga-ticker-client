#include "fpga_ticker_client_wx_frame.h"
#include <vector>
#include <wx/valnum.h>

using namespace fpga_ticker_client;

fpga_ticker_client_wx_frame::fpga_ticker_client_wx_frame()
    : wxFrame(nullptr, wxID_ANY, "FPGA ticker"), sending_thread(), sender()
{
    const uint8_t border = 10, gap = 5;

    panel = new wxPanel(this);

    auto input_sizer = new wxFlexGridSizer(3, 2, gap, gap);
    input_sizer->AddGrowableCol(1, 1);

    device_input = new wxTextCtrl(panel, wxID_ANY);
    text_input = new wxTextCtrl(panel, wxID_ANY);
    period_input = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,
        wxIntegerValidator<uint32_t>());

    const std::vector<std::pair<wxString, wxTextCtrl*>> inputs = {
        { "FPGA device name", device_input },
        { "Text to send", text_input },
        { "Period for each character", period_input }
    };

    for (size_t input_no = 0; input_no < inputs.size(); ++input_no) {
        input_sizer->Add(new wxStaticText(panel, wxID_ANY, inputs[input_no].first), 0, wxALIGN_CENTER);
        auto sizer = new wxBoxSizer(wxHORIZONTAL);
        sizer->Add(inputs[input_no].second, 1, wxALIGN_CENTER);
        input_sizer->Add(sizer, 0, wxEXPAND);
        input_sizer->AddGrowableRow(input_no, 1);
    }

    start_button = new wxButton(panel, start_button_id, "Start");
    start_button->Enable(true);
    stop_button = new wxButton(panel, stop_button_id, "Stop");
    stop_button->Enable(false);
    auto buttons_sizer = new wxBoxSizer(wxHORIZONTAL);
    buttons_sizer->Add(start_button, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER, border);
    buttons_sizer->Add(stop_button, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER, border);

    auto panel_sizer = new wxBoxSizer(wxVERTICAL);
    panel_sizer->Add(input_sizer, 1, wxALL | wxEXPAND, border);
    panel_sizer->Add(buttons_sizer, 1, wxALL, border);
    panel->SetSizer(panel_sizer);

    Bind(wxEVT_BUTTON, &fpga_ticker_client_wx_frame::on_start_sending, this, start_button_id);
    Bind(wxEVT_BUTTON, &fpga_ticker_client_wx_frame::on_stop_sending, this, stop_button_id);
    Bind(DEVICE_OPEN_ERROR_EVENT, &fpga_ticker_client_wx_frame::on_device_open_failure, this);
    Bind(DATA_SEND_ERROR_EVENT, &fpga_ticker_client_wx_frame::on_data_send_error, this);
    Bind(SEND_STOPPED_EVENT, &fpga_ticker_client_wx_frame::on_send_stop, this);
}

void fpga_ticker_client_wx_frame::enable_inputs(const bool enable)
{
    device_input->Enable(enable);
    text_input->Enable(enable);
    period_input->Enable(enable);
    start_button->Enable(enable);
    stop_button->Enable(!enable);
}

void fpga_ticker_client_wx_frame::on_start_sending(wxCommandEvent &event)
{
    if (event.GetId() == start_button_id) {
        if (panel->Validate()) {
            enable_inputs(false);
            unsigned long period;
            period_input->GetValue().ToULong(&period, 10);
            sending_thread = std::make_unique<std::thread>(&fpga_ticker_client_wx_frame::sending_routine, this,
                device_input->GetValue().ToStdString(), text_input->GetValue().ToStdString(),
                std::chrono::milliseconds(period));
        }
    } else {
        event.Skip();
    }
}

void fpga_ticker_client_wx_frame::on_stop_sending(wxCommandEvent &event)
{
    if (event.GetId() == stop_button_id) {
        sender->stop();
    } else {
        event.Skip();
    }
}

void fpga_ticker_client_wx_frame::sending_routine(const std::string& device_name, const std::string& text,
    const std::chrono::milliseconds& period)
{
    send_event* event;
    sender = std::make_unique<fpga_sender>(device_name);
    if (sender->is_opened()) {
        try {
            sender->send(text, period);
            event = new send_event("", this->GetId(), SEND_STOPPED_EVENT);
        } catch (const std::exception& e) {
            event = new send_event(e.what(), this->GetId(), DATA_SEND_ERROR_EVENT);
        }
    } else {
        event = new send_event("", this->GetId(), DEVICE_OPEN_ERROR_EVENT);
    }
    event->SetEventObject(this);
    QueueEvent(event);
}

void fpga_ticker_client_wx_frame::on_device_open_failure(send_event& event)
{
    const std::string& message = event.get_message();
    wxMessageBox("Error opening device" + (!message.empty() ? ": " + message : ""), "Error", wxICON_ERROR);
    sending_thread->join();
    sending_thread.reset();
    sender.reset();
    enable_inputs(true);
}

void fpga_ticker_client_wx_frame::on_data_send_error(send_event& event)
{
    const std::string& message = event.get_message();
    wxMessageBox("Error sending data to device" + (!message.empty() ? ": " + message : ""), "Error", wxICON_ERROR);
    sending_thread->join();
    sending_thread.reset();
    sender.reset();
    enable_inputs(true);
}

void fpga_ticker_client_wx_frame::on_send_stop(send_event&)
{
    sending_thread->join();
    sending_thread.reset();
    sender.reset();
    enable_inputs(true);
}

fpga_ticker_client_wx_frame::~fpga_ticker_client_wx_frame()
{
    if (sender) {
        sender->stop();
    }
    if (sending_thread && sending_thread->joinable()) {
        sending_thread->join();
    }
}
