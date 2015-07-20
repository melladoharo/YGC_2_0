#include "stdafx.h"
#include "FormOverview.h"

FormOverview::FormOverview(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener /*= 0*/) :
FormBase(tray, oldListener),
mGameInfo(gInfo),
mCameraMan(0),
mCurrentState(FO_SHORTDESC),
mLCTRLpressed(false),
mLALTpressed(false)
{
	_setCameraDvdClose();
	_createOverview();
}

FormOverview::~FormOverview()
{
	if (mCameraMan) delete mCameraMan;
	delete mDvdClose;
	delete mDvdOpen;
	for (unsigned int i = 0; i < mDiscOpen.size(); ++i) delete mDiscOpen[i];
	delete mDiscClose;
}




bool FormOverview::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	// hide the disc when the animation ends
	if (mSceneMgr->hasAnimation("Anim/Disc/In") && mSceneMgr->getAnimationState("Anim/Disc/In")->hasEnded())
		mDiscClose->hide();

	if (mCameraMan && mCurrentState == FO_OPENVIEW)
		mCameraMan->frameRenderingQueued(evt);

	return FormBase::frameRenderingQueued(evt);
}

bool FormOverview::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_LCONTROL)
		mLCTRLpressed = true;
	else if (arg.key == OIS::KC_LMENU)
		mLALTpressed = true;

	return FormBase::keyPressed(arg);
}

bool FormOverview::keyReleased(const OIS::KeyEvent &arg)
{
	mLCTRLpressed = mLALTpressed = false;

	return FormBase::keyReleased(arg);
}

bool FormOverview::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;
	
	if (mCameraMan && mCurrentState == FO_OPENVIEW)
		mCameraMan->injectMouseMove(arg);

	if (mLBPressed)
	{
		if (mLCTRLpressed && mLALTpressed)
		{
			mDragging = true;

			if (mCurrentState == FO_ZOOM)
			{
				Ogre::Real dist = (mCamera->getPosition() - mDvdClose->getNode()->_getDerivedPosition()).length();
				mCamera->moveRelative(Ogre::Vector3(0, arg.state.Y.rel * 0.00080f * dist, 0));
			}
		}
		else  if (mCurrentState == FO_ZOOM)
		{
			mDvdClose->getNode()->yaw(Ogre::Degree(arg.state.X.rel * 0.40f));
		}
	}
	else if (mRBPressed)
	{
		mDragging = true;

		if (mCurrentState == FO_ZOOM)
		{
			Ogre::Real dist = (mCamera->getPosition() - mDvdClose->getNode()->_getDerivedPosition()).length();
			mCamera->moveRelative(Ogre::Vector3(0, 0, arg.state.Y.rel * 0.0025f * dist));
		}
	}
	else if (mMBPressed)
	{

	}

	return FormBase::mouseMoved(arg);;
}

bool FormOverview::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	if (mCameraMan && mCurrentState == FO_OPENVIEW)
		mCameraMan->injectMouseDown(arg, id);

	if (id == OIS::MouseButtonID::MB_Left)
	{
		mLBPressed = true;
		if (mTimeDoubleClick > 0 && mTimeDoubleClick < 0.15f)
		{
			mDoubleClick = true;
			if (mCurrentState == FO_SHORTDESC)
			{
				createAnimNode("Anim/Zoom", 0.45f, mDvdClose->getNode(),
					mDvdClose->getNode()->getPosition(), mDvdClose->getNode()->getOrientation(),
					Ogre::Vector3(0, 0, -48), Ogre::Quaternion(Ogre::Degree(0), Ogre::Vector3::UNIT_Y));
				FormBase::hide();
				mTrayMgr->getWidget("FormOverview/Logo")->show();
				mCurrentState = FO_ZOOM;
			}
			else if (mCurrentState == FO_ZOOM)
			{
				mDvdClose->hide();
				mDvdOpen->show();
				for (unsigned int i = 0; i < mDiscOpen.size(); ++i) mDiscOpen[i]->show();
				_setCameraDvdOpen();
				mCurrentState = FO_OPENVIEW;
			}
		}
	}
	else if (id == OIS::MouseButtonID::MB_Right)
	{
		mRBPressed = true;
	}
	else if (id == OIS::MouseButtonID::MB_Middle)
	{
		mMBPressed = true;
	}

	return FormBase::mousePressed(arg, id);;
}

