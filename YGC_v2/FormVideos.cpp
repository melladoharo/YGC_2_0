#include "stdafx.h"
#include "FormVideos.h"
#include "ConfigReader.h"

FormVideos::FormVideos(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener /*= 0*/) :
FormBase(tray, oldListener),
mGameInfo(gInfo),
mLastThumbOver(0),
mParentThumbs(mSceneMgr->getRootSceneNode()->createChildSceneNode()),
mVideoPlayer(0),
mTimeOver(0.0f),
mCurrentIndex(0)
{
	resetCamera();
	// load grid-thumbs values from .ini config file
	mGridThumbs.rows = Ogre::StringConverter::parseInt(ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM.VIDEOS", "Thumbs_Rows", "1"));
	mGridThumbs.top = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM.VIDEOS", "Thumbs_Top", 0);
	mGridThumbs.left = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM.VIDEOS", "Thumbs_Left", 0);
	mGridThumbs.horizontalSep = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM.VIDEOS", "Thumbs_Horizontal_Sep", 0);
	mGridThumbs.verticalSep = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM.VIDEOS", "Thumbs_Vertial_Sep", 0);
	mGridThumbs.size = ConfigReader::getSingletonPtr()->getReader()->GetDoubleValue("FORM.VIDEOS", "Thumbs_Size", 0);
	Ogre::String resThumb = ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM.VIDEOS", "Thumbs_Resolution", 0);

	// find all videos of the game [trailers + gameplays]
	mGameInfo->findTrailersResources(mVideos);
	mIndexTrailer = std::make_pair(0, mVideos.size());
	mGameInfo->findGameplaysResources(mVideos);
	mIndexGameplay = std::make_pair(mIndexTrailer.second, mVideos.size());

	// load thumbnails...
	for (unsigned int i = 0; i < mVideos.size(); ++i)
	{
		unsigned int resThumbValue = (resThumb != "Source Size") ? Ogre::StringConverter::parseInt(resThumb) : 256;
		if (!boost::filesystem::is_regular_file(mVideos[i].pathThumb))
			_createThumbFromVideo(mVideos[i].path, mVideos[i].pathThumb, resThumbValue);
		GameInfo::loadImageFromDisk(mVideos[i].pathThumb, mVideos[i].nameThumb, mGameInfo->getGroupName());
		mThumbs.push_back(new Thumbnail3D(mParentThumbs, "FormVideos/Thumb/" + Ogre::StringConverter::toString(i),
			mGameInfo->getGroupName(), mVideos[i].caption, mVideos[i].nameThumb));
		mThumbs.back()->setIndex(i);
		mThumbs.back()->setScale(Ogre::Vector3(mGridThumbs.size, mGridThumbs.size, mGridThumbs.size));
	}

	// ... and set them in a grid
	Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);

	// slider 
	Ogre::Real minValue = 0;
	Ogre::Real maxValue = (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0);
	Slider* sd = mTrayMgr->createSlider("FormVideos/Slider", 370, minValue, maxValue, 500);
	sd->getOverlayElement()->setVerticalAlignment(Ogre::GVA_BOTTOM);
	sd->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
	sd->setLeft(-(sd->getWidth() / 2));
	sd->setTop(-50);
	addWidgetToForm(sd);

	// select menu
	Ogre::StringVector items;
		items.push_back("All Videos");
		items.push_back("Trailers");
		items.push_back("Gameplays");
	SelectMenu* sm = mTrayMgr->createSelectMenu("FormVideos/SelMenu", items, 275.0f);
	sm->getOverlayElement()->setVerticalAlignment(Ogre::GVA_TOP);
	sm->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
	sm->setTop(120);
	sm->setLeft(-sm->getWidth() / 2);
	addWidgetToForm(sm);

	mPlayer = mTrayMgr->createMediaPlayer("FormVideos/MediaPlayer");
	mPlayer->_assignListener(this);
	mPlayer->_assignSliderListener(this);
	show();
}

