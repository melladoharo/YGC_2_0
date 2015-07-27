#include "stdafx.h"
#include "FormImages.h"
#include "ConfigReader.h"

FormImages::FormImages(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener) : 
FormBase(tray, oldListener),
mParentThumbs(mSceneMgr->getRootSceneNode()->createChildSceneNode()),
mGameInfo(gInfo),
mLastThumbOver(0)
{
	resetCamera();
	// load grid-thumbs values from .ini config file
	mGridThumbs.rows = Ogre::StringConverter::parseInt(ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM_IMAGES", "Thumbs_Rows", "1"));
	mGridThumbs.top = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM_IMAGES", "Thumbs_Top", 0);
	mGridThumbs.left = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM_IMAGES", "Thumbs_Left", 0);
	mGridThumbs.horizontalSep = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM_IMAGES", "Thumbs_Horizontal_Sep", 0);
	mGridThumbs.verticalSep = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM_IMAGES", "Thumbs_Vertial_Sep", 0);
	mGridThumbs.size = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM_IMAGES", "Thumbs_Size", 0);

	// find all images of the game
	mGameInfo->findScreenshotsResources(mImages);
	mIndexScreenshots = std::make_pair(0, mImages.size());
	mGameInfo->findWallResource(mImages);
	mIndexWallpapers = std::make_pair(mIndexScreenshots.second, mImages.size());
	mGameInfo->findConceptArtResources(mImages);
	mIndexConceptArt = std::make_pair(mIndexWallpapers.second, mImages.size());

	// load thumbnails...
	for (unsigned int i = 0; i < mImages.size(); ++i)
	{
		if (!boost::filesystem::is_regular_file(mImages[i].pathThumb))
			GameInfo::createThumbnail(mImages[i].path, mImages[i].pathThumb, 256);
		GameInfo::loadImageFromDisk(mImages[i].pathThumb, mImages[i].nameThumb, mGameInfo->getGroupName(), 0);
		mThumbs.push_back(new Thumbnail3D(mParentThumbs, "FormImages/Thumb/" + Ogre::StringConverter::toString(i),
			mGameInfo->getGroupName(), mImages[i].filename, mImages[i].nameThumb));
		mThumbs.back()->setIndex(mThumbs.size() - 1);
		mThumbs.back()->setScale(Ogre::Vector3(mGridThumbs.size, mGridThumbs.size, mGridThumbs.size));
	}
	
	// ... and set them in a grid
	Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);

	// material for original-selected images
	Ogre::MaterialPtr matImage = Ogre::MaterialManager::getSingleton().create(
		"FormImage/Mat/Original", mGameInfo->getGroupName());

	// slider 
	Ogre::Real minValue = 0;
	Ogre::Real maxValue = (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0);
	Slider* sd = mTrayMgr->createSlider("FormImages/Slider", 370, minValue, maxValue, 1000);
	sd->getOverlayElement()->setVerticalAlignment(Ogre::GVA_BOTTOM);
	sd->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
	sd->setLeft(-(sd->getWidth() / 2));
	sd->setTop(-50);
	addWidgetToForm(sd);

	// select menu 
	Ogre::StringVector items;
		items.push_back("All Images"); items.push_back("Concept Art");
		items.push_back("Screenshots"); items.push_back("Wallpapers");
	SelectMenu* sm = mTrayMgr->createSelectMenu("FormImages/SelMenu", items, 275.0f);
	sm->getOverlayElement()->setVerticalAlignment(Ogre::GVA_TOP);
	sm->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
	sm->setTop(120);
	sm->setLeft(-sm->getWidth() / 2);
	addWidgetToForm(sm);
}

FormImages::~FormImages()
{
	Ogre::MaterialManager::getSingleton().remove("FormImage/Mat/Original");
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		delete mThumbs[i];
	mSceneMgr->destroySceneNode(mParentThumbs);
	hideAllOptions();
}



bool FormImages::keyPressed(const OIS::KeyEvent &arg)
{
	return FormBase::keyPressed(arg);
}

