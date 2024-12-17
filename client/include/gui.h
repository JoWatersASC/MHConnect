#pragma once

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
#include <wx/listbox.h>
#include <wx/frame.h>

#include "client.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class MyFrame1
///////////////////////////////////////////////////////////////////////////////

namespace osf
{

class MyFrame1 : public wxFrame
{
	private:

	protected:
		wxStaticBitmap* vid_box;
		wxTextCtrl* inputField;
		wxButton* submitBtn;
		wxListBox* chat_list;

		// Virtual event handlers, override them in your derived class
		virtual void submit_msg( wxKeyEvent& event ) { event.Skip(); }
		virtual void submit_msg( wxMouseEvent& event ) { event.Skip(); }


	public:

		MyFrame1( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL|wxMODERN );

		~MyFrame1();

};

class Interface : public MyFrame1 {
public:
	Interface(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), 
		long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL|wxMODERN );

	
	Client* m_client;
	void add_text(std::string&);
	
private:
	
	virtual void submit_msg( wxKeyEvent& event ) override;
	virtual void submit_msg( wxMouseEvent& event ) override;
	
};

class Application : public wxApp{
	public:
		virtual bool OnInit() override;
};

}