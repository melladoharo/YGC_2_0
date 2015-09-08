#include "stdafx.h"
#include "FormGames.h"
#include "ConfigReader.h"

FormGames::FormGames(const Ogre::String& pathGames, GuiManager* tray, GuiListener* oldListener) :
FormBase(tray, oldListener),
mFormSelector(0),
mFormNewGame(0),
mPathGames(pathGames),
mParentThumbs(mSceneMgr->getRootSceneNode()->createChildSceneNode()),
mLastThumbOver(0),
mQuestionDelete(Ogre::StringUtil::BLANK)
{
	resetCamera();
	// load grid-thumbs values from .ini config file
	mGridThumbs.rows = Ogre::StringConverter::parseInt(ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM.GAMES", "Thumbs_Rows", "1"));
	mGridThumbs.top = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM.GAMES", "Thumbs_Top", 0);
	mGridThumbs.left = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM.GAMES", "Thumbs_Left", 0);
	mGridThumbs.horizontalSep = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM.GAMES", "Thumbs_Horizontal_Sep", 0);
	mGridThumbs.verticalSep = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM.GAMES", "Thumbs_Vertial_Sep", 0);
	mGridThumbs.size = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM.GAMES", "Thumbs_Size", 0);
	Ogre::String resThumb = ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM.GAMES", "Thumbs_Resolution", 0);

	// initialize resource group 'FormGames'
	Ogre::ResourceGroupManager::getSingleton().createResourceGroup("Group/FormGames");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Group/FormGames");

	// iterate over the main path game and add them 
	for (boost::filesystem::directory_iterator it(mPathGames), end; it != end; ++it)
	{
		if (boost::filesystem::is_directory(it->path()))
		{
			mGameInfo.push_back(new GameInfo(it->path().generic_string()));
			// create (if not) and load the thumbnail header game
			mInfoHeader.push_back(sInfoResource());
			mGameInfo.back()->findHeaderResource(mInfoHeader.back());
			if (mInfoHeader.back().path != Ogre::StringUtil::BLANK) // game has a header
			{
				if (resThumb == "Source Size") // load the original image
				{
					GameInfo::loadImageFromDisk(mInfoHeader.back().path, mInfoHeader.back().nameThumb, "Group/FormGames");
				}
				else // load the thumbnail
				{
					unsigned int resThumbValue = Ogre::StringConverter::parseInt(resThumb);
					if (!boost::filesystem::is_regular_file(mInfoHeader.back().pathThumb))
						GameInfo::createThumbnail(mInfoHeader.back().path, mInfoHeader.back().pathThumb, resThumbValue);
					GameInfo::loadImageFromDisk(mInfoHeader.back().pathThumb, mInfoHeader.back().nameThumb, "Group/FormGames");
				}
			}
			// create the 3d thumbnail widget
			mThumbs.push_back(new Thumbnail3D(mParentThumbs, "FormGames/Thumb/Widget/" + Ogre::StringConverter::toString(mThumbs.size()),
				"Group/FormGames", mGameInfo.back()->getName(), mInfoHeader.back().nameThumb));
			mThumbs.back()->setIndex(mThumbs.size() - 1);
			mThumbs.back()->setScale(Ogre::Vector3(mGridThumbs.size, mGridThumbs.size, mGridThumbs.size));
		}
	}

	// set thumbs in a grid
	Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);

	// bottom slider
	Ogre::Real minValue = 0;
	Ogre::Real maxValue = (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0);
	Slider* sd = mTrayMgr->createSlider("FormGames/Slider", 500, minValue, maxValue, 1000);
	sd->getOverlayElement()->setVerticalAlignment(Ogre::GVA_BOTTOM);
	sd->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
	sd->setLeft(-(sd->getWidth() / 2)); // center slider 
	sd->setTop(-50);
	addWidgetToForm(sd);

	// top select menu
	Ogre::StringVector items; 
		items.push_back("All");  items.push_back("Installed");
		items.push_back("Genre"); items.push_back("Year");
	SelectMenu* sm = mTrayMgr->createSelectMenu("FormGames/SelectMenu", items, 275.0f);
	sm->getOverlayElement()->setVerticalAlignment(Ogre::GVA_TOP);
	sm->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
	sm->setTop(120);
	sm->setLeft(-sm->getWidth() / 2);
	addWidgetToForm(sm);
}