FormVideos::~FormVideos()
{
	mSceneMgr->destroySceneNode(mParentThumbs);
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		delete mThumbs[i];
	if (mVideoPlayer) { mVideoPlayer->close(); delete mVideoPlayer;}
	Ogre::MaterialManager::getSingleton().unload("Mat/FormVideos/Videoplayer");
	Ogre::MaterialManager::getSingleton().remove("Mat/FormVideos/Videoplayer");
	hideAllOptions();
	mTrayMgr->destroyMediaPlayer(mPlayer);
}



bool FormVideos::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (mVideoPlayer)
	{
		mVideoPlayer->update();
		mPlayer->setSliderValue(mVideoPlayer->getCurrentTime(), mVideoPlayer->getDuration(), false);
	}

	if (mTimeOver > 5.0f && !mVideoPlayer)
	{
		if (mLastThumbOver)
		{
			mCurrentIndex = mLastThumbOver->getIndex();
			playVideo(mCurrentIndex, false);
			mLastThumbOver->setMaterialWidget("Mat/FormVideos/Videoplayer");
		}
	}

	mTimeOver += evt.timeSinceLastFrame;

	return FormBase::frameRenderingQueued(evt);
}

bool FormVideos::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_LEFT)
	{
		mVideoPlayer->seek(mVideoPlayer->getCurrentTime() - 10);
	}
	else if (arg.key == OIS::KC_RIGHT)
	{
		mVideoPlayer->seek(mVideoPlayer->getCurrentTime() + 10);
	}
	else if (arg.key == OIS::KC_SPACE)
	{
		if (mVideoPlayer->isPaused()) mVideoPlayer->play();
		else mVideoPlayer->pause();
	}
	return FormBase::keyPressed(arg);
}

bool FormVideos::keyReleased(const OIS::KeyEvent &arg)
{
	return FormBase::keyReleased(arg);
}

bool FormVideos::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;

	if (mPlayer->isVisible())
		mPlayer->_cursorMoved(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));
	else 
	{
		// show the icon settings if cursor is over the right bottom corner
		if (mIconSettings->isVisible())
			mIconSettings->hide();

		if (arg.state.X.abs <= 50 && arg.state.Y.abs >= (mScreenSize.y - 50))
		{
			mIconSettings->show();
		}
	}

	// is a option windows or mediaplayer visible? if not, continue
	if (!mTrayMgr->isWindowDialogVisible() && !mTrayMgr->isMiniPlayerVisible()) 
	{
		if (mVideoPlayer && arg.state.Y.abs >= (mScreenSize.y - 55)) mPlayer->show();
		else mPlayer->hide();
		
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
		if (currentOver)
		{
			mLastThumbOver = currentOver;
			mTimeOver = 0.0f;
		}
		else if (mLastThumbOver)
		{
			if (mVideoPlayer) { mVideoPlayer->close(); delete mVideoPlayer; }
			mVideoPlayer = 0;
			mLastThumbOver->setMouseUp(); mLastThumbOver = 0; mTimeOver = 0.0f;
			mTrayMgr->playMiniPlayer();
		}
	}
	
	//return FormBase::mouseMoved(arg);
}

bool FormVideos::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	if (mPlayer->isVisible())
		mPlayer->_cursorPressed(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));

	return FormBase::mousePressed(arg, id);;
}

