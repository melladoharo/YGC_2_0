#ifndef _FORMSELECTORMAIN_H__
#define _FORMSELECTORMAIN_H__

#include "GuiManager.h"

class FormBase;
class FormGames;
class FormSagas;
class FormOptionMain;

class FormSelectorMain : public GuiListener
{
public:
	FormSelectorMain(GuiManager* tray, GuiListener* oldListener = 0);
	~FormSelectorMain();

	bool isFinished() { return (mFormCurrent == 0); }

	void menuBarItemHit(MenuBar* menu);

private:
	GuiManager* mTrayMgr;
	GuiListener* mOldListener;
	FormBase* mFormCurrent;
	FormGames* mFormGames;
};

#endif // #ifndef _FORMSELECTORMAIN_H__

