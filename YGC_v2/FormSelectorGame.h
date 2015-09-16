#ifndef _FORMSELECTORGAME_H__
#define _FORMSELECTORGAME_H__

#include "GuiManager.h"
#include "GameInfo.h"

class FormBase;
class FormOverview;
class FormReview;
class FormModels;
class FormImages;
class FormVideos;
class FormMusic;
class FormCollector;
class FormPlay;

class FormSelectorGame : public GuiListener
{
public:
	FormSelectorGame(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener = 0);
	~FormSelectorGame();

	bool isFinished() { return (mFormCurrent == 0); }

	void menuBarItemHit(MenuBar* menu);

private:
	void _loadRandomWallpaper();

	GuiManager* mTrayMgr;
	GameInfo* mGameInfo;
	FormBase* mFormCurrent;
	FormOverview* mFormOverview;
	FormReview* mFormReview;
	FormModels* mFormModels;
	FormImages* mFormImages;
	FormVideos* mFormVideos;
	FormMusic* mFormMusic;
	FormCollector* mFormCollector;
	FormPlay* mFormPlay;
};

#endif // #ifndef _FORMSELECTORGAME_H__