bool FormVideos::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;

	if (mPlayer->isVisible())
		mPlayer->_cursorReleased(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));

	if (id == OIS::MouseButtonID::MB_Left)
	{
		if (mLastThumbOver)
		{
			mCurrentIndex = mLastThumbOver->getIndex();
			mLastThumbOver->setMouseUp();
			mLastThumbOver = 0;

			Ogre::Real currentTime = 0.0f;
			if (mVideoPlayer)
				currentTime = mVideoPlayer->getCurrentTime();
			playVideo(mCurrentIndex);
			mVideoPlayer->seek(currentTime);
			mTrayMgr->autoResizeBackdrop();
			mTrayMgr->hideMenuBar();			
			hide();
		}
	}
	else if (id == OIS::MouseButtonID::MB_Right)
	{
		// mouse over a thumb, 
		if (mLastThumbOver)
		{
			if (mVideoPlayer)
			{
				mVideoPlayer->seek(mVideoPlayer->getCurrentTime() + 10);
			}
			else
			{
				mCurrentIndex = mLastThumbOver->getIndex();
				playVideo(mCurrentIndex, false);
				mLastThumbOver->setMaterialWidget("Mat/FormVideos/Videoplayer");
			}
		}

		// playing a video? then close the video
		if (mTrayMgr->isBackdropVisible())
		{
			if (mVideoPlayer) { mVideoPlayer->close(); delete mVideoPlayer; } mVideoPlayer = 0;
			mTrayMgr->hideBackdrop();
			mTrayMgr->showMenuBar();
			mTrayMgr->showWidgets();
			mTrayMgr->playMiniPlayer();
			mPlayer->hide(); 
			show();
		}
	}

	return FormBase::mouseReleased(arg, id);
}



void FormVideos::hide()
{
	FormBase::hide();
	mPlayer->hide();
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		mThumbs[i]->hide();
	hideAllOptions();
	Widget* logoGame = mTrayMgr->getWidget("FormOverview/Logo");
	if (logoGame) logoGame->hide();
}

void FormVideos::show()
{
	// correct position to see this form correctly
	resetCamera();

	FormBase::show();
	mPlayer->hide();
	// show slider if images are not empty
	if (mVideos.size() < mGridThumbs.rows * 2)
		mTrayMgr->getWidget("FormVideos/Slider")->hide();

	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		mThumbs[i]->show();

	Widget* logoGame = mTrayMgr->getWidget("FormOverview/Logo");
	if (logoGame) logoGame->show();
}



void FormVideos::buttonHit(Button* button)
{
	if (button->getName() == "FormVideos/Button/CloseOptions")
	{
		hideOptions();
	}
	else if (button->getName() == "FormVideos/Button/CloseOptionsThumbs")
	{
		hideOptionsThumbs();
		showOptions();
		ConfigReader::getSingletonPtr()->saveConfig();
	}
	else if (button->getName() == "FormVideos/Button/DefaultThumbs")
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

		ItemSelector* selectorRows = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormVideos/Selector/Rows"));
		SliderOptions* sliderTop = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormVideos/Slider/Top"));
		SliderOptions* sliderLeft = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormVideos/Slider/Left"));
		SliderOptions* sliderHorSep = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormVideos/Slider/SepHor"));
		SliderOptions* sliderVerSep = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormVideos/Slider/SepVer"));
		SliderOptions* sliderSize = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormVideos/Slider/Size"));
		if (selectorRows) selectorRows->selectOption(Ogre::StringConverter::toString(mGridThumbs.rows), false);
		if (sliderTop) sliderTop->setValue(mGridThumbs.top, false);
		if (sliderLeft) sliderLeft->setValue(mGridThumbs.left, false);
		if (sliderHorSep) sliderHorSep->setValue(mGridThumbs.horizontalSep, false);
		if (sliderVerSep) sliderVerSep->setValue(mGridThumbs.verticalSep, false);
		if (sliderSize) sliderSize->setValue(mGridThumbs.size, false);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.VIDEOS", "Thumbs_Rows", mGridThumbs.rows);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.VIDEOS", "Thumbs_Size", mGridThumbs.size);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.VIDEOS", "Thumbs_Top", mGridThumbs.top);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.VIDEOS", "Thumbs_Left", mGridThumbs.left);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.VIDEOS", "Thumbs_Horizontal_Sep", mGridThumbs.horizontalSep);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.VIDEOS", "Thumbs_Vertial_Sep", mGridThumbs.verticalSep);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormVideos/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
}

void FormVideos::sliderMoved(Slider* slider)
{
	if (slider->getName() == "FormVideos/Slider") // thumbs slider
	{
		mParentThumbs->setPosition(-slider->getValue(), 0, 0);
	}
}

