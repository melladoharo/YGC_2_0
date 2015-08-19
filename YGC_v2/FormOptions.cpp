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
	else if (button->getName() == "FormOptions/Button/CloseThumbnails")
	{
		ItemSelector* selResGames = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/ResGames"));
		if (selResGames) ConfigReader::getSingletonPtr()->getReader()->SetValue("FORM.GAMES", "Thumbs_Resolution", selResGames->getSelectedOption().c_str());
		ItemSelector* selResImages = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/ResImages"));
		if (selResImages) ConfigReader::getSingletonPtr()->getReader()->SetValue("FORM.IMAGES", "Thumbs_Resolution", selResImages->getSelectedOption().c_str());
		ItemSelector* selResVideos = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/ResVideos"));
		if (selResVideos) ConfigReader::getSingletonPtr()->getReader()->SetValue("FORM.VIDEOS", "Thumbs_Resolution", selResVideos->getSelectedOption().c_str());
		ItemSelector* selResCover = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/ResCover"));
		if (selResCover) ConfigReader::getSingletonPtr()->getReader()->SetValue("FORM.OVERVIEW", "Cover_Resolution", selResCover->getSelectedOption().c_str());
		ItemSelector* selResDisc = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/ResDisc"));
		if (selResDisc) ConfigReader::getSingletonPtr()->getReader()->SetValue("FORM.OVERVIEW", "Disc_Resolution", selResDisc->getSelectedOption().c_str());
		ItemSelector* selResLogo = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/ResLogo"));
		if (selResLogo) ConfigReader::getSingletonPtr()->getReader()->SetValue("FORM.OVERVIEW", "Logo_Resolution", selResLogo->getSelectedOption().c_str());
		ItemSelector* selBoxArt = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/ResBoxArt"));
		if (selBoxArt) ConfigReader::getSingletonPtr()->getReader()->SetValue("FORM.OVERVIEW", "BoxArt_Resolution", selBoxArt->getSelectedOption().c_str());
		ConfigReader::getSingletonPtr()->saveConfig();
		hideOptionsThumbnails();
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
		ConfigReader::getSingletonPtr()->saveConfig();
		hideOptionsGraphics();
		showOptions();
	}
	else if (button->getName() == "FormOptions/Button/CloseAdvancedGraphics")
	{
		ItemSelector* selFsaa = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/Fsaa"));
		if (selFsaa) ConfigReader::getSingletonPtr()->getReader()->SetValue("SYSTEM.GRAPHICS", "FSAA", selFsaa->getSelectedOption().c_str());
		ItemSelector* selShadows = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/EnableShadows"));
		if (selShadows) ConfigReader::getSingletonPtr()->getReader()->SetValue("SYSTEM.GRAPHICS", "Shadows", selShadows->getSelectedOption().c_str());
		ItemSelector* selResShadows = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/ResShadows"));
		if (selResShadows) ConfigReader::getSingletonPtr()->getReader()->SetValue("SYSTEM.GRAPHICS", "Shadows_resolution", selResShadows->getSelectedOption().c_str());
		ItemSelector* selBlurShadows = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/BlurShadows"));
		if (selBlurShadows) ConfigReader::getSingletonPtr()->getReader()->SetValue("SYSTEM.GRAPHICS", "Blur_shadows", selBlurShadows->getSelectedOption().c_str());
		ItemSelector* selFilterMode = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/FilterMode"));
		if (selFilterMode) ConfigReader::getSingletonPtr()->getReader()->SetValue("SYSTEM.GRAPHICS", "Filtering_mode", selFilterMode->getSelectedOption().c_str());
		ItemSelector* selAnisoFilter = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormOptions/Selector/Anisotropic"));
		if (selAnisoFilter) ConfigReader::getSingletonPtr()->getReader()->SetValue("SYSTEM.GRAPHICS", "Anisotropic_filter", selAnisoFilter->getSelectedOption().c_str());
		ConfigReader::getSingletonPtr()->saveConfig();
		hideOptionsAdvancedGraphics();
		showOptionsGraphics();
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
	else if (label->getName() == "FormOptions/Label/Thumbs")
	{
		hideOptions();
		showOptionsThumbnails();
	}
	else if (label->getName() == "FormOptions/Label/ResetThumbnails")
	{
		_resetThumbnails();
		mTrayMgr->showOkDialog("RESET THUMBNAILS", "All thumbnails created with YGC2 have been erased. "
			"You must restart the application to see the new thumbnails.");
	}
	else if (label->getName() == "FormOptions/Label/Graphics")
	{
		hideOptions();
		showOptionsGraphics();
	}
	else if (label->getName() == "FormOptions/Label/AdvancedGraphics")
	{
		hideOptionsGraphics();
		showOptionsAdvancedGraphics();
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
	hideOptionsAdvancedGraphics();
	hideOptionsThumbnails();
}

void FormOptions::hideOptions()
{
	if (mTrayMgr->getWidget("FormOptions/Window/Options"))
	{
		mTrayMgr->destroyDialogWindow("FormOptions/Window/Options");
		mTrayMgr->destroyWidget("FormOptions/Label/General");
		mTrayMgr->destroyWidget("FormOptions/Label/Thumbs");
		mTrayMgr->destroyWidget("FormOptions/Label/Graphics");
		mTrayMgr->destroyWidget("FormOptions/Label/Sound");
		mTrayMgr->destroyWidget("FormOptions/Label/Video");
	}
}

void FormOptions::showOptions()
{
	if (!mTrayMgr->isWindowDialogVisible()) // menu is hidden?
	{
		unsigned int numOptions = 6;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 280;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;

		mTrayMgr->createDialogWindow("FormOptions/Window/Options", "  OPTIONS  ", left, top, width, height);	 top += sepOptions / 2;
		mTrayMgr->createLabel("FormOptions/Label/General", "GENERAL", left, top, width, 23); top += sepOptions;
		mTrayMgr->createLabel("FormOptions/Label/Thumbs", "THUMBNAILS", left, top, width, 23); top += sepOptions;
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

void FormOptions::hideOptionsThumbnails()
{
	if (mTrayMgr->getWidget("FormOptions/Window/Thumbnails"))
	{
		mTrayMgr->destroyDialogWindow("FormOptions/Window/Thumbnails");
		mTrayMgr->destroyWidget("FormOptions/Selector/ResGames");
		mTrayMgr->destroyWidget("FormOptions/Selector/ResImages");
		mTrayMgr->destroyWidget("FormOptions/Selector/ResVideos");
		mTrayMgr->destroyWidget("FormOptions/Selector/ResCover");
		mTrayMgr->destroyWidget("FormOptions/Selector/ResDisc");
		mTrayMgr->destroyWidget("FormOptions/Selector/ResLogo");
		mTrayMgr->destroyWidget("FormOptions/Selector/ResBoxArt");
		mTrayMgr->destroyWidget("FormOptions/Label/ResetThumbnails");
		mTrayMgr->destroyWidget("FormOptions/Button/CloseThumbnails");
	}
}

void FormOptions::showOptionsThumbnails()
{
	if (!mTrayMgr->getWidget("FormOptions/Window/Thumbnails")) // menu is hidden
	{
		unsigned int numOptions = 9;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 420;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;

		Ogre::StringVector itemsRes; 
		itemsRes.push_back("128"); itemsRes.push_back("256"); itemsRes.push_back("512"); 
		itemsRes.push_back("1024"); itemsRes.push_back("2048"); itemsRes.push_back("Source Size");

		mTrayMgr->createDialogWindow("FormOptions/Window/Thumbnails", "THUMBNAILS OPTIONS", left, top, width, height);	 top += sepOptions / 2;
		ItemSelector* selResGames = mTrayMgr->createItemSelector("FormOptions/Selector/ResGames", "Form Games Resolution", itemsRes, left, top, width); top += sepOptions;
		ItemSelector* selResImages = mTrayMgr->createItemSelector("FormOptions/Selector/ResImages", "Form Images Resolution", itemsRes, left, top, width); top += sepOptions;
		ItemSelector* selResVideos = mTrayMgr->createItemSelector("FormOptions/Selector/ResVideos", "Form Videos Resolution", itemsRes, left, top, width); top += sepOptions;
		ItemSelector* selResCover = mTrayMgr->createItemSelector("FormOptions/Selector/ResCover", "Cover Resolution", itemsRes, left, top, width); top += sepOptions;
		ItemSelector* selResDisc = mTrayMgr->createItemSelector("FormOptions/Selector/ResDisc", "Discs Resolution", itemsRes, left, top, width); top += sepOptions;
		ItemSelector* selResLogo = mTrayMgr->createItemSelector("FormOptions/Selector/ResLogo", "Logo Resolution", itemsRes, left, top, width); top += sepOptions;
		ItemSelector* selBoxArt = mTrayMgr->createItemSelector("FormOptions/Selector/ResBoxArt", "BoxArt Resolution", itemsRes, left, top, width); top += sepOptions;
		mTrayMgr->createLabel("FormOptions/Label/ResetThumbnails", "RESET THUMBNAILS", left, top, width, 23); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormOptions/Button/CloseThumbnails", "BACK", left, top, 60);	

		Ogre::String gameValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM.GAMES", "Thumbs_Resolution", "0");
		selResGames->selectOption(gameValue, false);
		Ogre::String imagesValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM.IMAGES", "Thumbs_Resolution", "0");
		selResImages->selectOption(imagesValue, false);
		Ogre::String videoValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM.VIDEOS", "Thumbs_Resolution", "0");
		selResVideos->selectOption(videoValue, false);
		Ogre::String coverValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM.OVERVIEW", "Cover_Resolution", "0");
		selResCover->selectOption(coverValue, false);
		Ogre::String discValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM.OVERVIEW", "Disc_Resolution", "0");
		selResDisc->selectOption(discValue, false);
		Ogre::String logoValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM.OVERVIEW", "Logo_Resolution", "0");
		selResLogo->selectOption(logoValue, false);
		Ogre::String boxArtValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM.OVERVIEW", "BoxArt_Resolution", "0");
		selBoxArt->selectOption(boxArtValue, false);
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
		ItemSelector* selRatio = mTrayMgr->createItemSelector("FormOptions/Selector/AspectRatio", "Monitor Aspect Ratio", itemsAspect, left, top, width); top += sepOptions;
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

void FormOptions::hideOptionsAdvancedGraphics()
{
	if (mTrayMgr->getWidget("FormOptions/Window/AdvancedGraphics"))
	{
		mTrayMgr->destroyDialogWindow("FormOptions/Window/AdvancedGraphics");
		mTrayMgr->destroyWidget("FormOptions/Selector/Fsaa");
		mTrayMgr->destroyWidget("FormOptions/Selector/EnableShadows");
		mTrayMgr->destroyWidget("FormOptions/Selector/BlurShadows");
		mTrayMgr->destroyWidget("FormOptions/Selector/ResShadows");
		mTrayMgr->destroyWidget("FormOptions/Selector/FilterMode");
		mTrayMgr->destroyWidget("FormOptions/Selector/Anisotropic");
		mTrayMgr->destroyWidget("FormOptions/Button/CloseAdvancedGraphics");
		mTrayMgr->destroyWidget("FormOptions/Button/ApplyAdvancedGraphics");
	}
}

void FormOptions::showOptionsAdvancedGraphics()
{
	if (!mTrayMgr->getWidget("FormOptions/Window/AdvancedGraphics")) // menu is hidden
	{
		unsigned int numOptions = 7;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 420;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;

		Ogre::String fsaaValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM.GRAPHICS", "FSAA", "0");
		Ogre::String enableShadowsValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM.GRAPHICS", "Shadows", "Yes");
		Ogre::String resShadowValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM.GRAPHICS", "Shadows_resolution", "1024");
		Ogre::String blurShadowValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM.GRAPHICS", "Blur_shadows", "Yes");
		Ogre::String filterModeValue = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM.GRAPHICS", "Filtering_mode", "Anisotropic");
		Ogre::String anisotropicFilter = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM.GRAPHICS", "Anisotropic_filter", "1");
		Ogre::StringVector itemsYesNo; itemsYesNo.push_back("Yes"); itemsYesNo.push_back("No");
		Ogre::StringVector itemsResText; itemsResText.push_back("512"); itemsResText.push_back("1024"); itemsResText.push_back("2048");
		Ogre::StringVector itemsFiltering; itemsFiltering.push_back("Bilineal"); itemsFiltering.push_back("Trilineal"); itemsFiltering.push_back("Anisotropic");
		Ogre::StringVector itemsAniLevel; itemsAniLevel.push_back("1"); itemsAniLevel.push_back("2"); itemsAniLevel.push_back("4"); itemsAniLevel.push_back("8"); itemsAniLevel.push_back("16");
		Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystemByName("Direct3D9 Rendering Subsystem");
		Ogre::StringVector& itemsFsaa = rs->getConfigOptions()["FSAA"].possibleValues;
		
		mTrayMgr->createDialogWindow("FormOptions/Window/AdvancedGraphics", "ADVANCED GRAPHICS", left, top, width, height);	 top += sepOptions / 2;
		ItemSelector* selFsaa = mTrayMgr->createItemSelector("FormOptions/Selector/Fsaa", "FSAA", itemsFsaa, left, top, width); top += sepOptions;
		ItemSelector* selShadows = mTrayMgr->createItemSelector("FormOptions/Selector/EnableShadows", "Enable Shadows", itemsYesNo, left, top, width); top += sepOptions;
		ItemSelector* selResShadows = mTrayMgr->createItemSelector("FormOptions/Selector/ResShadows", "Shadows Resolution", itemsResText, left, top, width); top += sepOptions;
		ItemSelector* selBlurShadows = mTrayMgr->createItemSelector("FormOptions/Selector/BlurShadows", "Blur Shadows", itemsYesNo, left, top, width); top += sepOptions;
		ItemSelector* selTextFiltering = mTrayMgr->createItemSelector("FormOptions/Selector/FilterMode", "Filtering Mode", itemsFiltering, left, top, width); top += sepOptions;
		ItemSelector* selAnistropic = mTrayMgr->createItemSelector("FormOptions/Selector/Anisotropic", "Anisotropic Filter", itemsAniLevel, left, top, width); top += sepOptions;
		top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormOptions/Button/CloseAdvancedGraphics", "BACK", left, top, 60);
		mTrayMgr->createButton("FormOptions/Button/ApplyAdvancedGraphics", "APPLY", left + 65, top, 80);

		selFsaa->selectOption(fsaaValue, false);
		selShadows->selectOption(enableShadowsValue, false);
		selResShadows->selectOption(resShadowValue, false);
		selBlurShadows->selectOption(blurShadowValue, false);
		selTextFiltering->selectOption(filterModeValue, false);
		selAnistropic->selectOption(anisotropicFilter, false);
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

void FormOptions::_resetThumbnails()
{
	Ogre::String pathGames = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Games", "./Games");
	// ../Games
	for (boost::filesystem::directory_iterator it(pathGames), end; it != end; ++it)
	{
		// ..Game1/Images/Thumbs
		Ogre::String pathThumbs = it->path().generic_string() + "/Images/Thumbs";
		if (boost::filesystem::is_directory(pathThumbs))
		{
			boost::filesystem::remove_all(pathThumbs);
			boost::filesystem::create_directory(pathThumbs);
		}
	}
}

