#include "stdafx.h"
#include "FormMusic.h"

FormMusic::FormMusic(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener /*= 0*/) : 
FormBase(tray, oldListener),
mGameInfo(gInfo),
mDiscClose(0),
mTrackList(0),
mAudioPlayer(0),
mCurrentIndex(0)
{
	// set camera to view cd mesh properly
	mCamera->setAutoTracking(false);
	mCamera->setPosition(0, 0, 0);
	mCamera->setOrientation(mNodeCamera->getOrientation());
	mNodeCamera->setPosition(0, 9, 0);

	mDiscClose = new CModel("mesh_discDvd.mesh");
	mDiscClose->getNode()->setPosition(-12, 7.6, -35);
	mDiscClose->getNode()->pitch(Ogre::Degree(90));
	mDiscClose->getNode()->roll(Ogre::Degree(-38));
	std::vector<sInfoResource> infoDisc;
	mGameInfo->findDiscResource(infoDisc);
	if (!infoDisc.empty())
	{
		if (!Ogre::TextureManager::getSingleton().resourceExists(infoDisc.back().nameThumb))
			GameInfo::loadImageFromDisk(infoDisc.back().pathThumb, infoDisc.back().nameThumb, mGameInfo->getGroupName(), 2);
		mDiscClose->applyNewText(infoDisc.back().nameThumb);
	}

	Ogre::Real padLeft = mScreenSize.x / 40.0f;
	Ogre::Real padTop = mScreenSize.y / 36.0f;
	Ogre::Real padRight = mScreenSize.x / 25.0f;
	// title
	Ogre::Real titleWidth = (mScreenSize.x / 2.0f) + padLeft;
	Ogre::Real titleCaptionSize = (mScreenSize.x > 1400) ? 44 : 32;
	Ogre::Real titleHeight = titleCaptionSize + padLeft;
	Ogre::String tittle = "TRACK LIST";
	Ogre::StringUtil::toUpperCase(tittle);
	SimpleText* stTitle = mTrayMgr->createSimpleText("FormMusic/Title", tittle, "YgcFont/SemiBold/16", titleWidth, titleHeight, titleCaptionSize, 1);
	stTitle->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_RIGHT);
	stTitle->setTop(mScreenSize.y / 3.2f);
	stTitle->setLeft(-titleWidth);
	addWidgetToForm(stTitle);

	// decoration bar
	DecorWidget* decorBar = mTrayMgr->createDecorWidget("FormMusic/DecorBar", "YgcGui/DecorBar");
	decorBar->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_RIGHT);
	decorBar->getOverlayElement()->setWidth((stTitle->getOverlayElement()->getWidth() - padRight) + 10);
	decorBar->setTop(stTitle->getTop() + titleCaptionSize / 1.2f);
	decorBar->setLeft(stTitle->getLeft() - 5);
	addWidgetToForm(decorBar);

	// load music from path
	mGameInfo->findSoundtrackResources(mInfoTracks);
	if (!mInfoTracks.empty())
	{
		Ogre::StringVector tracks;
		for (unsigned int i = 0; i < mInfoTracks.size(); ++i)
			tracks.push_back(mInfoTracks[i].filename);
		mTrackList = mTrayMgr->createTrackList("FormMusic/TrackList", tracks, mScreenSize.x - titleWidth + 8, decorBar->getTop() + 30);
		addWidgetToForm(mTrackList);
	}
}

FormMusic::~FormMusic()
{
	if (mDiscClose) delete mDiscClose;
	if (mAudioPlayer) delete mAudioPlayer;
}



bool FormMusic::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (mAudioPlayer && mAudioPlayer->isReady())
	{
		mTrayMgr->setMediaPlayerSliderValue(mAudioPlayer->getActualTime(), mAudioPlayer->getDuration(), false);
	}

	return FormBase::frameRenderingQueued(evt);
}



bool FormMusic::keyPressed(const OIS::KeyEvent &arg)
{
	return FormBase::keyPressed(arg);
}

bool FormMusic::keyReleased(const OIS::KeyEvent &arg)
{
	return FormBase::keyReleased(arg);
}

bool FormMusic::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;

	if (mAudioPlayer && arg.state.Y.abs >= (mScreenSize.y - 55)) 
		mTrayMgr->showMediaPlayer(this);
	else 
		mTrayMgr->hideMediaPlayer();

	return FormBase::mouseMoved(arg);;
}

bool FormMusic::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	return FormBase::mousePressed(arg, id);;
}

bool FormMusic::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;

	return FormBase::mouseReleased(arg, id);;
}



void FormMusic::trackListHit(TrackList* track)
{
	playTrack(mCurrentIndex);
}

void FormMusic::mediaPlayerHit(MediaPlayer* player)
{
	if (player->getCurrentAction() == "Play")
	{
		mAudioPlayer->Play();
	}
	else if (player->getCurrentAction() == "Pause")
	{
		mAudioPlayer->Pause();
	}
	else if (player->getCurrentAction() == "Stop")
	{
		mAudioPlayer->Stop();
		mTrayMgr->hideMediaPlayer();
		delete mAudioPlayer;
		mAudioPlayer = 0;
	}
	else if (player->getCurrentAction() == "Previous")
	{
		mCurrentIndex = (mCurrentIndex == 0) ? mInfoTracks.size() - 1 : mCurrentIndex - 1;
		playTrack(mCurrentIndex);
	}
	else if (player->getCurrentAction() == "Next")
	{
		mCurrentIndex = (mCurrentIndex == mInfoTracks.size() - 1) ? 0 : mCurrentIndex + 1;
		playTrack(mCurrentIndex);
	}
}

void FormMusic::sliderMoved(Slider* slider)
{
	if (mAudioPlayer && mAudioPlayer->isPlaying())
	{
		mAudioPlayer->forwardTime(slider->getValue());
	}
}



void FormMusic::playTrack(unsigned int index)
{
	if (mAudioPlayer) delete mAudioPlayer;
	mAudioPlayer = 0;

	if (index >= 0 && index < mInfoTracks.size())
	{
		mAudioPlayer = new DirectShowSound(const_cast<char *>(mInfoTracks[index].path.c_str()));
		mAudioPlayer->Play();
		mAudioPlayer->setVolume(5000);

		mTrayMgr->setMediaPlayerRange(0, mAudioPlayer->getDuration(), mAudioPlayer->getDuration() + 1);
	}
}


void FormMusic::hide()
{
	FormBase::hide();
	mDiscClose->hide();
	Widget* logoGame = mTrayMgr->getWidget("FormOverview/Logo");
	if (logoGame) logoGame->hide();
}

void FormMusic::show()
{
	FormBase::show();
	mDiscClose->show();

	mCamera->setAutoTracking(false);
	mCamera->setPosition(0, 0, 0);
	mCamera->setOrientation(mNodeCamera->getOrientation());
	mNodeCamera->setPosition(0, 9, 0);
	Widget* logoGame = mTrayMgr->getWidget("FormOverview/Logo");
	if (logoGame) logoGame->show();
}

void FormMusic::showOptions()
{

}

