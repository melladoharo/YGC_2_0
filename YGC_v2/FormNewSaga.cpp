#include "stdafx.h"
#include "FormNewSaga.h"

FormNewSaga::FormNewSaga(GuiManager* tray, GuiListener* oldListener /*= 0*/) : 
FormBase(tray, oldListener),
mNameSaga(Ogre::StringUtil::BLANK),
mNewSagaAdded(false)
{
	showOptions();
}

FormNewSaga::~FormNewSaga()
{
	hideAllOptions();
}



bool FormNewSaga::keyPressed(const OIS::KeyEvent &arg)
{
	LineEdit* leName = dynamic_cast<LineEdit*>(mTrayMgr->getWidget("FormNewSaga/LineEdit/Name"));
	if (leName) leName->injectKeyPress(arg);

	return FormBase::keyPressed(arg);
}

bool FormNewSaga::keyReleased(const OIS::KeyEvent &arg)
{
	return FormBase::keyReleased(arg);
}



bool FormNewSaga::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;
	return FormBase::mouseMoved(arg);
}

bool FormNewSaga::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;
	return FormBase::mousePressed(arg, id);
}

bool FormNewSaga::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;
	return FormBase::mouseReleased(arg, id);
}



void FormNewSaga::buttonHit(Button* button)
{
	if (button->getName() == "FormNewSaga/Button/NextName")
	{
		LineEdit* leName = dynamic_cast<LineEdit*>(mTrayMgr->getWidget("FormNewSaga/LineEdit/Name"));
		mNameSaga = (leName) ? leName->getText() : Ogre::StringUtil::BLANK;
		LineEdit::removeSpecialCodes(mNameSaga);

		if (!mNameSaga.empty() && _isValidSaga(mNameSaga))
		{
			hideOptions();
			showSelectGames();
			mGamesSaga.clear();
		}
		else
		{
			mTrayMgr->showOkDialog("NEW SAGA ERROR", "A saga with this name already exists in YGC. "
				"Choose a different name for this saga.");
		}
	}
	else if (button->getName() == "FormNewSaga/Button/BackPathGames")
	{
		mGamesSaga.clear();
		hideSelectGames();
		showOptions();
	}
	else if (button->getName() == "FormNewSaga/Button/AddPathGames")
	{
		FileExplorer* fe = dynamic_cast<FileExplorer*>(mTrayMgr->getWidget("FormNewSaga/Explorer/PathGames"));
		if (fe->getSelectedFile() != Ogre::StringUtil::BLANK)
		{
			_addNewGameToSaga(fe->getSelectedFile());
			fe->deselectAll();
		}
	}
	else if (button->getName() == "FormNewSaga/Button/AcceptPathGames")
	{
		_createNewSaga(mNameSaga);
		mNewSagaAdded = true;
	}
}

void FormNewSaga::okDialogClosed(const Ogre::DisplayString& message)
{

}



bool FormNewSaga::_isValidSaga(const Ogre::String& nameSaga)
{
	Ogre::String pathSagas = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Sagas", "./Sagas");
	if (boost::filesystem::is_directory(pathSagas + "/" + nameSaga))
		return false; // sagas must have differents names, so no valid saga

	return true;
}


bool FormNewSaga::_addNewGameToSaga(const Ogre::String& nameGame)
{
	if (std::find(mGamesSaga.begin(), mGamesSaga.end(), nameGame) == mGamesSaga.end())
	{
		mGamesSaga.push_back(nameGame);
		return true;
	}
	return false;
}


bool FormNewSaga::_createNewSaga(const Ogre::String& nameSaga)
{
	Ogre::String pathNewSaga = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Sagas", "./Sagas");
	pathNewSaga += "/" + nameSaga;

	if (!boost::filesystem::create_directory(pathNewSaga)) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Images")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Images/Header")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Images/Logo")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Images/Thumbs")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Images/Wallpapers")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Models")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Models/Characters")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Sounds")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Sounds/Voices")) return false;

	CSimpleIniA defaultIni;
	for (unsigned int i = 0; i < mGamesSaga.size(); ++i)
	{
		Ogre::String nameKey = "Game_" + Ogre::StringConverter::toString(i + 1);
		defaultIni.SetValue("SAGA.GAMES", nameKey.c_str(), mGamesSaga[i].c_str());
	}

	if (defaultIni.SaveFile(Ogre::String(pathNewSaga + "/" + nameSaga + ".ini").c_str()) < 0) return false;

	return true; // ok!
}



void FormNewSaga::hideAllOptions()
{
	hideOptions();
	hideSelectGames();
}

void FormNewSaga::hideOptions()
{
	if (mTrayMgr->getWidget("FormNewSaga/Window/Options"))
	{
		mTrayMgr->destroyDialogWindow("FormNewSaga/Window/Options");
		mTrayMgr->destroyWidget("FormNewSaga/LineEdit/Name");
		mTrayMgr->destroyWidget("FormNewSaga/Button/NextName");
	}
}

void FormNewSaga::showOptions()
{
	// all submenu options are hiddens?
	if (!mTrayMgr->isWindowDialogVisible())
	{
		unsigned int numOptions = 2;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 425;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		mTrayMgr->createDialogWindow("FormNewSaga/Window/Options", "NEW SAGA", left, top, width, height);	 top += sepOptions / 2;
		mTrayMgr->createLineEdit("FormNewSaga/LineEdit/Name", "Name", "", left, top, width); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormNewSaga/Button/NextName", "NEXT", left, top, 60);
	}
}

void FormNewSaga::hideSelectGames()
{
	if (mTrayMgr->getWidget("FormNewSaga/Window/PathGames"))
	{
		mTrayMgr->destroyDialogWindow("FormNewSaga/Window/PathGames");
		mTrayMgr->destroyWidget("FormNewSaga/Explorer/PathGames");
		mTrayMgr->destroyWidget("FormNewSaga/Button/BackPathGames");
		mTrayMgr->destroyWidget("FormNewSaga/Button/AddPathGames");
		mTrayMgr->destroyWidget("FormNewSaga/Button/AcceptPathGames");
	}
}

void FormNewSaga::showSelectGames()
{
	if (!mTrayMgr->getWidget("FormNewSaga/Window/PathGames")) // menu is hidden
	{
		unsigned int numOptions = 11;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 480;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		Ogre::String pathGames = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Games", "./Games");

		mTrayMgr->createDialogWindow("FormNewSaga/Window/PathGames", "GAMES SAGA", left, top, width, height);	 top += sepOptions / 2;
		FileExplorer* fe = mTrayMgr->createFileExplorer("FormNewSaga/Explorer/PathGames", pathGames, left, top, width, height); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormNewSaga/Button/BackPathGames", "BACK", left, top, 60);
		mTrayMgr->createButton("FormNewSaga/Button/AddPathGames", "ADD", left + 65, top, 60);
		mTrayMgr->createButton("FormNewSaga/Button/AcceptPathGames", "ACCEPT", width - 30, top, 80);
		fe->enableSelectDir(true);
	}
}

