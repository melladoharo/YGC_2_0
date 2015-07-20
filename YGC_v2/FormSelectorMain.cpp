#include "stdafx.h"
#include "FormSelectorMain.h"
#include "FormBase.h"
#include "FormGames.h"

FormSelectorMain::FormSelectorMain(GuiManager* tray, GuiListener* oldListener /*= 0*/) : 
mTrayMgr(tray),
mOldListener(oldListener),
mFormCurrent(0),
mFormGames(0)
{
	mTrayMgr->setListener(this);
	mTrayMgr->assignListenerToMenuBar(this);
	mTrayMgr->loadMenuBarMain();

	// default form for info game
	mFormGames = new FormGames("D:/Programas/Juegos - Info DEBUG", mTrayMgr, this);
	mFormCurrent = mFormGames;
}


FormSelectorMain::~FormSelectorMain()
{
	if (mFormGames) delete mFormGames;
}

void FormSelectorMain::menuBarItemHit(MenuBar* menu)
{
	if (menu->getSelectedItem() == "mbw_main_exit")
	{
		mTrayMgr->shutDownApp();
	}
	else if (menu->getSelectedItem() == "mbw_main_games")
	{
		if (mFormCurrent) mFormCurrent->hide();
		if (!mFormGames)
			mFormGames = new FormGames("D:/Programas/Juegos - Info DEBUG", mTrayMgr, this);
		mFormCurrent = mFormGames;
		mFormCurrent->show();
	}
	else if (menu->getSelectedItem() == "mbw_main_sagas")
	{

	}
}

