#include "stdafx.h"
#include "FormVideos.h"

FormVideos::FormVideos(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener /*= 0*/) :
FormBase(tray, oldListener),
mGameInfo(gInfo),
mLastThumbOver(0),
mParentThumbs(mSceneMgr->getRootSceneNode()->createChildSceneNode()),
mVideoPlayer(0),
mTimeOver(0.0f),
mCurrentIndex(0)
{
	// find all videos of the game [trailers + gameplays]
	mGameInfo->findTrailersResources(mVideos);
	mIndexTrailer = std::make_pair(0, mVideos.size());
	mGameInfo->findGameplaysResources(mVideos);
	mIndexGameplay = std::make_pair(mIndexTrailer.second, mVideos.size());

	// load thumbnails...
	for (unsigned int i = 0; i < mVideos.size(); ++i)
	{
		if (!boost::filesystem::is_regular_file(mVideos[i].pathThumb))
			_createThumbFromVideo(mVideos[i].path, mVideos[i].pathThumb, 256);
		GameInfo::loadImageFromDisk(mVideos[i].pathThumb, mVideos[i].nameThumb, mGameInfo->getGroupName());
		mThumbs.push_back(new Thumbnail3D(mParentThumbs, "FormVideos/Thumb/" + Ogre::StringConverter::toString(i),
			mGameInfo->getGroupName(), mVideos[i].caption, mVideos[i].nameThumb));
		mThumbs.back()->setIndex(i);
		//mThumbs.back()->scale(Ogre::Vector3(1.75f, 1.75f, 1.75f));
	}

	Thumbnail3D::setThumbs3DInGrid(mThumbs, 3, 10.4f, 6.25f, 3);

	// slider [only exists if more than 10 items]
	if (mThumbs.size() > 9)
	{
		Ogre::Real minValue = 0;
		Ogre::Real maxValue = mThumbs.back()->getPosition().x;
		Slider* sd = mTrayMgr->createSlider("FormVideos/Slider", 370, minValue, maxValue, maxValue + 100);
		sd->getOverlayElement()->setVerticalAlignment(Ogre::GVA_BOTTOM);
		sd->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
		sd->setLeft(-(sd->getWidth() / 2));
		sd->setTop(-50);
		addWidgetToForm(sd);
	}

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
}



bool FormVideos::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (mVideoPlayer)
	{
		mVideoPlayer->update();
		mTrayMgr->setMediaPlayerSliderValue(mVideoPlayer->getCurrentTime(), mVideoPlayer->getDuration(), false);
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
		Ogre::Real durationSound = mVideoPlayer->getDuration();
		Ogre::Real currentTime = mVideoPlayer->getCurrentTime();
		mVideoPlayer->seek(mVideoPlayer->getCurrentTime() + 10);
	}
	else if (arg.key == OIS::KC_SPACE)
	{
		if (mVideoPlayer->isPaused())
			mVideoPlayer->play();
		else
			mVideoPlayer->pause();
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

	if (mVideoPlayer && arg.state.Y.abs >= (mScreenSize.y - 55))
		mTrayMgr->showMediaPlayer(this);
	else
		mTrayMgr->hideMediaPlayer();

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
		if (mVideoPlayer) {mVideoPlayer->close(); delete mVideoPlayer; }
		mVideoPlayer = 0;
		mLastThumbOver->setMouseUp(); mLastThumbOver = 0; mTimeOver = 0.0f;
	}
	
	return FormBase::mouseMoved(arg);;
}

bool FormVideos::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	return FormBase::mousePressed(arg, id);;
}

bool FormVideos::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;

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
			mTrayMgr->hideMediaPlayer();
			mTrayMgr->hideBackdrop();
			mTrayMgr->showMenuBar();
			mTrayMgr->showWidgets();
			show();
		}
	}

	return FormBase::mouseReleased(arg, id);;
}



void FormVideos::hide()
{
	FormBase::hide();
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		mThumbs[i]->hide();
}

void FormVideos::show()
{
	mCamera->setPosition(0, 0, 0);
	mCamera->setOrientation(Ogre::Quaternion(1, 0, 0, 0));
	mNodeCamera->setPosition(0, 0, 0);
	mNodeCamera->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

	FormBase::show();
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		mThumbs[i]->show();
}



void FormVideos::sliderMoved(Slider* slider)
{
	if (slider->getName() == "FormVideos/Slider")
	{
		mParentThumbs->setPosition(-slider->getValue(), 0, 0);
	}
	else
	{
		mVideoPlayer->seek(slider->getValue());
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

void FormVideos::medialPlayerHit(MediaPlayer* player)
{
	if (player->getCurrentAction() == "Play")
	{
		mVideoPlayer->play();
	}
	else if (player->getCurrentAction() == "Pause")
	{
		mVideoPlayer->pause();
	}
	else if (player->getCurrentAction() == "Stop")
	{
		mVideoPlayer->close();
		delete mVideoPlayer;
		mVideoPlayer = 0;
		mTrayMgr->hideMediaPlayer();
		mTrayMgr->hideBackdrop();
		mTrayMgr->showMenuBar();
		mTrayMgr->showWidgets();
		show();
	}
	else if (player->getCurrentAction() == "Previous")
	{
		mCurrentIndex = (mCurrentIndex == 0) ? mVideos.size() - 1 : mCurrentIndex - 1;
		playVideo(mCurrentIndex);
	}
	else if (player->getCurrentAction() == "Next")
	{
		mCurrentIndex = (mCurrentIndex == mVideos.size() - 1) ? 0 : mCurrentIndex + 1;
		playVideo(mCurrentIndex);
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
		mTrayMgr->setMediaPlayerRange(0, mVideoPlayer->getDuration(), mVideoPlayer->getDuration() + 1);

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
	if (boost::filesystem::is_regular_file("C:/Librerias/ogre_sdk_vs120/bin/debug/ffmpeg.exe"))
	{
		//...ffmpeg.exe -ss 15 -i video.mp4 -vf scale=512:-1 -vframes 1 OutputFile.jpg
		Ogre::String sizeThumb = "scale=-1:" + Ogre::StringConverter::toString(thumbHeight);
		Ogre::String processToStart("C:/Librerias/ogre_sdk_vs120/bin/debug/ffmpeg.exe");
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
		Thumbnail3D::setThumbs3DInGrid(mThumbs, 3, 10.4f, 6.25f, 3);

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

void FormVideos::showOptions()
{

}

