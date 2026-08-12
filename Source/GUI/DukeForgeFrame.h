#ifndef _DUKE_FORGE_FRAME_H_
#define _DUKE_FORGE_FRAME_H_

#include <boost/signals2.hpp>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/bookctrl.h>

#include <memory>

class DukeForge;

class DukeForgeFrame final : public wxFrame {
public:
	DukeForgeFrame();
	~DukeForgeFrame() override;

	bool isInitialized() const;
	bool initialize(std::shared_ptr<DukeForge> dukeForge);

#if wxUSE_MENUS
	void onMenuBarItemPressed(wxCommandEvent & event);
#endif // wxUSE_MENUS

	boost::signals2::signal<void ()> reloadRequested;

private:
	void requestReload();
	void onNotebookPageChanging(wxBookCtrlEvent & event);
	void onNotebookPageChanged(wxBookCtrlEvent & event);
	void onQuit(wxCommandEvent & event);
	void onAbout(wxCommandEvent & event);

#if wxUSE_MENUS
	wxMenuItem * m_resetWindowPositionMenuItem;
	wxMenuItem * m_resetWindowSizeMenuItem;
#endif // wxUSE_MENUS

	bool m_initialized;
	wxNotebook * m_notebook;

	DukeForgeFrame(const DukeForgeFrame &) = delete;
	const DukeForgeFrame & operator = (const DukeForgeFrame &) = delete;

	wxDECLARE_EVENT_TABLE();
};

#endif // _DUKE_FORGE_FRAME_H_