bool FormImages::keyReleased(const OIS::KeyEvent &arg)
{
	return FormBase::keyReleased(arg);
}

bool FormImages::mouseMoved(const OIS::MouseEvent &arg)
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

bool FormImages::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	return FormBase::mousePressed(arg, id);
}

bool FormImages::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;

	if (id == OIS::MouseButtonID::MB_Left)
	{
		if (mLastThumbOver)
		{
			unsigned int index = mLastThumbOver->getIndex();
			mLastThumbOver->setMouseUp();
			mLastThumbOver = 0;

			if (!Ogre::TextureManager::getSingleton().resourceExists(mImages[index].name))
				GameInfo::loadImageFromDisk(mImages[index].path, mImages[index].name, mGameInfo->getGroupName());
			Ogre::MaterialPtr matOriginal = Ogre::MaterialManager::getSingleton().getByName("FormImage/Mat/Original");
			matOriginal->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
			matOriginal->getTechnique(0)->getPass(0)->createTextureUnitState(mImages[index].name);

			mTrayMgr->showBackdrop(matOriginal->getName());
			mTrayMgr->autoResizeBackdrop();
			mTrayMgr->hideMenuBar();
			mTrayMgr->hideWidgets();
			hide();
		}
	}
	else if (id == OIS::MouseButtonID::MB_Right)
	{
		if (!mTrayMgr->isWidgetsVisible())
		{
			mTrayMgr->hideBackdrop();
			mTrayMgr->showMenuBar();
			mTrayMgr->showWidgets();
			show();
		}
	}

	return FormBase::mouseReleased(arg, id);
}



void FormImages::hide()
{
	FormBase::hide();
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		mThumbs[i]->hide();
	hideAllOptions();
}

void FormImages::show()
{
	// correct position to see this form correctly
	resetCamera();

	FormBase::show();
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		mThumbs[i]->show();
}



void FormImages::buttonHit(Button* button)
{
	if (button->getName() == "FormImages/Button/CloseOptions")
	{
		hideOptions();
	}
	else if (button->getName() == "FormImages/Button/CloseOptionsThumbs")
	{
		hideOptionsThumbs();
		showOptions();
		ConfigReader::getSingletonPtr()->saveConfig();
	}
	else if (button->getName() == "FormImages/Button/DefaultThumbs")
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

		ItemSelector* selectorRows = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormImages/Selector/Rows"));
		SliderOptions* sliderTop = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormImages/Slider/Top"));
		SliderOptions* sliderLeft = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormImages/Slider/Left"));
		SliderOptions* sliderHorSep = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormImages/Slider/SepHor"));
		SliderOptions* sliderVerSep = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormImages/Slider/SepVer"));
		SliderOptions* sliderSize = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormImages/Slider/Size"));
		if (selectorRows) selectorRows->selectOption(Ogre::StringConverter::toString(mGridThumbs.rows), false);
		if (sliderTop) sliderTop->setValue(mGridThumbs.top, false);
		if (sliderLeft) sliderLeft->setValue(mGridThumbs.left, false);
		if (sliderHorSep) sliderHorSep->setValue(mGridThumbs.horizontalSep, false);
		if (sliderVerSep) sliderVerSep->setValue(mGridThumbs.verticalSep, false);
		if (sliderSize) sliderSize->setValue(mGridThumbs.size, false);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM_IMAGES", "Thumbs_Rows", mGridThumbs.rows);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM_IMAGES", "Thumbs_Size", mGridThumbs.size);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM_IMAGES", "Thumbs_Top", mGridThumbs.top);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM_IMAGES", "Thumbs_Left", mGridThumbs.left);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM_IMAGES", "Thumbs_Horizontal_Sep", mGridThumbs.horizontalSep);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM_IMAGES", "Thumbs_Vertial_Sep", mGridThumbs.verticalSep);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormImages/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
}

void FormImages::sliderMoved(Slider* slider)
{
	mParentThumbs->setPosition(-slider->getValue(), 0, 0);
}

