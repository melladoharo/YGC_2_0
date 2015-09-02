#include "stdafx.h"
#include "FormSagas.h"

FormSagas::FormSagas(const Ogre::String& pathSagas, GuiManager* tray, GuiListener* oldListener /*= 0*/) : 
FormBase(tray, oldListener),
mParentThumbs(mSceneMgr->getRootSceneNode()->createChildSceneNode()),
mLastThumbOver(0)
{
	resetCamera();
	// load grid-thumbs values from .ini config file
	mGridThumbs.rows = Ogre::StringConverter::parseInt(ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM.SAGAS", "Thumbs_Rows", "1"));
	mGridThumbs.top = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM.SAGAS", "Thumbs_Top", 0);
	mGridThumbs.left = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM.SAGAS", "Thumbs_Left", 0);
	mGridThumbs.horizontalSep = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM.SAGAS", "Thumbs_Horizontal_Sep", 0);
	mGridThumbs.verticalSep = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM.SAGAS", "Thumbs_Vertial_Sep", 0);
	mGridThumbs.size = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM.SAGAS", "Thumbs_Size", 0);
	Ogre::String resThumb = ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM.SAGAS", "Thumbs_Resolution", 0);

	// initialize resource group 'FormSagas'
	Ogre::ResourceGroupManager::getSingleton().createResourceGroup("Group/FormSagas");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Group/FormSagas");

	// iterate over the main path game and add them 
	for (boost::filesystem::directory_iterator it(pathSagas), end; it != end; ++it)
	{
		if (boost::filesystem::is_directory(it->path()))
		{
			GameInfo infoSaga(it->path().generic_string());
			sInfoResource infoHeaderSaga;
			infoSaga.findHeaderResource(infoHeaderSaga);

			// header is not empty?
			if (infoHeaderSaga.path != Ogre::StringUtil::BLANK)
			{
				if (resThumb == "Source Size") // load the original image [full size]
				{
					GameInfo::loadImageFromDisk(infoHeaderSaga.path, infoHeaderSaga.nameThumb, "Group/FormSagas");
				}
				else // load the thumbnail [thumb size]
				{
					unsigned int resThumbValue = Ogre::StringConverter::parseInt(resThumb);
					if (!boost::filesystem::is_regular_file(infoHeaderSaga.pathThumb))
						GameInfo::createThumbnail(infoHeaderSaga.path, infoHeaderSaga.pathThumb, resThumbValue);
					GameInfo::loadImageFromDisk(infoHeaderSaga.pathThumb, infoHeaderSaga.nameThumb, "Group/FormSagas");
				}
			}
			// create the 3d thumbnail widget
			mThumbs.push_back(new Thumbnail3D(mParentThumbs, "FormSagas/Thumb/Widget/" + Ogre::StringConverter::toString(mThumbs.size()),
				"Group/FormSagas", infoSaga.getName(), infoHeaderSaga.nameThumb));
			mThumbs.back()->setIndex(mThumbs.size() - 1);
			mThumbs.back()->setScale(Ogre::Vector3(mGridThumbs.size, mGridThumbs.size, mGridThumbs.size));
		}
	}

	// set thumbs in a grid
	Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);

	// bottom slider
	Ogre::Real minValue = 0;
	Ogre::Real maxValue = (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0);
	Slider* sd = mTrayMgr->createSlider("FormSagas/Slider", 500, minValue, maxValue, 1000);
	sd->getOverlayElement()->setVerticalAlignment(Ogre::GVA_BOTTOM);
	sd->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
	sd->setLeft(-(sd->getWidth() / 2)); // center slider 
	sd->setTop(-50);
	addWidgetToForm(sd);
}

FormSagas::~FormSagas()
{
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		delete mThumbs[i];
	mSceneMgr->destroySceneNode(mParentThumbs);
	Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("Group/FormSagas");
	hideAllOptions();
}



bool FormSagas::mouseMoved(const OIS::MouseEvent &arg)
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

bool FormSagas::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;
	return FormBase::mousePressed(arg, id);
}

bool FormSagas::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
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
			//mFormSelector = new FormSelectorGame(mGameInfo[index], mTrayMgr, this);
		}
	}

	return FormBase::mouseReleased(arg, id);
}



void FormSagas::hide()
{
	FormBase::hide();
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		mThumbs[i]->hide();
	hideAllOptions();
}

void FormSagas::show()
{
	// correct position to see this form correctly
	resetCamera();

	FormBase::show();
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		mThumbs[i]->show();
}