FormGames::~FormGames()
{
	if (mFormSelector) delete mFormSelector;
	if (mFormNewGame) delete mFormNewGame;
	for (unsigned int i = 0; i < mGameInfo.size(); ++i)
		delete mGameInfo[i];
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		delete mThumbs[i];
	mSceneMgr->destroySceneNode(mParentThumbs);
	Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("Group/FormGames");
	hideAllOptions();
}



bool FormGames::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (mFormSelector && mFormSelector->isFinished())
	{
		mTrayMgr->enableFadeEffect();
		delete mFormSelector;
		mFormSelector = 0;
		mTrayMgr->assignListenerToMenuBar(mOldListener);
		mTrayMgr->setListener(this);
		mTrayMgr->setMouseEventCallback(this);
		mTrayMgr->setKeyboardEventCallback(this);
		mTrayMgr->loadMenuBarMain();
		show();
	}

	if (mFormNewGame && mFormNewGame->isNewGameAdded())
		removeNewGameForm();

	return FormBase::frameRenderingQueued(evt);
}

bool FormGames::keyPressed(const OIS::KeyEvent &arg)
{
	return FormBase::keyPressed(arg);
}

bool FormGames::keyReleased(const OIS::KeyEvent &arg)
{
	return FormBase::keyReleased(arg);
}

bool FormGames::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;

	if (!mTrayMgr->isWindowDialogVisible()) // is a option windows visible? if not, continue
	{
		// mouse ray [intersection with objects]
		Ogre::Ray mouseRay = mTrayMgr->getCursorRay(mCamera);
		mRaySceneQuery->setRay(mouseRay);

		// execute query
		Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
		Ogre::RaySceneQueryResult::reverse_iterator iter = result.rbegin();

		// thumbnail3d intersection 
		Thumbnail3D* currentOver = 0;
		for (iter; iter != result.rend(); ++iter)
		{
			if (iter->movable->isVisible() && iter->movable->getName().find("Mesh/Thumbnail3D/") != Ogre::String::npos)
			{
				currentOver = Thumbnail3D::getThumbnail3dOver(mThumbs, iter->movable->getName());
			}
		}
		if (currentOver) mLastThumbOver = currentOver;
		else if (mLastThumbOver) { mLastThumbOver->setMouseUp(); mLastThumbOver = 0; }
	}

	return FormBase::mouseMoved(arg);
}

bool FormGames::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	return FormBase::mousePressed(arg, id);;
}

bool FormGames::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;

	if (id == OIS::MouseButtonID::MB_Left)
	{
		// clin on a thumbnail, then open the game selected
		if (mLastThumbOver)
		{
			unsigned int index = mLastThumbOver->getIndex();
			mLastThumbOver->setMouseUp();
			mLastThumbOver = 0;

			hide();
			mFormSelector = new FormSelectorGame(mGameInfo[index], mTrayMgr, this);
		}
	}
	else if (id == OIS::MouseButtonID::MB_Right)
	{
		// clin on a thumbnail, then open the game selected
		if (mLastThumbOver)
		{
			unsigned int index = mLastThumbOver->getIndex();
			mLastThumbOver->setMouseUp();
			
			mQuestionDelete = "When you delete a game, all the resources like screenshots, 3d models, videos, etc "
				"are removed too. Are you sure want to delete \"" + mGameInfo[index]->getName() + "\" "
				"game from library?";
			mTrayMgr->showYesNoDialog("DELETE GAME", mQuestionDelete);
		}
	}

	return FormBase::mouseReleased(arg, id);
}



void FormGames::hide()
{
	FormBase::hide();
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		mThumbs[i]->hide();
	hideAllOptions();
}

void FormGames::show()
{
	// correct position to see this form correctly
	resetCamera();

	FormBase::show();
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		mThumbs[i]->show();
}