void FormImages::itemSelected(SelectMenu* menu)
{
	if (menu->getSelectedItem() == "All Images")
	{
		_filterImageByIndex(std::make_pair(0, mImages.size()));
	}
	else if (menu->getSelectedItem() == "Concept Art")
	{
		_filterImageByIndex(mIndexConceptArt);
	}
	else if (menu->getSelectedItem() == "Screenshots")
	{
		_filterImageByIndex(mIndexScreenshots);
	}
	else if (menu->getSelectedItem() == "Wallpapers")
	{
		_filterImageByIndex(mIndexWallpapers);
	}
}

void FormImages::labelHit(Label* label)
{
	if (label->getName() == "FormImages/Label/EditThumbs")
	{
		hideOptions();
		showOptionsThumbs();
	}
}

void FormImages::itemChanged(ItemSelector* selector)
{
	if (selector->getName() == "FormImages/Selector/Rows")
	{
		mGridThumbs.rows = Ogre::StringConverter::parseInt(selector->getSelectedOption());
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM_IMAGES", "Thumbs_Rows", mGridThumbs.rows);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormImages/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
}

void FormImages::sliderOptionsMoved(SliderOptions* slider)
{
	if (slider->getName() == "FormImages/Slider/Size")
	{
		mGridThumbs.size = slider->getValue();
		for (unsigned int i = 0; i < mThumbs.size(); ++i)
			mThumbs[i]->setScale(Ogre::Vector3(mGridThumbs.size, mGridThumbs.size, mGridThumbs.size));
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM_IMAGES", "Thumbs_Size", mGridThumbs.size);
	}
	else if (slider->getName() == "FormImages/Slider/Top")
	{
		mGridThumbs.top = -slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM_IMAGES", "Thumbs_Top", mGridThumbs.top);
	}
	else if (slider->getName() == "FormImages/Slider/Left")
	{
		mGridThumbs.left = slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM_IMAGES", "Thumbs_Left", mGridThumbs.left);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormImages/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
	else if (slider->getName() == "FormImages/Slider/SepHor")
	{
		mGridThumbs.horizontalSep = slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM_IMAGES", "Thumbs_Horizontal_Sep", mGridThumbs.horizontalSep);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormImages/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
	else if (slider->getName() == "FormImages/Slider/SepVer")
	{
		mGridThumbs.verticalSep = slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM_IMAGES", "Thumbs_Vertial_Sep", mGridThumbs.verticalSep);
	}
}



void FormImages::_filterImageByIndex(std::pair<unsigned int, unsigned int> indexFilter)
{
	mParentThumbs->detachAllObjects();
	mParentThumbs->setPosition(0, 0, 0);
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		delete mThumbs[i];
	mThumbs.clear();

	for (unsigned int i = indexFilter.first; i < indexFilter.second; ++i)
	{
		mThumbs.push_back(new Thumbnail3D(mParentThumbs, "FormImages/Thumb/" + Ogre::StringConverter::toString(i),
			mGameInfo->getGroupName(), mImages[i].filename, mImages[i].nameThumb));
		mThumbs.back()->setIndex(i);
	}

	if (!mThumbs.empty())
	{
		// adjust thumbnail in a grid
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);

		Slider* slider = (Slider *)mTrayMgr->getWidget("FormImages/Slider");
		if (slider)
		{
			if (mThumbs.size() > 9)
			{
				Ogre::Real minValue = 0;
				Ogre::Real maxValue = mThumbs.back()->getPosition().x;
				slider->show();
				slider->setRange(minValue, maxValue, maxValue + 100, false);
			}
			else
			{
				slider->hide();
			}
		}
	}
}



void FormImages::hideAllOptions()
{
	hideOptions();
	hideOptionsThumbs();
}

void FormImages::hideOptions()
{
	if (mTrayMgr->getWidget("FormImages/Window/Options"))
	{
		mTrayMgr->destroyDialogWindow("FormImages/Window/Options");
		mTrayMgr->destroyWidget("FormImages/Label/EditThumbs");
		mTrayMgr->destroyWidget("FormImages/Label/Help");
		mTrayMgr->destroyWidget("FormImages/Button/CloseOptions");
	}
}

void FormImages::showOptions()
{
	if (!mTrayMgr->getWidget("FormImages/Window/Options")) // menu is hidden
	{
		unsigned int numOptions = 3;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 450;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		mTrayMgr->createDialogWindow("FormImages/Window/Options", "FORM IMAGES OPTIONS", left, top, width, height); top += sepOptions / 2;
		mTrayMgr->createLabel("FormImages/Label/EditThumbs", "EDIT THUMBNAILS", left, top, width, 23);				top += sepOptions;
		mTrayMgr->createLabel("FormImages/Label/Help", "HELP", left, top, width, 23);								top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormImages/Button/CloseOptions", "BACK", left, top, 60);
	}
}

void FormImages::hideOptionsThumbs()
{
	if (mTrayMgr->getWidget("FormImages/Window/Thumbs"))
	{
		mTrayMgr->destroyDialogWindow("FormImages/Window/Thumbs");
		mTrayMgr->destroyWidget("FormImages/Selector/Rows");
		mTrayMgr->destroyWidget("FormImages/Slider/Top");
		mTrayMgr->destroyWidget("FormImages/Slider/Left");
		mTrayMgr->destroyWidget("FormImages/Slider/SepHor");
		mTrayMgr->destroyWidget("FormImages/Slider/SepVer");
		mTrayMgr->destroyWidget("FormImages/Slider/Size");
		mTrayMgr->destroyWidget("FormImages/Button/CloseOptionsThumbs");
		mTrayMgr->destroyWidget("FormImages/Button/DefaultThumbs");
	}
}

void FormImages::showOptionsThumbs()
{
	if (!mTrayMgr->getWidget("FormImages/Window/Thumbs")) // menu is hidden
	{
		unsigned int numOptions = 7;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 450;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		Ogre::StringVector itemsRows;
		for (unsigned int i = 1; i < 5; ++i) itemsRows.push_back(Ogre::StringConverter::toString(i));

		mTrayMgr->createDialogWindow("FormImages/Window/Thumbs", "THUMBNAILS OPTIONS", left, top, width, height);											top += sepOptions / 2;
		ItemSelector* selectorRows = mTrayMgr->createItemSelector("FormImages/Selector/Rows", "Number of rows", itemsRows, left, top, width);				top += sepOptions;
		SliderOptions* sliderTop = mTrayMgr->createSliderOptions("FormImages/Slider/Top", "Thumbs Top", left, top, width, -10, 10, 100);						top += sepOptions;
		SliderOptions* sliderLeft = mTrayMgr->createSliderOptions("FormImages/Slider/Left", "Thumbs Left", left, top, width, -25, 0, 100);					top += sepOptions;
		SliderOptions* sliderHorSep = mTrayMgr->createSliderOptions("FormImages/Slider/SepHor", "Horizontal Separation", left, top, width, 1, 20, 100);		top += sepOptions;
		SliderOptions* sliderVerSep = mTrayMgr->createSliderOptions("FormImages/Slider/SepVer", "Vertical Separation", left, top, width, 1, 20, 100);		top += sepOptions;
		SliderOptions* sliderSize = mTrayMgr->createSliderOptions("FormImages/Slider/Size", "Thumbs size", left, top, width, 2, 5, 100);						top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormImages/Button/CloseOptionsThumbs", "BACK", left, top, 60);
		mTrayMgr->createButton("FormImages/Button/DefaultThumbs", "DEFAULT", left + 68, top, 85);

		selectorRows->selectOption(Ogre::StringConverter::toString(mGridThumbs.rows), false);
		sliderTop->setValue(mGridThumbs.top, false);
		sliderLeft->setValue(mGridThumbs.left, false);
		sliderHorSep->setValue(mGridThumbs.horizontalSep, false);
		sliderVerSep->setValue(mGridThumbs.verticalSep, false);
		sliderSize->setValue(mGridThumbs.size, false);
	}
}