void FormSagas::sliderMoved(Slider* slider)
{
	mParentThumbs->setPosition(-slider->getValue(), 0, 0);
}

void FormSagas::buttonHit(Button* button)
{
	if (button->getName() == "FormSagas/Button/CloseOptions")
	{
		hideOptions();
	}
	else if (button->getName() == "FormSagas/Button/CloseOptionsThumbs")
	{
		ConfigReader::getSingletonPtr()->saveConfig();
		hideOptionsThumbs();
		showOptions();
	}
	else if (button->getName() == "FormSagas/Button/DefaultThumbs")
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

		ItemSelector* selectorRows = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormSagas/Selector/Rows"));
		SliderOptions* sliderTop = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormSagas/Slider/Top"));
		SliderOptions* sliderLeft = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormSagas/Slider/Left"));
		SliderOptions* sliderHorSep = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormSagas/Slider/SepHor"));
		SliderOptions* sliderVerSep = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormSagas/Slider/SepVer"));
		SliderOptions* sliderSize = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormSagas/Slider/Size"));
		if (selectorRows) selectorRows->selectOption(Ogre::StringConverter::toString(mGridThumbs.rows), false);
		if (sliderTop) sliderTop->setValue(mGridThumbs.top, false);
		if (sliderLeft) sliderLeft->setValue(mGridThumbs.left, false);
		if (sliderHorSep) sliderHorSep->setValue(mGridThumbs.horizontalSep, false);
		if (sliderVerSep) sliderVerSep->setValue(mGridThumbs.verticalSep, false);
		if (sliderSize) sliderSize->setValue(mGridThumbs.size, false);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.SAGAS", "Thumbs_Rows", mGridThumbs.rows);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.SAGAS", "Thumbs_Size", mGridThumbs.size);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.SAGAS", "Thumbs_Top", mGridThumbs.top);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.SAGAS", "Thumbs_Left", mGridThumbs.left);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.SAGAS", "Thumbs_Horizontal_Sep", mGridThumbs.horizontalSep);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.SAGAS", "Thumbs_Vertial_Sep", mGridThumbs.verticalSep);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormSagas/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
}

void FormSagas::labelHit(Label* label)
{
	if (label->getName() == "FormSagas/Label/EditThumbs")
	{
		hideOptions();
		showOptionsThumbs();
	}
}

