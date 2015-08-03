#include "stdafx.h"
#include "FormOptions.h"
#include "ConfigReader.h"

FormOptions::FormOptions(GuiManager* tray, GuiListener* oldListener /*= 0*/) : 
FormBase(tray, oldListener)
{
	showOptions();
}

FormOptions::~FormOptions()
{
	ConfigReader::getSingletonPtr()->saveConfig();
	hideAllOptions();
}



bool FormOptions::keyPressed(const OIS::KeyEvent &arg)
{
	return FormBase::keyPressed(arg);
}

bool FormOptions::keyReleased(const OIS::KeyEvent &arg)
{
	return FormBase::keyReleased(arg);
}



bool FormOptions::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;

	if (arg.state.Z.rel != 0)
	{
		FileExplorer* explorer = dynamic_cast<FileExplorer*>(mTrayMgr->getWidget("FormOptions/Explorer/PathGames"));
		if (explorer)
		{
			if (arg.state.Z.rel > 0) explorer->scrollUp();
			else explorer->scrollDown();
		}
	}

	return FormBase::mouseMoved(arg);
}

bool FormOptions::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;
	return FormBase::mouseReleased(arg, id);
}

bool FormOptions::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;
	return FormBase::mouseReleased(arg, id);
}



void FormOptions::buttonHit(Button* button)
{
	if (button->getName() == "FormOptions/Button/CloseOptionsGeneral")
	{
		hideOptionsGeneral();
		showOptions();
	}
	else if (button->getName() == "FormOptions/Button/CloseOptionsGraphics")
	{
		ItemSelector* selFullscreen = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/Fullscreen"));
		if (selFullscreen) ConfigReader::getSingletonPtr()->getReader()->SetValue("SYSTEM.GRAPHICS", "Fullscreen", selFullscreen->getSelectedOption().c_str());
		ItemSelector* selVsync = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/Vsync"));
		if (selVsync) ConfigReader::getSingletonPtr()->getReader()->SetValue("SYSTEM.GRAPHICS", "VSync", selVsync->getSelectedOption().c_str());
		ItemSelector* selRatio = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/AspectRatio"));
		if (selRatio) ConfigReader::getSingletonPtr()->getReader()->SetValue("SYSTEM.GRAPHICS", "Aspect_Ratio", selRatio->getSelectedOption().c_str());
		ItemSelector* selResolution = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/Resolution"));
		if (selResolution) ConfigReader::getSingletonPtr()->getReader()->SetValue("SYSTEM.GRAPHICS", "Resolution", selResolution->getSelectedOption().c_str());
		hideOptionsGraphics();
		showOptions();
	}
	else if (button->getName() == "FormOptions/Button/ClosePathGames")
	{
		hideEditPathGames();
		showOptionsGeneral();
	}
	else if (button->getName() == "FormOptions/Button/SelectPathGames")
	{
		FileExplorer* explorer = dynamic_cast<FileExplorer*>(mTrayMgr->getWidget("FormOptions/Explorer/PathGames"));
		if (explorer)
		{
			Ogre::String newPath = explorer->getSelectedPath();
			if (boost::filesystem::is_directory(newPath))
			{
				ConfigReader::getSingletonPtr()->getReader()->SetValue("SYSTEM", "Path_Games", newPath.c_str());
			}
		}
		
		hideEditPathGames();
		showOptionsGeneral();
	}
}

void FormOptions::labelHit(Label* label)
{
	if (label->getName() == "FormOptions/Label/General")
	{
		hideOptions();
		showOptionsGeneral();
	}
	else if (label->getName() == "FormOptions/Label/Graphics")
	{
		hideOptions();
		showOptionsGraphics();
	}
	else if (label->getName() == "FormOptions/Label/EditGamesPath")
	{
		hideOptionsGeneral();
		showEditPathGames();
	}
}

void FormOptions::itemChanged(ItemSelector* selector)
{
	if (selector->getName() == "FormOptions/Selector/AspectRatio")
	{
		ItemSelector* selResolution = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/Resolution"));
		if (selResolution) selResolution->setItems(_getResolutionModes(selector->getSelectedOption()));
	}
}



void FormOptions::hide()
{
	hideAllOptions();
}

void FormOptions::show()
{
	showOptions();
}