void FormVideos::itemSelected(SelectMenu* menu)
{
	if (menu->getSelectedItem() == "All Videos")
	{
		_filterVideosByIndex(std::make_pair(0, mVideos.size()));
	}
	else if (menu->getSelectedItem() == "Trailers")
	{
		_filterVideosByIndex(mIndexTrailer);
	}
	else if (menu->getSelectedItem() == "Gameplays")
	{
		_filterVideosByIndex(mIndexGameplay);
	}
}

void FormVideos::mediaPlayerHit(MediaPlayer* player)
{
	switch (player->getSelectedAction())
	{
	case(MP_PLAY):
		mVideoPlayer->play();
		break;
	case(MP_PAUSE):
		mVideoPlayer->pause();
		break;
	case(MP_STOP):
		mVideoPlayer->close();
		delete mVideoPlayer;
		mVideoPlayer = 0;
		mTrayMgr->hideBackdrop();
		mTrayMgr->showMenuBar();
		mTrayMgr->showWidgets();
		mTrayMgr->playMiniPlayer();
		show();
		break;
	case(MP_NEXT):
		mCurrentIndex = (mCurrentIndex == mVideos.size() - 1) ? 0 : mCurrentIndex + 1;
		playVideo(mCurrentIndex);
		break;
	case(MP_PREV):
		mCurrentIndex = (mCurrentIndex == 0) ? mVideos.size() - 1 : mCurrentIndex - 1;
		playVideo(mCurrentIndex);
		break;
	case(MP_VOLUME):
		break;
	}
}

void FormVideos::labelHit(Label* label)
{
	if (label->getName() == "FormVideos/Label/EditThumbs")
	{
		hideOptions();
		showOptionsThumbs();
	}
}