bool FormOverview::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;

	if (mCameraMan && mCurrentState == FO_OPENVIEW)
		mCameraMan->injectMouseUp(arg, id);

	if (mMBPressed && !mDragging && mCurrentState == FO_ZOOM)
	{
		if (mDiscClose->isVisible())
		{
			if (!mSceneMgr->hasAnimation("Anim/Disc/Out"))
				createAnimNode("Anim/Disc/In", 0.25f, mDiscClose->getNode(),
				mDiscClose->getPosition(), mDiscClose->getOrientation(),
				mDiscClose->getPosition() + Ogre::Vector3(-10, 0, 0), mDiscClose->getOrientation());
			//mDvdClosedDisc->setVisible(false); [see frameRenderingQueued]
		}
		else
		{
			if (!mSceneMgr->hasAnimation("Anim/Disc/In"))
				createAnimNode("Anim/Disc/Out", 0.25f, mDiscClose->getNode(),
				mDiscClose->getPosition(), mDiscClose->getOrientation(),
				mDiscClose->getPosition() + Ogre::Vector3(10, 0, 0), mDiscClose->getOrientation());
			mDiscClose->setVisible(true);
		}
	}
	else if (mRBPressed && !mDragging)
	{
		if (mCurrentState == FO_ZOOM)
		{
			mDvdClose->getNode()->setPosition(-18, 0, -48);
			mDvdClose->getNode()->setOrientation(Ogre::Quaternion(Ogre::Degree(50), Ogre::Vector3::UNIT_Y));
			mDiscClose->getNode()->setPosition(0, 11, 0);
			mDiscClose->hide();
			_setCameraDvdClose();
			FormBase::show();
			mCurrentState = FO_SHORTDESC;
		}
		else if (mCurrentState == FO_OPENVIEW)
		{
			mDvdClose->getNode()->setPosition(Ogre::Vector3(0, 0, -48));
			mDvdClose->getNode()->setOrientation(Ogre::Quaternion(Ogre::Degree(0), Ogre::Vector3::UNIT_Y));
			mDvdClose->show();
			mDvdOpen->hide();
			for (unsigned int i = 0; i < mDiscOpen.size(); ++i) mDiscOpen[i]->hide();
			_setCameraDvdClose();
			mCurrentState = FO_ZOOM;
		}
	}

	return FormBase::mouseReleased(arg, id);;
}

void FormOverview::hide()
{
	FormBase::hide();
	mDvdClose->hide();
	mDvdOpen->hide();
	for (unsigned int i = 0; i < mDiscOpen.size(); ++i) mDiscOpen[i]->hide();
	_setCameraDvdClose();
}

void FormOverview::show()
{
	FormBase::show();

	if (mCurrentState == FO_OPENVIEW)
	{
		_setCameraDvdOpen();
		mDvdOpen->show();
		for (unsigned int i = 0; i < mDiscOpen.size(); ++i) mDiscOpen[i]->show();
		FormBase::hide();
	}
	else if (mCurrentState == FO_ZOOM)
	{
		_setCameraDvdClose();
		mDvdClose->show();
		FormBase::hide();
	}
	else // mCurrentState=FO_SHORTDESC
	{
		_setCameraDvdClose();
		mDvdClose->show();
	}
}

