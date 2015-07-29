#include "stdafx.h"
#include "FormOverview.h"

FormOverview::FormOverview(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener /*= 0*/) :
FormBase(tray, oldListener),
mGameInfo(gInfo),
mCameraMan(0),
mTarget(mSceneMgr->getRootSceneNode()->createChildSceneNode()),
mCurrentState(FO_SHORTDESC),
mCtrlPressed(false), mAltPressed(false), mShiftPressed(false)
{
	_setCameraDvdClose();
	_createOverview();
}

FormOverview::~FormOverview()
{
	if (mCameraMan) delete mCameraMan;
	if (mTarget) mSceneMgr->destroySceneNode(mTarget);
	delete mDvdClose;
	delete mDvdOpen;
	for (unsigned int i = 0; i < mDiscOpen.size(); ++i) delete mDiscOpen[i];
	delete mDiscClose;
	hideAllOptions();
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
	if (arg.key == OIS::KC_LCONTROL)	mCtrlPressed = true;
	else if (arg.key == OIS::KC_LMENU)	mAltPressed = true;
	else if (arg.key == OIS::KC_LSHIFT)	mShiftPressed = true;
	else if (arg.key == OIS::KC_C)
	{
		if (mCurrentState == FO_ZOOM) _setCameraDvdZoom();
		else if (mCurrentState == FO_OPENVIEW) _setCameraDvdOpen();
	}

	return FormBase::keyPressed(arg);
}

bool FormOverview::keyReleased(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_LCONTROL)	mCtrlPressed = false;
	else if (arg.key == OIS::KC_LMENU)	mAltPressed = false;
	else if (arg.key == OIS::KC_LSHIFT)	mShiftPressed = false;

	return FormBase::keyReleased(arg);
}



bool FormOverview::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;

	if (mLBPressed) // left button 
	{
		if (mCurrentState == FO_ZOOM || mCurrentState == FO_OPENVIEW)
		{
			if (mAltPressed) // left ALT...
			{
				if (mShiftPressed) // ...and left SHIFT are pressed? then move camera and mTarget
				{
					mDragging = true;
					Ogre::Vector3 newPos =
						(mCamera->getRight() * -arg.state.X.rel * 0.040f) +
						(mCamera->getUp() * arg.state.Y.rel *0.025f);
					mTarget->translate(newPos);
					mCamera->move(newPos);
				}
				else if (mCtrlPressed) // ...and left CTRL are pressed? then zoom
				{
					Ogre::Real dist = (mCamera->getPosition() - mDvdClose->getNode()->_getDerivedPosition()).length();
					mCamera->moveRelative(Ogre::Vector3(0, 0, arg.state.Y.rel * 0.0020f * dist));
				}
				else // ...is pressed (only left ALT)? then rotate camera around mTarget
				{
					if (mCameraMan)
					{
						mCameraMan->injectMouseMove(arg);
						if (mCamera->getPosition().y < 0)
							mCamera->moveRelative(Ogre::Vector3(0, -mCamera->getPosition().y, 0));
					}
				}
			}
			else if (mCtrlPressed) // only left CTRL is pressed?, yaw the dvdcase model
			{
				mDvdClose->getNode()->yaw(Ogre::Degree(arg.state.X.rel * 0.40f));
			}
		}
	}
	else if (mRBPressed)
	{

	}
	else if (mMBPressed)
	{

	}

	return FormBase::mouseMoved(arg);;
}

