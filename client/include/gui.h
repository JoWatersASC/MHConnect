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


class Interface : public MainFrame {
public:
	Interface(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = "MHConnect",
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), 
		long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL|wxMODERN );

	
	osf::Client* m_client;
	void add_text(std::string&);
	
private:
	

	virtual void submit_msg(wxCommandEvent& event);
	
};

class Application : public wxApp{
	public:
		virtual bool OnInit() override;
};
