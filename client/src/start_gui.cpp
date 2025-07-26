#include "gui.h"
#include "video.h"

StartMenuFrame::StartMenuFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxGridBagSizer* screenSizer;
	screenSizer = new wxGridBagSizer( 0, 0 );
	screenSizer->SetFlexibleDirection( wxBOTH );
	screenSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );


	screenSizer->Add( 100, 100, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxEXPAND, 5 );

	wxBoxSizer* input_boxes;
	input_boxes = new wxBoxSizer( wxHORIZONTAL );

	ip_input = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	ip_input->SetHint("IP Address");
	input_boxes->Add( ip_input, 0, wxALL, 5 );

	port_input = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	port_input->SetHint("Port No.");
	input_boxes->Add( port_input, 0, wxALL, 5 );


	screenSizer->Add( input_boxes, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );

	accept_btns = new wxStdDialogButtonSizer();
	accept_btnsOK = new wxButton( this, wxID_OK );
	accept_btns->AddButton( accept_btnsOK );
	accept_btnsCancel = new wxButton( this, wxID_CANCEL );
	accept_btns->AddButton( accept_btnsCancel );
	accept_btns->Realize();

	screenSizer->Add( accept_btns, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );


	this->SetSizer( screenSizer );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	accept_btnsCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( StartMenuFrame::clearFields ), NULL, this );
	accept_btnsOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( StartMenuFrame::connectServer ), NULL, this );
}

void StartMenuFrame::clearFields( wxCommandEvent& event ) {
	ip_input->SetValue(wxEmptyString);
	port_input->SetValue(wxEmptyString);
}
void StartMenuFrame::connectServer( wxCommandEvent& event ) {
	if(ip_input->GetValue() == wxEmptyString || port_input->GetValue() == wxEmptyString) {
		clearFields(event);
		return;
	}
	std::string _ip = ip_input->GetValue().ToStdString();
	int _portno = stoi(port_input->GetValue().ToStdString());
	
	sockaddr_in serv_addr = osf::create_address(std::move(_ip), std::move(_portno));
	int cl_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	osf::Client* new_client = new osf::Client(cl_fd, serv_addr, 6);
	
	if(!new_client->start_connect()) {
		clearFields(event);
		delete new_client;
		return;
	}
	
	new_client->start_recv();
	
	Interface* _gui = new Interface(nullptr, *new_client, wxID_ANY, "MHConnect");
	
	this->Enable(false);
	this->Hide();
	
	_gui->Show(true);
}
Interface* StartMenuFrame::getUserScreen() {
	return nullptr;
}

StartMenuFrame::~StartMenuFrame()
{
}