void FormGames::buttonHit(Button* button)
{
	if (button->getName() == "FormGames/Button/CloseOptions")
	{
		hideOptions();
	}
	else if (button->getName() == "FormGames/Button/CloseOptionsThumbs")
	{
		hideOptionsThumbs();
		showOptions();
		ConfigReader::getSingletonPtr()->saveConfig();
	}
	else if (button->getName() == "FormGames/Button/DeleteClose")
	{
		hideOptionsDelete();
		showOptions();
	}
	else if (button->getName() == "FormGames/Button/DefaultThumbs")
	{
		mGridThumbs.rows = 3;
		mGridThumbs.top = 3;
		mGridThumbs.left = -20;
		mGridThumbs.horizontalSep = 11.5f;
		mGridThumbs.verticalSep = 6.25f;
		mGridThumbs.size = 2.7f;
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		for (unsigned int i = 0; i < mThumbs.size(); ++i)
			mThumbs[i]->setScale(Ogre::Vector3(mGridThumbs.size, mGridThumbs.size, mGridThumbs.size));

		ItemSelector* selectorRows = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormGames/Selector/Rows"));
		SliderOptions* sliderTop = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormGames/Slider/Top"));
		SliderOptions* sliderLeft = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormGames/Slider/Left"));
		SliderOptions* sliderHorSep = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormGames/Slider/SepHor"));
		SliderOptions* sliderVerSep = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormGames/Slider/SepVer"));
		SliderOptions* sliderSize = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormGames/Slider/Size"));
		if(selectorRows) selectorRows->selectOption(Ogre::StringConverter::toString(mGridThumbs.rows), false);
		if(sliderTop) sliderTop->setValue(mGridThumbs.top, false);
		if(sliderLeft) sliderLeft->setValue(mGridThumbs.left, false);
		if(sliderHorSep) sliderHorSep->setValue(mGridThumbs.horizontalSep, false);
		if(sliderVerSep) sliderVerSep->setValue(mGridThumbs.verticalSep, false);
		if(sliderSize) sliderSize->setValue(mGridThumbs.size, false);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.GAMES", "Thumbs_Rows", mGridThumbs.rows);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.GAMES", "Thumbs_Size", mGridThumbs.size);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.GAMES", "Thumbs_Top", mGridThumbs.top);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.GAMES", "Thumbs_Left", mGridThumbs.left);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.GAMES", "Thumbs_Horizontal_Sep", mGridThumbs.horizontalSep);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.GAMES", "Thumbs_Vertial_Sep", mGridThumbs.verticalSep);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormGames/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
}

void FormGames::sliderMoved(Slider* slider)
{
	mParentThumbs->setPosition(-slider->getValue(), 0, 0);
}

void FormGames::itemSelected(SelectMenu* menu)
{
	if (menu->getSelectedItem() == "All")
	{
		_filterGamesAll();
	}
	else if (menu->getSelectedItem() == "Installed")
	{
		_filterGamesByInstalled();
	}
}

void FormGames::labelHit(Label* label)
{
	if (label->getName() == "FormGames/Label/EditThumbs")
	{
		hideOptions();
		showOptionsThumbs();
	}
	else if (label->getName() == "FormGames/Label/NewGame")
	{
		hide();
		mTrayMgr->loadMenuBarNewGame();
		mFormNewGame = new FormNewGame(mTrayMgr, this);
	}
	else if (label->getName() == "FormGames/Label/DeleteGame")
	{
		hideOptions();
		showOptionsDelete();
	}
}

