#include "stdafx.h"
#include "FormSelectorMain.h"
#include "FormBase.h"
#include "FormGames.h"
#include "FormSagas.h"
#include "FormOptions.h"
#include "ConfigReader.h"

FormSelectorMain::FormSelectorMain(GuiManager* tray, GuiListener* oldListener /*= 0*/) : 
mTrayMgr(tray),
mOldListener(oldListener),
mFormCurrent(0),
mFormGames(0),
mFormSagas(0),
mFormOptions(0)
{
	mTrayMgr->setListener(this);
	mTrayMgr->assignListenerToMenuBar(this);
	mTrayMgr->loadMenuBarMain();

	// default form for info game
	mFormSagas = new FormSagas(ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Sagas", "./Sagas"), mTrayMgr, this);
	mFormSagas->hide();
	mFormSagas->disableForm();
	mFormOptions = new FormOptions(mTrayMgr, this);
	mFormOptions->hide();
	mFormOptions->disableForm();
	mFormGames = new FormGames(ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Games", "./Games"), mTrayMgr, this);
	mFormCurrent = mFormGames;
}


FormSelectorMain::~FormSelectorMain()
{
	if (mFormGames) delete mFormGames;
	if (mFormSagas) delete mFormSagas;
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
		if (!mFormSagas) mFormSagas = new FormSagas(ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Sagas", "./Sagas"), mTrayMgr, this);
		mFormCurrent = mFormSagas;
	}
	else if (menu->getSelectedItem() == "mbw_saga_home")
	{
		mFormSagas->removeSagaForm();
		mTrayMgr->loadMenuBarMain(1);
	}
	else if (menu->getSelectedItem() == "mbw_new_saga_back")
	{
		mFormSagas->removeNewSagaForm();
		mTrayMgr->loadMenuBarMain(1);
	}
	else if (menu->getSelectedItem() == "mbw_main_options")
	{
		if (!mFormOptions) mFormOptions = new FormOptions(mTrayMgr, this);
		mFormCurrent = mFormOptions;
	}
	else if (menu->getSelectedItem() == "mbw_new_game_back")
	{
		if (mFormGames) mFormGames->removeNewGameForm();
	}

	// apply the new form
	if (mFormCurrent)
	{
		previousForm->hide();
		previousForm->disableForm();
		mFormCurrent->enableForm();
		mFormCurrent->show();
	}
}