void FormOptions::hideAllOptions()
{
	hideOptions();
	hideOptionsGeneral();
	hideOptionsGraphics();
}

void FormOptions::hideOptions()
{
	if (mTrayMgr->getWidget("FormOptions/Window/Options"))
	{
		mTrayMgr->destroyDialogWindow("FormOptions/Window/Options");
		mTrayMgr->destroyWidget("FormOptions/Label/General");
		mTrayMgr->destroyWidget("FormOptions/Label/Graphics");
		mTrayMgr->destroyWidget("FormOptions/Label/Sound");
		mTrayMgr->destroyWidget("FormOptions/Label/Video");
	}
}

void FormOptions::showOptions()
{
	if (!mTrayMgr->getWidget("FormOptions/Window/Options")) // menu is hidden
	{
		unsigned int numOptions = 5;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 280;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;

		mTrayMgr->createDialogWindow("FormOptions/Window/Options", "OPTIONS", left, top, width, height);	 top += sepOptions / 2;
		mTrayMgr->createLabel("FormOptions/Label/General", "GENERAL", left, top, width, 23); top += sepOptions;
		mTrayMgr->createLabel("FormOptions/Label/Graphics", "GRAPHICS", left, top, width, 23); top += sepOptions;
		mTrayMgr->createLabel("FormOptions/Label/Sound", "SOUND", left, top, width, 23); top += sepOptions;
		mTrayMgr->createLabel("FormOptions/Label/Video", "VIDEO", left, top, width, 23); top += sepOptions;
	}
}

void FormOptions::hideOptionsGeneral()
{
	if (mTrayMgr->getWidget("FormOptions/Window/General"))
	{
		mTrayMgr->destroyDialogWindow("FormOptions/Window/General");
		mTrayMgr->destroyWidget("FormOptions/Label/EditGamesPath");
		mTrayMgr->destroyWidget("FormOptions/Label/EditSagasPath");
		mTrayMgr->destroyWidget("FormOptions/Button/CloseOptionsGeneral");
	}
}

void FormOptions::showOptionsGeneral()
{
	if (!mTrayMgr->getWidget("FormOptions/Window/General")) // menu is hidden
	{
		unsigned int numOptions = 3;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 350;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;

		mTrayMgr->createDialogWindow("FormOptions/Window/General", "GENERAL OPTIONS", left, top, width, height);	 top += sepOptions / 2;
		mTrayMgr->createLabel("FormOptions/Label/EditGamesPath", "EDIT GAMES PATH", left, top, width, 23); top += sepOptions;
		mTrayMgr->createLabel("FormOptions/Label/EditSagasPath", "EDIT SAGAS PATH", left, top, width, 23); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormOptions/Button/CloseOptionsGeneral", "BACK", left, top, 60);
	}
}

void FormOptions::hideOptionsGraphics()
{
	if (mTrayMgr->getWidget("FormOptions/Window/Graphics"))
	{
		mTrayMgr->destroyDialogWindow("FormOptions/Window/Graphics");
		mTrayMgr->destroyWidget("FormOptions/Selector/Fullscreen");
		mTrayMgr->destroyWidget("FormOptions/Selector/Vsync");
		mTrayMgr->destroyWidget("FormOptions/Selector/AspectRatio");
		mTrayMgr->destroyWidget("FormOptions/Selector/Resolution");
		mTrayMgr->destroyWidget("FormOptions/Label/AdvancedGraphics");
		mTrayMgr->destroyWidget("FormOptions/Button/CloseOptionsGraphics");
		mTrayMgr->destroyWidget("FormOptions/Button/ApplyGraphics");
	}
}

