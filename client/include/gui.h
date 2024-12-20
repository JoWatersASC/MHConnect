#pragma once

#include "client.h"

#include <wx/wx.h>

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/statbox.h>
#include <wx/frame.h>
#include <wx/gbsizer.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame
{
private:

protected:
	wxPanel* m_usrpanel;
	wxStaticBitmap* m_bitmap1;
	wxTextCtrl* m_txtctrl;
	wxButton* m_sendbtn;
	wxPanel* m_panel2;
	wxTextCtrl* m_chatbox;

	// Virtual event handlers, override them in your derived class
	virtual void submit_msg(wxCommandEvent& event) { event.Skip(); }


public:

	MainFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(800, 600), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

	~MainFrame();

};


class Interface : public MainFrame, public osf::Client::listener {
public:
	Interface(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = "MHConnect",
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), 
		long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL|wxMODERN );
		
	//To be constructed with a client
	Interface(wxWindow* parent, osf::Client&, wxWindowID id = wxID_ANY, const wxString& title = "MHConnect",
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), 
		long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL|wxMODERN);

	void linkClient(osf::Client& _client) {
		m_client = &_client;
		m_client->addListener(this);
	}

	void onNotify(const packet& _data) {
		std::string msg = "[MSG] " + std::string(_data.data);
		add_text(msg);
	}
	void add_text(std::string&);
	
private:
	osf::Client* m_client;
	virtual void submit_msg(wxCommandEvent& event);
	
};

class StartMenuFrame : public wxFrame
{
	private:
		Interface* getUserScreen();
		
	protected:
		wxTextCtrl* ip_input;
		wxTextCtrl* port_input;
		wxStdDialogButtonSizer* accept_btns;
		wxButton* accept_btnsOK;
		wxButton* accept_btnsCancel;

		// Virtual event handlers, override them in your derived class
		virtual void clearFields( wxCommandEvent& event );
		virtual void connectServer( wxCommandEvent& event );


	public:

		StartMenuFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~StartMenuFrame();

};

class Application : public wxApp{
	public:
		virtual bool OnInit() override;
};