bool FormOverview::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	if (mCameraMan && mAltPressed)
		mCameraMan->injectMouseDown(arg, id);

	if (id == OIS::MouseButtonID::MB_Left)
	{
		// double clic? then go to the next mode of this form [description->zoom | zoom->openview]
		if (mTimeDoubleClick > 0 && mTimeDoubleClick < 0.15f)
		{
			mDoubleClick = true;
			if (mCurrentState == FO_SHORTDESC)
			{
				mTrayMgr->enableFadeEffect();
				mDvdClose->getNode()->setOrientation(Ogre::Quaternion(Ogre::Degree(0), Ogre::Vector3::UNIT_Y));
				FormBase::hide();
				mTrayMgr->getWidget("FormOverview/Logo")->show();
				_setCameraDvdZoom();
				mCurrentState = FO_ZOOM;
			}
			else if (mCurrentState == FO_ZOOM)
			{
				mTrayMgr->enableFadeEffect();
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
	}
	else if (id == OIS::MouseButtonID::MB_Middle)
	{
		// zoom in-out animation for dvd disc
		if (mCurrentState == FO_ZOOM)
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
	}

	return FormBase::mousePressed(arg, id);;
}

bool FormOverview::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;

	if (mCameraMan && mAltPressed)
		mCameraMan->injectMouseUp(arg, id);

	// return to the previous mode form [zoom->descripcion | openview->zoom]
	if (mRBPressed)
	{
		if (mCurrentState == FO_ZOOM)
		{
			mTrayMgr->enableFadeEffect();
			mDvdClose->getNode()->setPosition(-18, 0, -48);
			mDvdClose->getNode()->setOrientation(Ogre::Quaternion(Ogre::Degree(50), Ogre::Vector3::UNIT_Y));
			mDiscClose->getNode()->setPosition(0, 11, 0);
			mDiscClose->hide();
			FormBase::show();
			_setCameraDvdClose();
			mCurrentState = FO_SHORTDESC;
		}
		else if (mCurrentState == FO_OPENVIEW)
		{
			mTrayMgr->enableFadeEffect();
			mDvdClose->getNode()->setPosition(Ogre::Vector3(0, 0, -48));
			mDvdClose->getNode()->setOrientation(Ogre::Quaternion(Ogre::Degree(0), Ogre::Vector3::UNIT_Y));
			mDvdClose->show();
			mDvdOpen->hide();
			for (unsigned int i = 0; i < mDiscOpen.size(); ++i) 
				mDiscOpen[i]->hide();
			_setCameraDvdZoom();
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
	for (unsigned int i = 0; i < mDiscOpen.size(); ++i) 
		mDiscOpen[i]->hide();
	hideAllOptions();
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
		_setCameraDvdZoom();
		mDvdClose->show();
		FormBase::hide();
	}
	else // mCurrentState=FO_SHORTDESC
	{
		_setCameraDvdClose();
		mDvdClose->show();
	}
}



void FormOverview::buttonHit(Button* button)
{
	if (button->getName() == "FormOverview/Button/CloseOptions")
	{
		hideOptions();
	}
}

void FormOverview::labelHit(Label* label)
{
	if (label->getName() == "FormOverview/Label/Help")
	{
		hideOptions();
		Ogre::String messageOverview = 
			"The overview view has three subviews: the overview view, the zoom view, and the open view. "
			"Passing through the different views is as simple as double clicking anywhere on the screen. "
			"To return to the previous view click with the right mouse button.\n\n"
			"In zoom and open view, you can move the camera around. Here are the controls to move the camera:\n"
			"[Left ALT] + [Left Mouse Button] = move around target.\n"
			"[Left CTRL] + [Left ALT] + [Left Mouse Button] = zoom.\n"
			"[Left SHIFT] + [Left ALT] + [Left Mouse Button] = move target.";
		mTrayMgr->showOkDialog("OVERVIEW TUTORIAL", messageOverview);
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

void FormOverview::_setCameraDvdClose()
{
	if (mCameraMan) delete mCameraMan;
	mCameraMan = 0;
	resetCamera();
	mNodeCamera->setPosition(0, 14, 0);
}

void FormOverview::_setCameraDvdZoom()
{
	mNodeCamera->setPosition(0, 0, 0);
	mTarget->setPosition(mDvdClose->getPosition());
	mTarget->translate(0, 14, 0);
	if (!mCameraMan)
		mCameraMan = new OgreBites::SdkCameraMan(mCamera);
	mCameraMan->setStyle(OgreBites::CS_ORBIT);
	mCameraMan->setTarget(mTarget);
	mCameraMan->setYawPitchDist(Ogre::Degree(0), Ogre::Degree(0), 46);
}

void FormOverview::_setCameraDvdOpen()
{
	mNodeCamera->setPosition(0, 0, 0);
	mTarget->setPosition(mDvdOpen->getPosition());
	mTarget->translate(0, 9, 0);
	if (!mCameraMan) 
		mCameraMan = new OgreBites::SdkCameraMan(mCamera);
	mCameraMan->setStyle(OgreBites::CS_ORBIT);
	mCameraMan->setTarget(mTarget);
	mCameraMan->setYawPitchDist(Ogre::Degree(0), Ogre::Degree(30), 58);
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


void FormOverview::hideAllOptions()
{
	hideOptions();
}

void FormOverview::hideOptions()
{
	if (mTrayMgr->getWidget("FormOverview/Window/Options"))
	{
		mTrayMgr->destroyDialogWindow("FormOverview/Window/Options");
		mTrayMgr->destroyWidget("FormOverview/Label/Help");
		mTrayMgr->destroyWidget("FormOverview/Button/CloseOptions");
	}
}

void FormOverview::showOptions()
{
	if (!mTrayMgr->getWidget("FormOverview/Window/Options")) // menu is hidden
	{
		unsigned int numOptions = 2;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 250;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;

		mTrayMgr->createDialogWindow("FormOverview/Window/Options", "OPTIONS", left, top, width, height); top += sepOptions / 2;
		mTrayMgr->createLabel("FormOverview/Label/Help", "HELP", left, top, width, 23); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormOverview/Button/CloseOptions", "BACK", left, top, 60);
	}
}