void FormVideos::itemChanged(ItemSelector* selector)
{
	if (selector->getName() == "FormVideos/Selector/Rows")
	{
		mGridThumbs.rows = Ogre::StringConverter::parseInt(selector->getSelectedOption());
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.VIDEOS", "Thumbs_Rows", mGridThumbs.rows);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormVideos/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
}

void FormVideos::sliderOptionsMoved(SliderOptions* slider)
{
	if (slider->getName() == "FormVideos/Slider/Size")
	{
		mGridThumbs.size = slider->getValue();
		for (unsigned int i = 0; i < mThumbs.size(); ++i)
			mThumbs[i]->setScale(Ogre::Vector3(mGridThumbs.size, mGridThumbs.size, mGridThumbs.size));
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.VIDEOS", "Thumbs_Size", mGridThumbs.size);
	}
	else if (slider->getName() == "FormVideos/Slider/Top")
	{
		mGridThumbs.top = -slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.VIDEOS", "Thumbs_Top", mGridThumbs.top);
	}
	else if (slider->getName() == "FormVideos/Slider/Left")
	{
		mGridThumbs.left = slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.VIDEOS", "Thumbs_Left", mGridThumbs.left);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormVideos/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
	else if (slider->getName() == "FormVideos/Slider/SepHor")
	{
		mGridThumbs.horizontalSep = slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.VIDEOS", "Thumbs_Horizontal_Sep", mGridThumbs.horizontalSep);
		Slider* sd = dynamic_cast<Slider*>(mTrayMgr->getWidget("FormVideos/Slider"));
		sd->setRange(0, (!mThumbs.empty() ? mThumbs.back()->getPosition().x : 0), 1000, false);
	}
	else if (slider->getName() == "FormVideos/Slider/SepVer")
	{
		mGridThumbs.verticalSep = slider->getValue();
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);
		ConfigReader::getSingletonPtr()->getReader()->SetDoubleValue("FORM.VIDEOS", "Thumbs_Vertial_Sep", mGridThumbs.verticalSep);
	}
	else // player slider
	{
		mVideoPlayer->seek(slider->getValue());
	}
}



void FormVideos::playVideo(unsigned int index, bool fullscreen /*= true*/)
{
	// video material
	if (!Ogre::MaterialManager::getSingleton().resourceExists("Mat/FormVideos/Videoplayer"))
		Ogre::MaterialManager::getSingleton().create("Mat/FormVideos/Videoplayer", mGameInfo->getGroupName());
	Ogre::MaterialPtr matVideo = Ogre::MaterialManager::getSingleton().getByName("Mat/FormVideos/Videoplayer");
	matVideo->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	matVideo->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
	matVideo->getTechnique(0)->getPass(0)->createTextureUnitState();

	if (index >= 0 && index < mVideos.size())
	{
		// video player
		if (mVideoPlayer)
		{
			mVideoPlayer->close();
			delete mVideoPlayer;
			mVideoPlayer = 0;
		}
		mVideoPlayer = new Video::VideoPlayer;
		mVideoPlayer->setAudioFactory(new SDLMovieAudioFactory());
		mVideoPlayer->playVideo(mVideos[index].filename);
		mPlayer->setSliderRange(0, mVideoPlayer->getDuration(), mVideoPlayer->getDuration() + 1);
		mTrayMgr->pauseMiniPlayer();

		if (!mVideoPlayer->getTextureName().empty())
		{
			matVideo->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(mVideoPlayer->getTextureName());
			if (fullscreen) mTrayMgr->showBackdrop(matVideo->getName());
		}
	}
}



void FormVideos::_createThumbFromVideo(const Ogre::String& pathVideo, const Ogre::String& pathThumb, unsigned int thumbHeight)
{
	// Comprueba que existe el ffmpeg, si no muestra un mensaje de error
	boost::filesystem::path pathffmpeg("./ffmpeg.exe");
	if (boost::filesystem::is_regular_file(pathffmpeg))
	{
		//...ffmpeg.exe -ss 15 -i video.mp4 -vf scale=512:-1 -vframes 1 OutputFile.jpg
		Ogre::String sizeThumb = "scale=" + Ogre::StringConverter::toString(thumbHeight) + ":-1";
		Ogre::String processToStart("ffmpeg.exe");
		Ogre::String processParams("-ss 5 -i \"" + pathVideo + "\" -vf " + sizeThumb + " -vframes 1 \"" + pathThumb +"\"");

		SHELLEXECUTEINFO shellExInfo;
		shellExInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		shellExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shellExInfo.hwnd = NULL;
		shellExInfo.lpVerb = "open";
		shellExInfo.lpFile = processToStart.c_str();
		shellExInfo.lpParameters = processParams.c_str();
		shellExInfo.lpDirectory = NULL;
		shellExInfo.nShow = SW_HIDE;
		shellExInfo.hInstApp = NULL;
		ShellExecuteEx(&shellExInfo); // empieza el proceso
		WaitForSingleObject(shellExInfo.hProcess, INFINITE); // espera a que termine el nuevo proceso
	}
}

void FormVideos::_filterVideosByIndex(std::pair<unsigned int, unsigned int> indexFilter)
{
	mParentThumbs->detachAllObjects();
	mParentThumbs->setPosition(0, 0, 0);
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		delete mThumbs[i];
	mThumbs.clear();

	for (unsigned int i = indexFilter.first; i < indexFilter.second; ++i)
	{
		mThumbs.push_back(new Thumbnail3D(mParentThumbs, "FormVideos/Thumb/" + Ogre::StringConverter::toString(i),
			mGameInfo->getGroupName(), mVideos[i].filename, mVideos[i].nameThumb));
		mThumbs.back()->setIndex(i);
	}

	if (!mThumbs.empty())
	{
		// adjust thumbnail in a grid
		Thumbnail3D::setThumbs3DInGrid(mThumbs, mGridThumbs);

		Slider* slider = (Slider *)mTrayMgr->getWidget("FormVideos/Slider");
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



void FormVideos::hideAllOptions()
{
	hideOptions();
	hideOptionsThumbs();
}

void FormVideos::hideOptions()
{
	if (mTrayMgr->getWidget("FormVideos/Window/Options"))
	{
		mTrayMgr->destroyDialogWindow("FormVideos/Window/Options");
		mTrayMgr->destroyWidget("FormVideos/Label/EditThumbs");
		mTrayMgr->destroyWidget("FormVideos/Label/Help");
		mTrayMgr->destroyWidget("FormVideos/Button/CloseOptions");
	}
}

void FormVideos::showOptions()
{
	if (!mTrayMgr->getWidget("FormVideos/Window/Options")) // menu is hidden
	{
		unsigned int numOptions = 3;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 450;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		mTrayMgr->createDialogWindow("FormVideos/Window/Options", "FORM IMAGES OPTIONS", left, top, width, height); top += sepOptions / 2;
		mTrayMgr->createLabel("FormVideos/Label/EditThumbs", "EDIT THUMBNAILS", left, top, width, 23);				top += sepOptions;
		mTrayMgr->createLabel("FormVideos/Label/Help", "HELP", left, top, width, 23);								top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormVideos/Button/CloseOptions", "BACK", left, top, 60);
	}
}

void FormVideos::hideOptionsThumbs()
{
	if (mTrayMgr->getWidget("FormVideos/Window/Thumbs"))
	{
		mTrayMgr->destroyDialogWindow("FormVideos/Window/Thumbs");
		mTrayMgr->destroyWidget("FormVideos/Selector/Rows");
		mTrayMgr->destroyWidget("FormVideos/Slider/Top");
		mTrayMgr->destroyWidget("FormVideos/Slider/Left");
		mTrayMgr->destroyWidget("FormVideos/Slider/SepHor");
		mTrayMgr->destroyWidget("FormVideos/Slider/SepVer");
		mTrayMgr->destroyWidget("FormVideos/Slider/Size");
		mTrayMgr->destroyWidget("FormVideos/Button/CloseOptionsThumbs");
		mTrayMgr->destroyWidget("FormVideos/Button/DefaultThumbs");
	}
}

void FormVideos::showOptionsThumbs()
{
	if (!mTrayMgr->getWidget("FormVideos/Window/Thumbs")) // menu is hidden
	{
		unsigned int numOptions = 7;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 450;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		Ogre::StringVector itemsRows;
		for (unsigned int i = 1; i < 5; ++i) itemsRows.push_back(Ogre::StringConverter::toString(i));

		mTrayMgr->createDialogWindow("FormVideos/Window/Thumbs", "THUMBNAILS OPTIONS", left, top, width, height);										top += sepOptions / 2;
		ItemSelector* selectorRows = mTrayMgr->createItemSelector("FormVideos/Selector/Rows", "Number of rows", itemsRows, left, top, width);			top += sepOptions;
		SliderOptions* sliderTop = mTrayMgr->createSliderOptions("FormVideos/Slider/Top", "Thumbs Top", left, top, width, -10, 10, 100);					top += sepOptions;
		SliderOptions* sliderLeft = mTrayMgr->createSliderOptions("FormVideos/Slider/Left", "Thumbs Left", left, top, width, -25, 0, 100);				top += sepOptions;
		SliderOptions* sliderHorSep = mTrayMgr->createSliderOptions("FormVideos/Slider/SepHor", "Horizontal Separation", left, top, width, 1, 20, 100);	top += sepOptions;
		SliderOptions* sliderVerSep = mTrayMgr->createSliderOptions("FormVideos/Slider/SepVer", "Vertical Separation", left, top, width, 1, 20, 100);	top += sepOptions;
		SliderOptions* sliderSize = mTrayMgr->createSliderOptions("FormVideos/Slider/Size", "Thumbs size", left, top, width, 2, 5, 100);					top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormVideos/Button/CloseOptionsThumbs", "BACK", left, top, 60);
		mTrayMgr->createButton("FormVideos/Button/DefaultThumbs", "DEFAULT", left + 68, top, 85);

		selectorRows->selectOption(Ogre::StringConverter::toString(mGridThumbs.rows), false);
		sliderTop->setValue(-mGridThumbs.top, false);
		sliderLeft->setValue(mGridThumbs.left, false);
		sliderHorSep->setValue(mGridThumbs.horizontalSep, false);
		sliderVerSep->setValue(mGridThumbs.verticalSep, false);
		sliderSize->setValue(mGridThumbs.size, false);
	}
}

