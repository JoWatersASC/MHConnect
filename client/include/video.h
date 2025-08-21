#pragma once

#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <wx/wx.h>

#include <thread>

namespace osf
{

inline bool capture(cv::VideoCapture&, cv::Mat&);
inline std::pair<int, int> getScaleOfFrame(const int, const int, const cv::Mat&);

class VidPanel : public wxPanel {
public:
	VidPanel(wxPanel* parent, int _cam_id)
		: wxPanel(parent) {

		m_cap.open(_cam_id);
		if (!m_cap.isOpened()) {
			wxMessageBox(wxString::Format("Could not open camera %d!", _cam_id),
						"Error", wxICON_ERROR);
			return;
		}

		//timer_ = new wxTimer(this);
		//Bind(wxEVT_PAINT, &VidPanel::OnPaint, this);
		//Bind(wxEVT_TIMER, &VidPanel::OnTimer, this);
		Bind(wxEVT_THREAD, &VidPanel::OnFrameReady, this, ID_NUMBER);
		Bind(wxEVT_PAINT, &VidPanel::OnPaint, this);
		//timer_->Start(45);
		cap_thread = std::thread(&VidPanel::CaptureLoop, this);
	}

	~VidPanel() {
		/*if (timer_) {
			timer_->Stop();
			delete timer_;
		}*/
		m_cap.release();
	}

private:
	enum { ID_NUMBER = wxID_HIGHEST + 1 };

	cv::VideoCapture m_cap;
	std::thread cap_thread;
	//wxTimer* timer_;
	cv::Mat m_frame;
	std::mutex m_mtx;
	bool running_ = true;

	/*void OnTimer(wxTimerEvent& event) {
	}
	*/
	void OnPaint() {}
	void OnPaint(wxPaintEvent& e) {
		wxPaintDC dc(this);
		if (m_frame.empty()) return;

		std::lock_guard<std::mutex> lock(m_mtx);
		/*if (capture(m_cap, m_frame))
			Refresh();*/

		//std::thread([this]() { OnPaint(); }).detach();

		wxSize panel_size = GetSize();
		const int p_width = panel_size.GetWidth();
		const int p_height = panel_size.GetHeight();
		
		auto [_width, _height] = getScaleOfFrame(p_width, p_height, m_frame);

		int x_offset = (p_width - _width) / 2;
		int y_offset = (p_height - _height) / 2;

		wxImage wx_image(m_frame.cols, m_frame.rows, m_frame.data, true);
		wxBitmap bitmap(wx_image.Scale(_width, _height, wxIMAGE_QUALITY_HIGH));
		dc.DrawBitmap(bitmap, x_offset, y_offset, false);
	}/**/

	void CaptureLoop() {
		while (running_) {
			// std::lock_guard<std::mutex> lock(m_mtx);
			if (m_cap.read(m_frame)) {
				cv::cvtColor(m_frame, m_frame, cv::COLOR_BGR2RGB);
				wxQueueEvent(this, new wxCommandEvent(wxEVT_THREAD, ID_NUMBER));
			}
		}
	}
	void OnFrameReady(wxThreadEvent& event) {
		Refresh();
	}
};


bool capture(cv::VideoCapture& _cap, cv::Mat& _frame) {
	if (_cap.read(_frame)) {
		cv::cvtColor(_frame, _frame, cv::COLOR_BGR2RGB);
		return true;
	}
	
	return false;
}
std::pair<int, int> getScaleOfFrame(const int _w, const int _h, const cv::Mat& _frame) {
	double scale = std::min(
		(double)_w / _frame.cols,
		(double)_h / _frame.rows
	);
	
	return { (int)(scale * _w), (int)(scale * _h) };
}

	
}
