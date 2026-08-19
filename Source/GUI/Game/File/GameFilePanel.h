#ifndef _GAME_FILE_PANEL_H_
#define _GAME_FILE_PANEL_H_

#include <boost/signals2.hpp>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

class GameFile;

class GameFilePanel : public wxPanel {
public:
	GameFilePanel(std::shared_ptr<GameFile> gameFile, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER, const std::string & name = {});
	~GameFilePanel() override;

	virtual std::string getPanelName() const = 0;
	std::shared_ptr<const GameFile> getGameFile() const;
	std::shared_ptr<GameFile> getGameFile();
	bool isModified() const;
	virtual void update() = 0;

	boost::signals2::signal<void (GameFilePanel & /* gameFilePanel*/)> gameFileModified;

private:
	void onGameFileModified(const GameFile & gameFile);

	std::shared_ptr<GameFile> m_gameFile;
	boost::signals2::connection m_gameFileModifiedConnection;

	GameFilePanel(const GameFilePanel &) = delete;
	const GameFilePanel & operator = (const GameFilePanel &) = delete;
};

#endif // _GAME_FILE_PANEL_H_
