#include "stdafx.h"
#include "FormMusic.h"

FormMusic::FormMusic(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener /*= 0*/) : 
FormBase(tray, oldListener),
mGameInfo(gInfo),
mDiscClose(0),
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

	/*
	MediaPlayerMini* mpm = mTrayMgr->getDefaultMiniPlayer();
	mpm->show();
	mpm->setLeft(-((mScreenSize.x / 2.5f) + padLeft));
	mpm->setTop(mScreenSize.y / 3.35f);
	mpm->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_RIGHT);
	
	tl->getOverlayElement()->setHorizontalAlignment(mpm->getOverlayElement()->getHorizontalAlignment());
	tl->setTop(mpm->getTop() + mpm->getHeight() + 10);
	tl->setLeft(mpm->getLeft());
	tl->show();
	*/

	// load music from path
	std::vector<sInfoResource> mInfoTracks;
	mGameInfo->findSoundtrackResources(mInfoTracks);
	Ogre::StringVector tracks;
	for (unsigned int i = 0; i < mInfoTracks.size(); ++i)
		tracks.push_back(mInfoTracks[i].path);	
	TrackList* tl = mTrayMgr->getDefaultTrackList();
	tl->setTracks(tracks);
}

FormMusic::~FormMusic()
{
	if (mDiscClose) delete mDiscClose;
	mTrayMgr->hideMiniPlayer();
}



bool FormMusic::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
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