void FormSagas::itemChanged(ItemSelector* selector)
{
	if (selector->getName() == "FormSagas/Selector/Rows")
	{
		mGridThumbs.rows = Ogre::StringConverter::parseInt(selector->getSelectedOption());
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.SAGAS", "Thumbs_Rows", mGridThumbs.rows);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormSagas/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
}

void FormSagas::sliderOptionsMoved(SliderOptions* slider)
{
	if (slider->getName() == "FormSagas/Slider/Size")
	{
		mGridThumbs.size = slider->getValue();
		for (unsigned int i = 0; i < mThumbs.size(); ++i)
			mThumbs[i]->setScale(Ogre::Vector3(mGridThumbs.size, mGridThumbs.size, mGridThumbs.size));
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.SAGAS", "Thumbs_Size", mGridThumbs.size);
	}
	else if (slider->getName() == "FormSagas/Slider/Top")
	{
		mGridThumbs.top = -slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.SAGAS", "Thumbs_Top", mGridThumbs.top);
	}
	else if (slider->getName() == "FormSagas/Slider/Left")
	{
		mGridThumbs.left = slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.SAGAS", "Thumbs_Left", mGridThumbs.left);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormSagas/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
	else if (slider->getName() == "FormSagas/Slider/SepHor")
	{
		mGridThumbs.horizontalSep = slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.SAGAS", "Thumbs_Horizontal_Sep", mGridThumbs.horizontalSep);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormSagas/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
	else if (slider->getName() == "FormSagas/Slider/SepVer")
	{
		mGridThumbs.verticalSep = slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.SAGAS", "Thumbs_Vertial_Sep", mGridThumbs.verticalSep);
	}
}


void FormSagas::hideAllOptions()
{
	hideOptions();
	hideOptionsThumbs();
}

void FormSagas::hideOptions()
{
	if (mTrayMgr->getWidget("FormSagas/Window/Options"))
	{
		mTrayMgr->destroyDialogWindow("FormSagas/Window/Options");
		mTrayMgr->destroyWidget("FormSagas/Label/NewSaga");
		mTrayMgr->destroyWidget("FormSagas/Label/DeleteSaga");
		mTrayMgr->destroyWidget("FormSagas/Label/EditThumbs");
		mTrayMgr->destroyWidget("FormSagas/Label/Help");
		mTrayMgr->destroyWidget("FormSagas/Button/CloseOptions");
	}
}

void FormSagas::showOptions()
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
		mTrayMgr->createDialogWindow("FormSagas/Window/Options", "SAGAS OPTIONS", left, top, width, height);	top += sepOptions / 2;
		mTrayMgr->createLabel("FormSagas/Label/NewSaga", "NEW SAGA", left, top, width, 23);	top += sepOptions;
		mTrayMgr->createLabel("FormSagas/Label/DeleteSaga", "DELETE SAGA", left, top, width, 23);	top += sepOptions;
		mTrayMgr->createLabel("FormSagas/Label/EditThumbs", "EDIT THUMBNAILS", left, top, width, 23);	top += sepOptions;
		mTrayMgr->createLabel("FormSagas/Label/Help", "HELP", left, top, width, 23);		top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormSagas/Button/CloseOptions", "BACK", left, top, 60);
	}
}

void FormSagas::hideOptionsThumbs()
{
	if (mTrayMgr->getWidget("FormSagas/Window/Thumbs"))
	{
		mTrayMgr->destroyDialogWindow("FormSagas/Window/Thumbs");
		mTrayMgr->destroyWidget("FormSagas/Selector/Rows");
		mTrayMgr->destroyWidget("FormSagas/Slider/Top");
		mTrayMgr->destroyWidget("FormSagas/Slider/Left");
		mTrayMgr->destroyWidget("FormSagas/Slider/SepHor");
		mTrayMgr->destroyWidget("FormSagas/Slider/SepVer");
		mTrayMgr->destroyWidget("FormSagas/Slider/Size");
		mTrayMgr->destroyWidget("FormSagas/Button/CloseOptionsThumbs");
		mTrayMgr->destroyWidget("FormSagas/Button/DefaultThumbs");
	}
}

void FormSagas::showOptionsThumbs()
{
	if (!mTrayMgr->getWidget("FormSagas/Window/Thumbs")) // menu is hidden
	{
		unsigned int numOptions = 7;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 450;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		Ogre::StringVector itemsRows;
		for (unsigned int i = 1; i < 5; ++i) itemsRows.push_back(Ogre::StringConverter::toString(i));

		mTrayMgr->createDialogWindow("FormSagas/Window/Thumbs", "THUMBNAILS OPTIONS", left, top, width, height);											top += sepOptions / 2;
		ItemSelector* selectorRows = mTrayMgr->createItemSelector("FormSagas/Selector/Rows", "Number of rows", itemsRows, left, top, width);				top += sepOptions;
		SliderOptions* sliderTop = mTrayMgr->createSliderOptions("FormSagas/Slider/Top", "Thumbs Top", left, top, width, -10, 10, 100);					top += sepOptions;
		SliderOptions* sliderLeft = mTrayMgr->createSliderOptions("FormSagas/Slider/Left", "Thumbs Left", left, top, width, -25, 0, 100);				top += sepOptions;
		SliderOptions* sliderHorSep = mTrayMgr->createSliderOptions("FormSagas/Slider/SepHor", "Horizontal Separation", left, top, width, 1, 20, 100);	top += sepOptions;
		SliderOptions* sliderVerSep = mTrayMgr->createSliderOptions("FormSagas/Slider/SepVer", "Vertical Separation", left, top, width, 1, 20, 100);		top += sepOptions;
		SliderOptions* sliderSize = mTrayMgr->createSliderOptions("FormSagas/Slider/Size", "Thumbs size", left, top, width, 2, 10, 100);					top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormSagas/Button/CloseOptionsThumbs", "BACK", left, top, 60);
		mTrayMgr->createButton("FormSagas/Button/DefaultThumbs", "DEFAULT", left + 68, top, 85);

		selectorRows->selectOption(Ogre::StringConverter::toString(mGridThumbs.rows), false);
		sliderTop->setValue(-mGridThumbs.top, false);
		sliderLeft->setValue(mGridThumbs.left, false);
		sliderHorSep->setValue(mGridThumbs.horizontalSep, false);
		sliderVerSep->setValue(mGridThumbs.verticalSep, false);
		sliderSize->setValue(mGridThumbs.size, false);
	}
}

