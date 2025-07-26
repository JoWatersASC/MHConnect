#include "gui.h"
#include "video.h"

MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxHORIZONTAL);

	bSizer1->SetMinSize(wxSize(800, 600));
	m_usrpanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* susr_box;
	susr_box = new wxBoxSizer(wxVERTICAL);
	
	
	
	cvid_panel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	cvid_panel->SetMinSize(wxSize(500, 500));
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 0, 0, 0, 0 );

	/*m_bitmap1 = new wxStaticBitmap( cvid_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer1->Add( m_bitmap1, 0, wxALL, 5 );*/

	osf::VidPanel* vp = new osf::VidPanel(cvid_panel, 0);

	cvid_panel->SetSizer( gSizer1 );
	cvid_panel->Layout();
	gSizer1->Fit( cvid_panel );
	susr_box->Add( cvid_panel, 8, wxEXPAND | wxALL, 5 );
	
	gSizer1->Add(vp, 0, wxALL | wxEXPAND, 5);
	

	// m_bitmap1 = new wxStaticBitmap(m_usrpanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0);
	// susr_box->Add(m_bitmap1, 8, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	wxBoxSizer* input_bar;
	input_bar = new wxBoxSizer(wxHORIZONTAL);

	m_txtctrl = new wxTextCtrl(m_usrpanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	m_txtctrl->SetMinSize(wxSize(450, 50));

	input_bar->Add(m_txtctrl, 6, wxEXPAND | wxALL, 5);

	m_sendbtn = new wxButton(m_usrpanel, wxID_ANY, _("Send"), wxDefaultPosition, wxDefaultSize, 0);
	m_sendbtn->SetMinSize(wxSize(75, -1));

	input_bar->Add(m_sendbtn, 1, wxALIGN_CENTER | wxALL, 5);

	susr_box->Add(input_bar, 1, wxEXPAND, 5);


	m_usrpanel->SetSizerAndFit(susr_box);
	m_usrpanel->Layout();
	
	bSizer1->Add(m_usrpanel, 3, wxEXPAND | wxALL, 5);

	m_panel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_panel2->SetMinSize(wxSize(250, 550));

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer(new wxStaticBox(m_panel2, wxID_ANY, _("Chat List")), wxVERTICAL);

	m_chatbox = new wxTextCtrl(sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE | wxTE_READONLY | wxTE_WORDWRAP);
	m_chatbox->SetMinSize(wxSize(250, 550));

	sbSizer2->Add(m_chatbox, 1, wxALL | wxEXPAND, 5);


	m_panel2->SetSizerAndFit(sbSizer2);
	m_panel2->Layout();
	//sbSizer2->Fit(m_panel2);
	bSizer1->Add(m_panel2, 2, wxEXPAND | wxALL, 5);


	this->SetSizerAndFit(bSizer1);
	this->Layout();

	this->Centre(wxBOTH);

	// Connect Events
	m_txtctrl->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(MainFrame::submit_msg), NULL, this);
	m_sendbtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::submit_msg), NULL, this);
}

MainFrame::~MainFrame()
{
}
Interface::Interface(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
	: MainFrame(parent, id, title, pos, size, style) {}
	
Interface::Interface(wxWindow* parent, osf::Client& _client, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
	: MainFrame(parent, id, title, pos, size, style) {
		linkClient(_client);
	}
	
	
void Interface::submit_msg(wxCommandEvent& event) {
	if (!m_txtctrl->GetValue().IsEmpty()) {
		std::string text = m_txtctrl->GetValue().ToStdString();

		m_client->start_send(text);
		text = "[YOU] " + text;
		add_text(text);

		m_txtctrl->Clear();
	}
}

void Interface::add_text(std::string& _msg) {
	this->m_chatbox->AppendText(_msg);
	this->m_chatbox->AppendText('\n');
	this->m_chatbox->ShowPosition(m_chatbox->GetLastPosition());
}


bool Application::OnInit() {
#ifdef _WIN32
	if (winsock_initialize())
		return 1;
#endif
	
	StartMenuFrame* start_menu = new StartMenuFrame(nullptr, wxID_ANY, "MHConnect");
	start_menu->Show(true);
	SetTopWindow(start_menu);

	return true;

#ifdef _WIN32
	WSACleanup();
#endif
}