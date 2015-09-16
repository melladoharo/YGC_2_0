#include "stdafx.h"
#include "FormPlay.h"


FormPlay::FormPlay(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener /*= 0*/) :
FormBase(tray, oldListener),
mGameInfo(gInfo)
{

}

FormPlay::~FormPlay()
{
	hideAllOptions();
}



bool FormPlay::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;
	return FormBase::mouseMoved(arg);
}

bool FormPlay::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;
	return FormBase::mousePressed(arg, id);
}

bool FormPlay::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;
	return FormBase::mouseReleased(arg, id);
}



void FormPlay::hide()
{
	FormBase::hide();
	Widget* logoGame = mTrayMgr->getWidget("FormOverview/Logo");
	if (logoGame) logoGame->hide();
	hideAllOptions();
}

void FormPlay::show()
{
	FormBase::show();
	Widget* logoGame = mTrayMgr->getWidget("FormOverview/Logo");
	if (logoGame) logoGame->show();
	showOptions();
}



void FormPlay::buttonHit(Button* button)
{
	if (button->getName() == "FormPlay/Button/CloseOptions")
	{
		hideOptions();
	}
	else if (button->getName() == "FormPlay/Button/ClosePathGame")
	{
		hideOptionsEditPath();
		showOptions();
	}
	else if (button->getName() == "FormPlay/Button/SelectPathGame")
	{
		FileExplorer* explorer = dynamic_cast<FileExplorer*>(mTrayMgr->getWidget("FormPlay/Explorer/PathGame"));
		if (explorer)
		{
			Ogre::String newPath = explorer->getSelectedFile();
			if (boost::filesystem::is_regular_file(newPath))
			{
				mGameInfo->setValueOption("GAME.PATHS", "GamePath", newPath);
			}
		}
		hideOptionsEditPath();
		showOptions();
	}
}

void FormPlay::labelHit(Label* label)
{
	if (label->getName() == "FormPlay/Label/PlayGame")
	{
		Ogre::String pathGame = mGameInfo->getValueOption("GAME.PATHS", "GamePath", "./Games");
		if (boost::filesystem::is_regular_file(pathGame))
		{
			mTrayMgr->lauchGame(pathGame);
		}
	}
	else if (label->getName() == "FormPlay/Label/EditPath")
	{
		hideOptions();
		showOptionsEditPath();
	}
}



void FormPlay::hideAllOptions()
{
	hideOptions();
	hideOptionsEditPath();
}

void FormPlay::hideOptions()
{
	if (mTrayMgr->getWidget("FormPlay/Window/Options"))
	{
		mTrayMgr->destroyDialogWindow("FormPlay/Window/Options");
		mTrayMgr->destroyWidget("FormPlay/Label/PlayGame");
		mTrayMgr->destroyWidget("FormPlay/Label/EditPath");
		mTrayMgr->destroyWidget("FormPlay/Button/CloseOptions");
	}
}

void FormPlay::showOptions()
{
	// all submenu options are hiddens?
	if (!mTrayMgr->isWindowDialogVisible())
	{
		unsigned int numOptions = 3;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 350;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		mTrayMgr->createDialogWindow("FormPlay/Window/Options", "PLAY OPTIONS", left, top, width, height);	top += sepOptions / 2;
		mTrayMgr->createLabel("FormPlay/Label/PlayGame", "PLAY GAME", left, top, width, 23); top += sepOptions;
		mTrayMgr->createLabel("FormPlay/Label/EditPath", "EDIT PATH GAME", left, top, width, 23); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormPlay/Button/CloseOptions", "BACK", left, top, 60);
	}
}

void FormPlay::hideOptionsEditPath()
{
	if (mTrayMgr->getWidget("FormPlay/Window/PathGame"))
	{
		mTrayMgr->destroyDialogWindow("FormPlay/Window/PathGame");
		mTrayMgr->destroyWidget("FormPlay/Explorer/PathGame");
		mTrayMgr->destroyWidget("FormPlay/Button/ClosePathGame");
		mTrayMgr->destroyWidget("FormPlay/Button/SelectPathGame");
	}
}

void FormPlay::showOptionsEditPath()
{
	if (!mTrayMgr->isWindowDialogVisible()) // menu is hidden
	{
		unsigned int numOptions = 11;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 560;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		Ogre::String pathGame = mGameInfo->getValueOption("GAME.PATHS", "GamePath", "./Games");

		mTrayMgr->createDialogWindow("FormPlay/Window/PathGame", "PATH GAME", left, top, width, height);	 top += sepOptions / 2;
		mTrayMgr->createFileExplorer("FormPlay/Explorer/PathGame", pathGame, left, top, width, height); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormPlay/Button/ClosePathGame", "CANCEL", left, top, 60);
		mTrayMgr->createButton("FormPlay/Button/SelectPathGame", "SELECT", left + 70, top, 80);
	}
}