void FormOverview::_createOverview()
{
	// find and load cover
	sInfoResource infoCover;
	mGameInfo->findCoverResource(infoCover);
	if (!boost::filesystem::is_regular_file(infoCover.pathThumb))
		GameInfo::createThumbnail(infoCover.path, infoCover.pathThumb, 2048);
	GameInfo::loadImageFromDisk(infoCover.pathThumb, infoCover.nameThumb, mGameInfo->getGroupName(), 2);
	// Closed DVD model
	mDvdClose = new CModel("Dvd_Case.mesh");
	mDvdClose->getNode()->translate(-18, 0, -48);
	mDvdClose->getNode()->yaw(Ogre::Degree(50));
	mDvdClose->applyNewText(infoCover.nameThumb);

	// find and load discs
	std::vector<sInfoResource> infoDisc;
	mGameInfo->findDiscResource(infoDisc);
	for (unsigned int i = 0; i < infoDisc.size(); ++i)
	{
		if (!boost::filesystem::is_regular_file(infoDisc[i].pathThumb))
			GameInfo::createThumbnail(infoDisc[i].path, infoDisc[i].pathThumb, 1024);
		GameInfo::loadImageFromDisk(infoDisc[i].pathThumb, infoDisc[i].nameThumb, mGameInfo->getGroupName(), 2);
	}
	// Closed DVD's disc [close dvd view]
	mDiscClose = new CModel("mesh_CD.mesh", mDvdClose->getNode());
	mDiscClose->getNode()->translate(0, 11, 0);
	mDiscClose->getNode()->pitch(Ogre::Degree(90));
	mDiscClose->hide();
	mDiscClose->applyNewText(infoDisc.front().nameThumb);

	// find and load boxart resource
	sInfoResource infoBoxArt;
	mGameInfo->findBoxArtResource(infoBoxArt);
	if (!boost::filesystem::is_regular_file(infoBoxArt.pathThumb))
		GameInfo::createThumbnail(infoBoxArt.path, infoBoxArt.pathThumb, 1024);
	GameInfo::loadImageFromDisk(infoBoxArt.pathThumb, infoBoxArt.nameThumb, mGameInfo->getGroupName(), 2);

	// Open DVD model
	mDvdOpen = new CModel("mesh_openDvd.mesh");
	mDvdOpen->getNode()->translate(0, 0, -38);
	mDvdOpen->getNode()->yaw(Ogre::Degree(150));
	mDvdOpen->applyNewText(infoCover.nameThumb); // cover
	mDvdOpen->applyNewText(infoBoxArt.nameThumb, 3); // boxart
	mDvdOpen->applyNewText(infoDisc.front().nameThumb, 4); // disc
	mDvdOpen->hide();
	
	// all disc [open dvd view]
	for (unsigned int i = 0; i < infoDisc.size(); ++i)
	{
		mDiscOpen.push_back(new CModel("mesh_CD.mesh"));
		mDiscOpen.back()->hide();
		mDiscOpen.back()->applyNewText(infoDisc[i].nameThumb);
	}
	_setPositionDisc();

	// Options
	Ogre::Real padLeft = mScreenSize.x / 18.0f;
	Ogre::Real padTop = mScreenSize.y / 36.0f;
	Ogre::Real padRight = mScreenSize.x / 25.0f;

	// find and load logo
	sInfoResource infoLogo;
	mGameInfo->findLogoResource(infoLogo);
	if (!boost::filesystem::is_regular_file(infoLogo.pathThumb))
		GameInfo::createThumbnail(infoLogo.path, infoLogo.pathThumb, 512);
	GameInfo::loadImageFromDisk(infoLogo.pathThumb, infoLogo.nameThumb, mGameInfo->getGroupName(), 2);

	// logo rigth bottom corner
	DecorWidget* logo = mTrayMgr->createDecorWidget("FormOverview/Logo", "YgcGui/Logo");
	Ogre::MaterialPtr matLogo = Ogre::MaterialManager::getSingleton().create("Mat/FormOverview/Logo/", mGameInfo->getGroupName());
	matLogo->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
	matLogo->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	matLogo->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	matLogo->getTechnique(0)->getPass(0)->createTextureUnitState(infoLogo.nameThumb);
	logo->getOverlayElement()->setMaterialName(matLogo->getName());
	logo->_autoScale(120, matLogo);
	logo->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_RIGHT);
	logo->getOverlayElement()->setVerticalAlignment(Ogre::GVA_BOTTOM);
	logo->setTop(-(logo->getHeight() + padRight / 1.0f));
	logo->setLeft(-(logo->getWidth() + padRight / 1.0f));

	// title
	Ogre::Real titleWidth = (mScreenSize.x / 2.0f) + padLeft;
	Ogre::Real titleCaptionSize = (mScreenSize.x > 1400) ? 44 : 32;
	Ogre::Real titleHeight = titleCaptionSize + padLeft;
	Ogre::String tittle = mGameInfo->getValueOption("GAME.INFO", "Name", "Title");
	Ogre::StringUtil::toUpperCase(tittle);
	SimpleText* stTitle = mTrayMgr->createSimpleText("FormOverview/Title", tittle, "YgcFont/SemiBold/16", titleWidth, titleHeight, titleCaptionSize, 1);
	stTitle->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_RIGHT);
	stTitle->setTop(mScreenSize.y / 3.2f);
	stTitle->setLeft(-titleWidth);

	// decoration bar
	DecorWidget* decorBar = mTrayMgr->createDecorWidget("FormOverview/DecorBar", "YgcGui/DecorBar");
	decorBar->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_RIGHT);
	decorBar->getOverlayElement()->setWidth((stTitle->getOverlayElement()->getWidth() - padRight ) + 10);
	decorBar->setTop(stTitle->getTop() + titleCaptionSize / 1.2f);
	decorBar->setLeft(stTitle->getLeft() - 5);

	// info game header
	Ogre::Real headerWidth = 150;
	Ogre::Real headerCaptionSize = (mScreenSize.x > 1400) ? 22 : 16;
	Ogre::Real headerHeight = (5 * headerCaptionSize) + padTop;
	Ogre::String header = "Developer:\nPublisher:\nRelease Date:\nGenre:\nESRB:";
	SimpleText* stHeader = mTrayMgr->createSimpleText("FormOverview/Header", header, "YgcFont/Italic/16", headerWidth, headerHeight, headerCaptionSize, 6);
	stHeader->setTextColor(Ogre::ColourValue(0.78f, 0.78f, 0.78f));
	stHeader->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_RIGHT);
	stHeader->setTop(decorBar->getTop() + decorBar->getHeight() + padTop);
	stHeader->setLeft(stTitle->getLeft() + 12);

	// info game values
	Ogre::String values = Ogre::String(mGameInfo->getValueOption("GAME.INFO", "Developer", "???")) + "\n" +
		Ogre::String(mGameInfo->getValueOption("GAME.INFO", "Publisher", "???")) + "\n" +
		Ogre::String(mGameInfo->getValueOption("GAME.INFO", "ReleaseDate", "???")) + "\n" +
		Ogre::String(mGameInfo->getValueOption("GAME.INFO", "Genre", "???")) + "\n" +
		Ogre::String(mGameInfo->getValueOption("GAME.INFO", "Rating", "???")) + "\n";
	SimpleText* stHeaderValues = mTrayMgr->createSimpleText("FormOverview/Header/Values", values, "YgcFont/SemiboldItalic/16", headerWidth*2.0f, headerHeight, headerCaptionSize, 6);
	stHeaderValues->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_RIGHT);
	stHeaderValues->setTop(stHeader->getTop());
	stHeaderValues->setLeft(stHeader->getLeft() + stHeader->getWidth());

	// Overview
	Ogre::Real overviewCaptionSize = (mScreenSize.x > 1400) ? 26 : 20;
	SimpleText* stOverview = mTrayMgr->createSimpleText("FormOverview/Overview", mGameInfo->getValueOption("GAME.INFO", "Overview", "???"),
		"YgcFont/SemiBold/16", titleWidth - padRight, 350, overviewCaptionSize, 8);
	stOverview->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_RIGHT);
	stOverview->setTop(stHeader->getTop() + stHeader->getHeight() + padTop * 1.5f);
	stOverview->setLeft(stTitle->getLeft());

	addWidgetToForm(stTitle);
	addWidgetToForm(decorBar);
	addWidgetToForm(logo);
	addWidgetToForm(stHeader);
	addWidgetToForm(stHeaderValues);
	addWidgetToForm(stOverview);
}