void FormOptions::showOptionsGraphics()
{
	if (!mTrayMgr->getWidget("FormOptions/Window/Graphics")) // menu is hidden
	{
		unsigned int numOptions = 6;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 520;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;

		Ogre::String screenValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM.GRAPHICS", "Fullscreen", "Yes");
		Ogre::String vsyncValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM.GRAPHICS", "VSync", "Yes");
		Ogre::String ratioValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM.GRAPHICS", "Aspect_Ratio", "Auto");
		Ogre::String resolutionValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM.GRAPHICS", "Resolution", "");
		Ogre::StringVector itemsAspect; itemsAspect.push_back("Auto"); itemsAspect.push_back("16:10"); itemsAspect.push_back("16:9");
		Ogre::StringVector itemsYesNo; itemsYesNo.push_back("Yes"); itemsYesNo.push_back("No");
		Ogre::StringVector itemsResolution = _getResolutionModes(ratioValue);

		mTrayMgr->createDialogWindow("FormOptions/Window/Graphics", "GRAPHICS OPTIONS", left, top, width, height);	 top += sepOptions / 2;
		ItemSelector* selFullScreen = mTrayMgr->createItemSelector("FormOptions/Selector/Fullscreen", "Fullscreen", itemsYesNo, left, top, width); top += sepOptions;
		ItemSelector* selVsync = mTrayMgr->createItemSelector("FormOptions/Selector/Vsync", "Sync Every Frame", itemsYesNo, left, top, width); top += sepOptions;
		ItemSelector* selRatio = mTrayMgr->createItemSelector("FormOptions/Selector/AspectRatio", "Monitor Aspect ratio", itemsAspect, left, top, width); top += sepOptions;
		ItemSelector* selResolution = mTrayMgr->createItemSelector("FormOptions/Selector/Resolution", "Render Resolution", itemsResolution, left, top, width); top += sepOptions;
		mTrayMgr->createLabel("FormOptions/Label/AdvancedGraphics", "ADVANCED GRAPHICS", left, top, width, 23); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormOptions/Button/CloseOptionsGraphics", "BACK", left, top, 60);
		mTrayMgr->createButton("FormOptions/Button/ApplyGraphics", "APPLY", left + 65, top, 80);

		selFullScreen->selectOption(screenValue, false);
		selVsync->selectOption(vsyncValue, false);
		selRatio->selectOption(ratioValue, false);
		selResolution->selectOption(resolutionValue, false);
	}
}

void FormOptions::hideEditPathGames()
{
	if (mTrayMgr->getWidget("FormOptions/Window/PathGames"))
	{
		mTrayMgr->destroyDialogWindow("FormOptions/Window/PathGames");
		mTrayMgr->destroyWidget("FormOptions/Explorer/PathGames");
		mTrayMgr->destroyWidget("FormOptions/Button/ClosePathGames");
		mTrayMgr->destroyWidget("FormOptions/Button/SelectPathGames");
	}
}

void FormOptions::showEditPathGames()
{
	if (!mTrayMgr->getWidget("FormOptions/Window/PathGames")) // menu is hidden
	{
		unsigned int numOptions = 11;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 560;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		Ogre::String pathGames = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Games", "./Games");

		mTrayMgr->createDialogWindow("FormOptions/Window/PathGames", "GENERAL OPTIONS", left, top, width, height);	 top += sepOptions / 2;
		mTrayMgr->createFileExplorer("FormOptions/Explorer/PathGames", pathGames, left, top, width, height); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormOptions/Button/ClosePathGames", "BACK", left, top, 60);
		mTrayMgr->createButton("FormOptions/Button/SelectPathGames", "ACCEPT", left + 70, top, 80);
	}
}



Ogre::StringVector FormOptions::_getResolutionModes(const Ogre::String& aspectRatio)
{
	Ogre::StringVector allRes = Ogre::Root::getSingleton().getRenderSystem()->getConfigOptions()["Video Mode"].possibleValues;
	Ogre::Real aRatio = (aspectRatio == "16:9") ? 16.0f / 9.0f : 16.0f / 10.0f;
	unsigned int i = 0;

	while (i < allRes.size())
	{
		// remove the 16-bit color modes
		if (allRes[i].find("16-bit") != Ogre::String::npos)
		{
			allRes.erase(allRes.begin() + i);
		}
		else
		{
			if (aspectRatio != "Auto")
			{
				std::size_t found = allRes[i].find_first_of('x');
				if (found != std::string::npos)
				{
					Ogre::Real resX = Ogre::StringConverter::parseInt(allRes[i].substr(0, found - 1));
					Ogre::Real resY = Ogre::StringConverter::parseInt(allRes[i].substr(found + 1));
					if (resX / resY != aRatio) allRes.erase(allRes.begin() + i);
					else i++;
				}
			}
			else i++;
		}
	}
	
	return allRes;
}
