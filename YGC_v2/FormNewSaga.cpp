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
		}
		else
		{
			mTrayMgr->showOkDialog("NO VALID SAGA", "A saga with its name already exists.");
		}
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



bool FormNewSaga::_createNewSaga(const Ogre::String& nameSaga)
{
	Ogre::String pathNewSaga = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Games", "./Games");
	pathNewSaga += "/" + nameSaga;

	if (!boost::filesystem::create_directory(pathNewSaga)) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Fonts")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Images")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Images/BoxArt")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Images/Concept Art")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Images/Cover")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Images/Discs")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Images/Header")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Images/Logo")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Images/Screenshots")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Images/Thumbs")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Images/Wallpapers")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Models")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Models/Characters")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Models/Collectors Edition")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Models/Objects")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Reviews")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Sounds")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Sounds/Soundtrack")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Sounds/Voices")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Videos")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Videos/Gameplays")) return false;
	if (!boost::filesystem::create_directory(pathNewSaga + "/Videos/Trailers")) return false;

	CSimpleIniA defaultIni;
	if (defaultIni.SetValue("GAME.INFO", "Name", nameSaga.c_str()) < 0) return false;
	if (defaultIni.SetValue("GAME.INFO", "Platform", "") < 0) return false;
	if (defaultIni.SetValue("GAME.INFO", "Developer", "") < 0) return false;
	if (defaultIni.SetValue("GAME.INFO", "Publisher", "") < 0) return false;
	if (defaultIni.SetValue("GAME.INFO", "Genre", "") < 0) return false;
	if (defaultIni.SetValue("GAME.INFO", "ReleaseDate", "") < 0) return false;
	if (defaultIni.SetValue("GAME.INFO", "Rating", "") < 0) return false;
	if (defaultIni.SetValue("GAME.INFO", "Score", "") < 0) return false;
	if (defaultIni.SetValue("GAME.INFO", "Overview", "") < 0) return false;
	if (defaultIni.SetValue("GAME.INFO", "Summary", "") < 0) return false;
	if (defaultIni.SetValue("GAME.PATHS", "GamePath", "") < 0) return false;
	if (defaultIni.SetValue("GAME.STATS", "Playing", "") < 0) return false;
	if (defaultIni.SetValue("GAME.STATS", "PlayCount", "") < 0) return false;
	if (defaultIni.SetValue("GAME.STATS", "LastGame", "") < 0) return false;
	if (defaultIni.SetValue("GAME.STATS", "TimePlayed", "") < 0) return false;
	if (defaultIni.SaveFile(Ogre::String(pathNewSaga + "/GameInfo.ini").c_str()) < 0) return false;

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
		Ogre::Real width = 560;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		Ogre::String pathGames = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Games", "./Games");

		mTrayMgr->createDialogWindow("FormNewSaga/Window/PathGames", "GAMES SAGA", left, top, width, height);	 top += sepOptions / 2;
		mTrayMgr->createFileExplorer("FormNewSaga/Explorer/PathGames", pathGames, left, top, width, height); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormNewSaga/Button/BackPathGames", "BACK", left, top, 60);
		mTrayMgr->createButton("FormNewSaga/Button/AcceptPathGames", "ACCEPT", left + 70, top, 80);
	}
}