void FormOverview::_setCameraDvdOpen()
{
	mNodeCamera->setPosition(0, 0, 0);

	mCameraMan = new OgreBites::SdkCameraMan(mCamera);
	mCameraMan->setStyle(OgreBites::CS_ORBIT);

	Ogre::SceneNode* nodoObjetivo = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	nodoObjetivo->setPosition(0, 0, -38);
	nodoObjetivo->translate(0, 10, 3);
	mCameraMan->setTarget(nodoObjetivo);
	mCameraMan->setYawPitchDist(Ogre::Degree(0), Ogre::Degree(30), 54);
}

void FormOverview::_setCameraDvdClose()
{
	if (mCameraMan) delete mCameraMan;
	mCameraMan = 0;
	mCamera->setAutoTracking(false);
	mCamera->setPosition(0, 0, 0);
	mCamera->setOrientation(mNodeCamera->getOrientation());
	mNodeCamera->setPosition(0, 14, 0);
}

void FormOverview::_setPositionDisc()
{
	Ogre::Vector3 posCD = Ogre::Vector3(0.0f, 0.05f, -27.0f);
	Ogre::Real incX = 16.0f, incZ = 0.0f;

	switch (mDiscOpen.size())
	{
	case(1) :
		posCD = Ogre::Vector3(0.0f, 0.05f, -27.0f);
		break;
	case (2) :
		posCD = Ogre::Vector3(-8.0f, 0.05f, -29.0f);
		incX = 16.0f;
		break;
	case(3) :
		posCD = Ogre::Vector3(-16.0f, 0.05f, -33.0f);
		incX = 16.0f;
		incZ = 6;
		break;
	case (4) :
		posCD = Ogre::Vector3(-24.0f, 0.05f, -31.5f);
		incX = 16.0f;
		incZ = 4;
		break;
	default:
		break;
	}
	
	int centerDisc = (mDiscOpen.size() % 2 == 0) ? (mDiscOpen.size() / 2) - 1 : mDiscOpen.size() / 2;
	for (unsigned int i = 0; i < mDiscOpen.size(); ++i)
	{
		mDiscOpen[i]->getNode()->setPosition(posCD);
		if (i == centerDisc)
			incZ = -incZ;
		if (mDiscOpen.size() % 2 == 0 && i == centerDisc)
		{
			posCD.x += incX;
		}
		else
		{
			posCD.x += incX;
			posCD.z += incZ;
		}
	}
}

void FormOverview::showOptions()
{

}

