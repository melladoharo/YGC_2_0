#include "stdafx.h"
#include "FormSelectorMain.h"
#include "FormBase.h"
#include "FormGames.h"
#include "FormOptions.h"
#include "ConfigReader.h"

FormSelectorMain::FormSelectorMain(GuiManager* tray, GuiListener* oldListener /*= 0*/) : 
mTrayMgr(tray),
mOldListener(oldListener),
mFormCurrent(0),
mFormGames(0),
mFormOptions(0)
{
	mTrayMgr->setListener(this);
	mTrayMgr->assignListenerToMenuBar(this);
	mTrayMgr->loadMenuBarMain();

	// default form for info game
	mFormGames = new FormGames(ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Games", "./Games"), mTrayMgr, this);
	mFormCurrent = mFormGames;
}


FormSelectorMain::~FormSelectorMain()
{
	if (mFormGames) delete mFormGames;
	if (mFormOptions) delete mFormOptions;
}

void FormSelectorMain::menuBarItemHit(MenuBar* menu)
{
	FormBase* previousForm = mFormCurrent;

	if (menu->getSelectedItem() == "mbw_main_exit")
	{
		mFormCurrent = previousForm = 0;
	}
	else if (menu->getSelectedItem() == "mbw_main_games")
	{
		if (!mFormGames) mFormGames = new FormGames(ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Games", "./Games"), mTrayMgr, this);
		mFormCurrent = mFormGames;
	}
	else if (menu->getSelectedItem() == "mbw_main_sagas")
	{

	}
	else if (menu->getSelectedItem() == "mbw_main_options")
	{
		if (!mFormOptions) mFormOptions = new FormOptions(mTrayMgr, this);
		mFormCurrent = mFormOptions;
	}

	// apply the new form
	if (mFormCurrent)
	{
		previousForm->hide();
		//previousForm->disableForm(); // some forms need to be update every frame
		mFormCurrent->enableForm();
		mFormCurrent->show();
	}
}

