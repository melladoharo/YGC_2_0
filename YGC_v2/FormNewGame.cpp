#include "stdafx.h"
#include "FormNewGame.h"
#include "ConfigReader.h"

FormNewGame::FormNewGame(GuiManager* tray, GuiListener* oldListener /*= 0*/) :
FormBase(tray, oldListener),
mNameGame(Ogre::StringUtil::BLANK),
mNewGameAdded(false)
{
	showOptions();
}

FormNewGame::~FormNewGame()
{
	hideAllOptions();
}



bool FormNewGame::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	return FormBase::frameRenderingQueued(evt);
}



bool FormNewGame::keyPressed(const OIS::KeyEvent &arg)
{
	LineEdit* leName = dynamic_cast<LineEdit*>(mTrayMgr->getWidget("FormNewGame/LineEdit/Name"));
	if (leName) leName->injectKeyPress(arg);

	return FormBase::keyPressed(arg);
}

bool FormNewGame::keyReleased(const OIS::KeyEvent &arg)
{
	return FormBase::keyReleased(arg);
}



bool FormNewGame::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;
	return FormBase::mouseMoved(arg);
}

bool FormNewGame::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;
	return FormBase::mousePressed(arg, id);
}

bool FormNewGame::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;
	return FormBase::mouseReleased(arg, id);
}



void FormNewGame::buttonHit(Button* button)
{
	if (button->getName() == "FormNewGame/Button/NextName")
	{
		LineEdit* leName = dynamic_cast<LineEdit*>(mTrayMgr->getWidget("FormNewGame/LineEdit/Name"));
		mNameGame = (leName) ? leName->getText() : Ogre::StringUtil::BLANK;
		LineEdit::removeSpecialCodes(mNameGame);

		if (_isValidGame(mNameGame))
		{
			if (_createNewGame(mNameGame))
			{
				hideOptions();
				mTrayMgr->showOkDialog("NEW GAME", "The new game has been added to the library correctly.");
			}
		}
	}
}

void FormNewGame::okDialogClosed(const Ogre::DisplayString& message)
{
	if (message == "The new game has been added to the library correctly.")
	{
		mNewGameAdded = true;
	}
}



void FormNewGame::hideAllOptions()
{
	hideOptions();
}

void FormNewGame::hideOptions()
{
	if (mTrayMgr->getWidget("FormNewGame/Window/Options"))
	{
		mTrayMgr->destroyDialogWindow("FormNewGame/Window/Options");
		mTrayMgr->destroyWidget("FormNewGame/LineEdit/Name");
		mTrayMgr->destroyWidget("FormNewGame/Button/NextName");
	}
}

void FormNewGame::showOptions()
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
		mTrayMgr->createDialogWindow("FormNewGame/Window/Options", "NEW GAME", left, top, width, height);	 top += sepOptions / 2;
		mTrayMgr->createLineEdit("FormNewGame/LineEdit/Name", "Name", "", left, top, width); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormNewGame/Button/NextName", "NEXT", left, top, 60);
	}
}



bool FormNewGame::_isValidGame(const Ogre::String& nameGame)
{
	Ogre::String pathGames = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Games", "./Games");
	pathGames += "/" + nameGame;
	if (boost::filesystem::is_directory(pathGames))
		return false; // games must have differents names, so no valid game

	return true;
}

bool FormNewGame::_createNewGame(const Ogre::String& nameGame)
{
	Ogre::String pathGames = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Games", "./Games");
	pathGames += "/" + nameGame;
	
	if (!boost::filesystem::create_directory(pathGames)) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Fonts")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Images")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Images/BoxArt")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Images/Concept Art")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Images/Cover")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Images/Discs")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Images/Header")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Images/Logo")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Images/Screenshots")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Images/Thumbs")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Images/Wallpapers")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Models")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Models/Characters")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Models/Collectors Edition")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Models/Objects")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Reviews")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Sounds")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Sounds/Soundtrack")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Sounds/Voices")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Videos")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Videos/Gameplays")) return false;
	if (!boost::filesystem::create_directory(pathGames + "/Videos/Trailers")) return false;

	CSimpleIniA defaultIni;
	if (defaultIni.SetValue("GAME.INFO", "Name", nameGame.c_str()) < 0) return false;
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
	if (defaultIni.SaveFile(Ogre::String(pathGames + "/GameInfo.ini").c_str()) < 0) return false;

	return true; // ok!
}

