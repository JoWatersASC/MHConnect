#include "gui.h"

using namespace osf;

MyFrame1::MyFrame1( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) 
	: wxFrame( parent, id, title, pos, size, style ) {
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* MainBox;
	MainBox = new wxBoxSizer( wxHORIZONTAL );

	MainBox->SetMinSize( wxSize( 800,600 ) );
	wxBoxSizer* ClientBox;
	ClientBox = new wxBoxSizer( wxVERTICAL );

	ClientBox->SetMinSize( wxSize( 550,500 ) );
	vid_box = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	vid_box->SetMinSize( wxSize( 500,500 ) );

	ClientBox->Add( vid_box, 0, wxALL, 5 );

	wxBoxSizer* input_bar;
	input_bar = new wxBoxSizer( wxHORIZONTAL );

	inputField = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	inputField->SetMinSize( wxSize( 450,50 ) );

	input_bar->Add( inputField, 0, wxALL, 5 );

	submitBtn = new wxButton( this, wxID_ANY, _("Send"), wxDefaultPosition, wxDefaultSize, 0 );
	input_bar->Add( submitBtn, 0, wxALL, 5 );


	ClientBox->Add( input_bar, 1, wxEXPAND, 5 );


	MainBox->Add( ClientBox, 1, wxEXPAND, 5 );

	chat_list = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	chat_list->SetMinSize( wxSize( 250,550 ) );

	MainBox->Add( chat_list, 0, wxALL, 5 );


	this->SetSizer( MainBox );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	inputField->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( MyFrame1::submit_msg ), NULL, this );
	submitBtn->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( MyFrame1::submit_msg ), NULL, this );
}

MyFrame1::~MyFrame1()
{
}

Interface::Interface(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
	: MyFrame1(parent, id, title, pos, size, style) {}
	
void Interface::submit_msg(wxKeyEvent& event) {
	if(!inputField->GetValue().IsEmpty()) {
		if(event.GetKeyCode() != WXK_RETURN)
			return;
		
		std::string text = inputField->GetValue().ToStdString();
		m_client->start_send(text);
		inputField->Clear();
	}
}
void Interface::submit_msg(wxMouseEvent& event) {
	if(!inputField->GetValue().IsEmpty()) {
		std::string text = inputField->GetValue().ToStdString();
		m_client->start_send(text);
		inputField->Clear();
	}
}
void Interface::add_text(std::string& _msg) {
	this->chat_list->Append(_msg);
}

bool Application::OnInit() {
#ifdef _WIN32
	if (winsock_initialize())
		return 1;
#endif

	int cl_fd = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in serv_addr;

	serv_addr = create_address("127.0.0.1", 16000);

	Client firstc(cl_fd, serv_addr, 6);

	firstc.start_connect();
	firstc.start_recv();

	Interface* _gui = new Interface(nullptr);
	_gui->Show(true);

	_gui->m_client = &firstc;
	firstc.m_gui = _gui;

#ifdef _WIN32
	WSACleanup();
#endif
}