void FormGames::itemChanged(ItemSelector* selector)
{
	if (selector->getName() == "FormGames/Selector/Rows")
	{
		mGridThumbs.rows = Ogre::StringConverter::parseInt(selector->getSelectedOption());
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.GAMES", "Thumbs_Rows", mGridThumbs.rows);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormGames/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
}

void FormGames::sliderOptionsMoved(SliderOptions* slider)
{
	if (slider->getName() == "FormGames/Slider/Size")
	{
		mGridThumbs.size = slider->getValue();
		for (unsigned int i = 0; i < mThumbs.size(); ++i)
			mThumbs[i]->setScale(Ogre::Vector3(mGridThumbs.size, mGridThumbs.size, mGridThumbs.size));
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.GAMES", "Thumbs_Size", mGridThumbs.size);
	}
	else if (slider->getName() == "FormGames/Slider/Top")
	{
		mGridThumbs.top = -slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.GAMES", "Thumbs_Top", mGridThumbs.top);
	}
	else if (slider->getName() == "FormGames/Slider/Left")
	{
		mGridThumbs.left = slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.GAMES", "Thumbs_Left", mGridThumbs.left);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormGames/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
	else if (slider->getName() == "FormGames/Slider/SepHor")
	{
		mGridThumbs.horizontalSep = slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.GAMES", "Thumbs_Horizontal_Sep", mGridThumbs.horizontalSep);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormGames/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
	else if (slider->getName() == "FormGames/Slider/SepVer")
	{
		mGridThumbs.verticalSep = slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.GAMES", "Thumbs_Vertial_Sep", mGridThumbs.verticalSep);
	}
}

void FormGames::yesNoDialogClosed(const Ogre::DisplayString& question, bool yesHit)
{
	if (question == mQuestionDelete)
	{
		if (yesHit)
		{
			if (mLastThumbOver) _removeGame(mLastThumbOver->getIndex());
		}
		mLastThumbOver = 0;
	}
}



void FormGames::removeNewGameForm()
{
	if (mFormNewGame)
	{
		if (mFormNewGame->isNewGameAdded())
			_addNewGame(mFormNewGame->getName());

		mTrayMgr->loadMenuBarMain();
		mTrayMgr->enableFadeEffect();
		delete mFormNewGame;
		mFormNewGame = 0;
		enableForm();
		show();
	}
}



void FormGames::_addNewGame(const Ogre::String& newGame)
{
	// iterate over the main path game and add them 
	unsigned int indexNewGame = 0;
	for (boost::filesystem::directory_iterator it(mPathGames), end; it != end; ++it)
	{
		if (boost::filesystem::is_directory(it->path()) && it->path().filename().generic_string() == newGame)
		{
			mGameInfo.insert(mGameInfo.begin() + indexNewGame, new GameInfo(it->path().generic_string()));

			// create (if not) and load the thumbnail header game
			mInfoHeader.insert(mInfoHeader.begin() + indexNewGame, sInfoResource());
			mGameInfo[indexNewGame]->findHeaderResource(mInfoHeader[indexNewGame]);
			if (mInfoHeader[indexNewGame].path != Ogre::StringUtil::BLANK) // game has a header
			{
				if (!boost::filesystem::is_regular_file(mInfoHeader[indexNewGame].pathThumb))
					GameInfo::createThumbnail(mInfoHeader[indexNewGame].path, mInfoHeader[indexNewGame].pathThumb, 256);
				GameInfo::loadImageFromDisk(mInfoHeader[indexNewGame].pathThumb, mInfoHeader[indexNewGame].nameThumb, "Group/FormGames");
			}
			// create the 3d thumbnail widget
			mThumbs.insert(mThumbs.begin() + indexNewGame, new Thumbnail3D(mParentThumbs, "FormGames/Thumb/Widget/" + Ogre::StringConverter::toString(mThumbs.size()),
				"Group/FormGames", mGameInfo[indexNewGame]->getName(), mInfoHeader[indexNewGame].nameThumb));
			mThumbs[indexNewGame]->setIndex(indexNewGame);
			mThumbs[indexNewGame]->setScale(Ogre::Vector3(mGridThumbs.size, mGridThumbs.size, mGridThumbs.size));

			// set thumbs in a grid
			Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);

			break;
		}
		indexNewGame++;
	}
}

void FormGames::_removeGame(unsigned int index)
{
	boost::filesystem::remove_all(mGameInfo[index]->getPathGame());
	delete mGameInfo[index];
	delete mThumbs[index];
	mGameInfo.erase(mGameInfo.begin() + index);
	mInfoHeader.erase(mInfoHeader.begin() + index);
	mThumbs.erase(mThumbs.begin() + index);

	// a very bad solution [index thumbs] [fix] :(
	if (index < mThumbs.size())
	{
		for (unsigned int i = index; i < mThumbs.size(); ++i)
			mThumbs[i]->setIndex(mThumbs[i]->getIndex() - 1);
	}

	Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
}

void FormGames::_removeThumbs()
{
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		delete mThumbs[i];
	mThumbs.clear();
}

void FormGames::_adjustThumbs()
{
	if (!mThumbs.empty())
	{
		// adjust thumbnail in a grid
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);

		Slider* slider = (Slider *)mTrayMgr->getWidget("FormGames/Slider");
		if (slider)
		{
			if (mThumbs.size() > 9)
			{
				Ogre::Real minValue = 0;
				Ogre::Real maxValue = mThumbs.back()->getPosition().x;
				slider->show();
				slider->setRange(minValue, maxValue, 1000, false);
			}
			else
			{
				slider->hide();
			}
		}
	}
}

void FormGames::_filterGamesAll()
{
	_removeThumbs();

	for (unsigned int i = 0; i < mGameInfo.size(); ++i)
	{
		// create the 3d thumbnail widget
		mThumbs.push_back(new Thumbnail3D(mParentThumbs, "FormGames/Thumb/Widget/" + Ogre::StringConverter::toString(mThumbs.size()),
			"Group/FormGames", mGameInfo[i]->getName(), mInfoHeader[i].nameThumb));
		mThumbs.back()->setIndex(i);
	}

	_adjustThumbs();
}

void FormGames::_filterGamesByInstalled()
{
	_removeThumbs();

	for (unsigned int i = 0; i < mGameInfo.size(); ++i)
	{
		if (mGameInfo[i]->getValueOption("GAME.INFO", "Name", "???") == "Mass Effect 2")
		{
			// create the 3d thumbnail widget
			mThumbs.push_back(new Thumbnail3D(mParentThumbs, "FormGames/Thumb/Widget/" + Ogre::StringConverter::toString(mThumbs.size()),
				"Group/FormGames", mGameInfo[i]->getName(), mInfoHeader[i].nameThumb));
			mThumbs.back()->setIndex(i);
		}
	}
	
	_adjustThumbs();
}



void FormGames::hideAllOptions()
{
	hideOptions();
	hideOptionsThumbs();
	hideOptionsDelete();
}

void FormGames::hideOptions()
{
	if (mTrayMgr->getWidget("FormGames/Window/Options"))
	{
		mTrayMgr->destroyDialogWindow("FormGames/Window/Options");
		mTrayMgr->destroyWidget("FormGames/Label/NewGame");
		mTrayMgr->destroyWidget("FormGames/Label/DeleteGame");
		mTrayMgr->destroyWidget("FormGames/Label/EditThumbs");
		mTrayMgr->destroyWidget("FormGames/Label/Help");
		mTrayMgr->destroyWidget("FormGames/Button/CloseOptions");
	}
}

void FormGames::showOptions()
{
	// all submenu options are hiddens?
	if (!mTrayMgr->isWindowDialogVisible())
	{
		unsigned int numOptions = 5;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 380;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		mTrayMgr->createDialogWindow("FormGames/Window/Options", "GAMES OPTIONS", left, top, width, height);	top += sepOptions / 2;
		mTrayMgr->createLabel("FormGames/Label/NewGame", "NEW GAME", left, top, width, 23);	top += sepOptions;
		mTrayMgr->createLabel("FormGames/Label/DeleteGame", "DELETE GAME", left, top, width, 23);	top += sepOptions;
		mTrayMgr->createLabel("FormGames/Label/EditThumbs", "EDIT THUMBNAILS", left, top, width, 23);	top += sepOptions;
		mTrayMgr->createLabel("FormGames/Label/Help", "HELP", left, top, width, 23);		top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormGames/Button/CloseOptions", "BACK", left, top, 60);
	}
}

void FormGames::hideOptionsThumbs()
{
	if (mTrayMgr->getWidget("FormGames/Window/Thumbs"))
	{
		mTrayMgr->destroyDialogWindow("FormGames/Window/Thumbs");
		mTrayMgr->destroyWidget("FormGames/Selector/Rows");
		mTrayMgr->destroyWidget("FormGames/Slider/Top");
		mTrayMgr->destroyWidget("FormGames/Slider/Left");
		mTrayMgr->destroyWidget("FormGames/Slider/SepHor");
		mTrayMgr->destroyWidget("FormGames/Slider/SepVer");
		mTrayMgr->destroyWidget("FormGames/Slider/Size");
		mTrayMgr->destroyWidget("FormGames/Button/CloseOptionsThumbs");
		mTrayMgr->destroyWidget("FormGames/Button/DefaultThumbs");
	}
}

void FormGames::showOptionsThumbs()
{
	if (!mTrayMgr->getWidget("FormGames/Window/Thumbs")) // menu is hidden
	{
		unsigned int numOptions = 7;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 450;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		Ogre::StringVector itemsRows; 
		for (unsigned int i = 1; i < 5; ++i) itemsRows.push_back(Ogre::StringConverter::toString(i));

		mTrayMgr->createDialogWindow("FormGames/Window/Thumbs", "THUMBNAILS OPTIONS", left, top, width, height);											top += sepOptions / 2;
		ItemSelector* selectorRows = mTrayMgr->createItemSelector("FormGames/Selector/Rows", "Number of rows", itemsRows, left, top, width);				top += sepOptions;
		SliderOptions* sliderTop = mTrayMgr->createSliderOptions("FormGames/Slider/Top", "Thumbs Top", left, top, width, -10, 10, 100);					top += sepOptions;
		SliderOptions* sliderLeft = mTrayMgr->createSliderOptions("FormGames/Slider/Left", "Thumbs Left", left, top, width, -25, 0, 100);				top += sepOptions;
		SliderOptions* sliderHorSep = mTrayMgr->createSliderOptions("FormGames/Slider/SepHor", "Horizontal Separation", left, top, width, 1, 20, 100);	top += sepOptions;
		SliderOptions* sliderVerSep = mTrayMgr->createSliderOptions("FormGames/Slider/SepVer", "Vertical Separation", left, top, width, 1, 20, 100);		top += sepOptions;
		SliderOptions* sliderSize = mTrayMgr->createSliderOptions("FormGames/Slider/Size", "Thumbs size", left, top, width, 2, 5, 100);					top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormGames/Button/CloseOptionsThumbs", "BACK", left, top, 60);
		mTrayMgr->createButton("FormGames/Button/DefaultThumbs", "DEFAULT", left + 68, top, 85);

		selectorRows->selectOption(Ogre::StringConverter::toString(mGridThumbs.rows), false);
		sliderTop->setValue(-mGridThumbs.top, false);
		sliderLeft->setValue(mGridThumbs.left, false);
		sliderHorSep->setValue(mGridThumbs.horizontalSep, false);
		sliderVerSep->setValue(mGridThumbs.verticalSep, false);
		sliderSize->setValue(mGridThumbs.size, false);
	}
}

void FormGames::hideOptionsDelete()
{
	if (mTrayMgr->getWidget("FormGames/Window/DeleteGame"))
	{
		mTrayMgr->destroyDialogWindow("FormGames/Window/DeleteGame");
		mTrayMgr->destroyWidget("FormGames/Desc/InfoDelete");
		mTrayMgr->destroyWidget("FormGames/Button/DeleteClose");
	}
}

void FormGames::showOptionsDelete()
{
	if (!mTrayMgr->getWidget("FormGames/Window/DeleteGame"))
	{
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 450;
		Ogre::String infoCaption = "To delete a game from library you must click on the game you want to delete with the right mouse button.";
		SimpleText* infoText = mTrayMgr->createSimpleText("FormGames/Desc/InfoDelete", infoCaption, "YgcFont/SemiBold/16", width - 30, 250, 19, 9);
		Ogre::Real height = infoText->getNumLines() * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;

		mTrayMgr->createDialogWindow("FormGames/Window/DeleteGame", "DELETE GAME", left, top, width, height); top += sepOptions / 2;
		infoText->setLeft(left + 12); infoText->setTop(top + 8); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormGames/Button/DeleteClose", "BACK", left, top, 60);
	}
}

