#ifndef _DUKE_FORGE_APPLICATION_H_
#define _DUKE_FORGE_APPLICATION_H_

#include "Application/DukeForge.h"
#include "DukeForgeFrame.h"

#include <Arguments/ArgumentParser.h>

#include <boost/signals2.hpp>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <memory>

class DukeForgeInitializationDoneEvent;

class DukeForgeApplication : public wxApp {
public:
	DukeForgeApplication();
	~DukeForgeApplication() override;

	void reload();
	void displayArgumentHelp();

	void onFrameClosed(wxCloseEvent & event);

	// wxApp Virtuals
	bool OnInit() override;
	int OnExit() override;
	void CleanUp() override;

private:
	void initialize();
	void showWindow();
	void onInitializationDone(DukeForgeInitializationDoneEvent & event);
	void onReloadRequested();

	std::shared_ptr<ArgumentParser> m_arguments;
	std::shared_ptr<DukeForge> m_dukeForge;
	DukeForgeFrame * m_dukeForgeFrame;
	DukeForgeFrame * m_newDukeForgeFrame;
	boost::signals2::connection m_dukeForgeFrameReloadRequestedConnection;
	bool m_reloadRequired;
	boost::signals2::connection m_dukeForgeInitializationProgressConnection;

	DukeForgeApplication(const DukeForgeApplication &) = delete;
	const DukeForgeApplication & operator = (const DukeForgeApplication &) = delete;
};

#endif // _DUKE_FORGE_APPLICATION_H_
