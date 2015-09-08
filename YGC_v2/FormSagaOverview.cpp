#include "stdafx.h"
#include "FormSagaOverview.h"
#include "ConfigReader.h"

FormSagaOverview::FormSagaOverview(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener) :
FormBase(tray, oldListener),
mGameInfo(gInfo),
mCameraMan(0),
mTarget(mSceneMgr->getRootSceneNode()->createChildSceneNode()),
mCtrlPressed(false), mAltPressed(false), mShiftPressed(false)
{
	// initialize resource group 'FormSagas'
	Ogre::ResourceGroupManager::getSingleton().createResourceGroup("Group/SagaOverview");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Group/SagaOverview");

	// load a ramdon wallpaper (if the saga has one)
	mTrayMgr->hideMenuBar();
	mTrayMgr->hideCursor();
	_showRandomWallpaper();
	mTrayMgr->showProgressBar();
	mTrayMgr->setProgressBarCaption("Loading saga...");

	// find the .ini config file
	sInfoResource infoIni;
	mGameInfo->findIniSaga(infoIni);

	// create the saga
	Ogre::StringVector gameName = _getNameGames(infoIni.path);
	Ogre::String pathGames = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Games", "./Games");
	for (unsigned int i = 0; i < gameName.size(); i++)
	{
		if (boost::filesystem::is_directory(pathGames + "/" + gameName[i]))
		{
			mTrayMgr->setProgressBarProgress((i + 1.0f) / gameName.size());
			mTrayMgr->setProgressBarCaption("Loading " + gameName[i] + " ...");
			_addDvdModel(pathGames + "/" + gameName[i]);
			_addDiscModel(pathGames + "/" + gameName[i]);
		}
	}

	_setPositionModels();
	_setCameraSaga();
	mTrayMgr->hideProgressBar();
	mTrayMgr->enableFadeEffect();
	mTrayMgr->showMenuBar();
	mTrayMgr->showCursor();
}

FormSagaOverview::~FormSagaOverview()
{
	if (mCameraMan) delete mCameraMan;
	if (mTarget) mSceneMgr->destroySceneNode(mTarget);
	Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("Group/SagaOverview");
	for (unsigned int i = 0; i < mDvdModel.size(); ++i) delete mDvdModel[i];
	for (unsigned int i = 0; i < mDiscModel.size(); ++i) delete mDiscModel[i];
	hideAllOptions();
}



bool FormSagaOverview::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_LCONTROL)	mCtrlPressed = true;
	else if (arg.key == OIS::KC_LMENU)	mAltPressed = true;
	else if (arg.key == OIS::KC_LSHIFT)	mShiftPressed = true;
	//else if (arg.key == OIS::KC_C)

	return FormBase::keyPressed(arg);
}

bool FormSagaOverview::keyReleased(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_LCONTROL)	mCtrlPressed = false;
	else if (arg.key == OIS::KC_LMENU)	mAltPressed = false;
	else if (arg.key == OIS::KC_LSHIFT)	mShiftPressed = false;

	return FormBase::keyReleased(arg);
}



bool FormSagaOverview::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;

	if (mLBPressed) // left button 
	{
		if (mAltPressed) // left ALT...
		{
			if (mShiftPressed) // ...and left SHIFT are pressed? then move camera and mTarget
			{
				mDragging = true;
				Ogre::Vector3 newPos = (mCamera->getRight() * -arg.state.X.rel * 0.040f) + (mCamera->getUp() * arg.state.Y.rel *0.025f);
				mTarget->translate(newPos);
				mCamera->move(newPos);
			}
			else if (mCtrlPressed) // ...and left CTRL are pressed? then zoom
			{
				Ogre::Real dist = (mCamera->getPosition() - _getCenterSaga()).length();
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
	}
	else if (mRBPressed)
	{

	}
	else if (mMBPressed)
	{

	}

	return FormBase::mouseMoved(arg);
}

bool FormSagaOverview::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	if (mCameraMan && mAltPressed)
		mCameraMan->injectMouseDown(arg, id);

	return FormBase::mousePressed(arg, id);
}

bool FormSagaOverview::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;

	if (mCameraMan && mAltPressed)
		mCameraMan->injectMouseUp(arg, id);

	return FormBase::mouseReleased(arg, id);
}



void FormSagaOverview::hideAllOptions()
{
	hideOptions();
}

void FormSagaOverview::hideOptions()
{

}

void FormSagaOverview::showOptions()
{

}



void FormSagaOverview::_showRandomWallpaper()
{
	std::vector<sInfoResource> infoWalls;
	mGameInfo->findWallResource(infoWalls);

	unsigned int index = rand() % infoWalls.size();
	if (!Ogre::TextureManager::getSingleton().resourceExists(infoWalls[index].name))
		GameInfo::loadImageFromDisk(infoWalls[index].path, infoWalls[index].name, "Group/SagaOverview");
	Ogre::MaterialPtr matImage = Ogre::MaterialManager::getSingleton().create("/Mat/Wallpaper", "Group/SagaOverview");
	matImage->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
	matImage->getTechnique(0)->getPass(0)->createTextureUnitState(infoWalls[index].name);
	mTrayMgr->showBackdrop(matImage->getName());
}

Ogre::StringVector FormSagaOverview::_getNameGames(const Ogre::String& pathIni)
{
	Ogre::StringVector gameNames;

	CSimpleIniA* simpleIni = new CSimpleIniA;
	SI_Error iniStatus = simpleIni->LoadFile(pathIni.c_str());
	if (iniStatus == SI_OK)
	{
		// find games
		CSimpleIniA::TNamesDepend keys;
		simpleIni->GetAllKeys("SAGA.GAMES", keys);
		CSimpleIniA::TNamesDepend::const_iterator i;
		for (i = keys.begin(); i != keys.end(); i++)
		{
			gameNames.push_back(simpleIni->GetValue("SAGA.GAMES", i->pItem, "Game"));
		}
	}
	delete simpleIni;
	return gameNames;
}

void FormSagaOverview::_addDvdModel(const Ogre::String& pathGame)
{
	GameInfo gameInfo(pathGame);

	// find and load cover
	sInfoResource infoCover;
	gameInfo.findCoverResource(infoCover);
	Ogre::String resThumb = ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM.OVERVIEW", "Cover_Resolution", 0);
	if (resThumb == "Source Size") // load the original image
	{
		GameInfo::loadImageFromDisk(infoCover.path, infoCover.nameThumb, "Group/SagaOverview", 2);
	}
	else // load the thumbnail
	{
		unsigned int resThumbValue = Ogre::StringConverter::parseInt(resThumb);
		if (!boost::filesystem::is_regular_file(infoCover.pathThumb))
			GameInfo::createThumbnail(infoCover.path, infoCover.pathThumb, resThumbValue);
		GameInfo::loadImageFromDisk(infoCover.pathThumb, infoCover.nameThumb, "Group/SagaOverview", 2);
	}

	// Closed DVD model
	mDvdModel.push_back (new CModel("mesh_closeDvd.mesh"));
	mDvdModel.back()->getNode()->translate(0, 0, -48);
	mDvdModel.back()->applyNewText(infoCover.nameThumb);
}

void FormSagaOverview::_addDiscModel(const Ogre::String& pathGame)
{
	GameInfo gameInfo(pathGame);

	// find and load discs
	std::vector<sInfoResource> infoDisc;
	gameInfo.findDiscResource(infoDisc);
	Ogre::String resThumb = ConfigReader::getSingletonPtr()->getReader()->GetValue("FORM.OVERVIEW", "Disc_Resolution", 0);

	//for (unsigned int i = 0; i < infoDisc.size(); ++i)
	if (!infoDisc.empty())
	{
		if (resThumb == "Source Size") // load the original image
		{
			GameInfo::loadImageFromDisk(infoDisc.front().path, infoDisc.front().nameThumb, "Group/SagaOverview", 2);
		}
		else // load the thumbnail
		{
			unsigned int resThumbValue = Ogre::StringConverter::parseInt(resThumb);
			if (!boost::filesystem::is_regular_file(infoDisc.front().pathThumb))
				GameInfo::createThumbnail(infoDisc.front().path, infoDisc.front().pathThumb, resThumbValue);
			GameInfo::loadImageFromDisk(infoDisc.front().pathThumb, infoDisc.front().nameThumb, "Group/SagaOverview", 2);
		}
	}

	// Closed DVD's disc [close dvd view]
	mDiscModel.push_back(new CModel("mesh_discDvd.mesh"));
	mDiscModel.back()->getNode()->translate(0, 0, -38);
	mDiscModel.back()->applyNewText(infoDisc.front().nameThumb);
}


void FormSagaOverview::_setCameraSaga()
{
	resetCamera();
	mNodeCamera->setPosition(0, 0, 0);
	mTarget->setPosition(_getCenterSaga());
	mTarget->translate(0, 9, 0);
	if (!mCameraMan)
		mCameraMan = new OgreBites::SdkCameraMan(mCamera);
	mCameraMan->setStyle(OgreBites::CS_ORBIT);
	mCameraMan->setTarget(mTarget);
	std::vector<Ogre::Real> distAux = { 70, 60, 50, 32, 22, -5, -18, -30, -45 };
	Ogre::Real distCam = (mDvdModel.size() > 1)
		? (mDvdModel.front()->getNode()->getPosition() - mDvdModel.back()->getNode()->getPosition()).length() + distAux[mDvdModel.size()]
		: 40.0f;
	mCameraMan->setYawPitchDist(Ogre::Degree(0), Ogre::Degree(22), distCam);
}

void FormSagaOverview::_setPositionModels()
{
	Ogre::Vector3 posDvd(0, 0, -50);
	Ogre::Vector3 posDisc(0, 0, -37);
	Ogre::Real incDvdX = 23.0f;
	Ogre::Real incDiscX = 23.0f;

	for (unsigned int i = 0; i < mDvdModel.size(); ++i)
	{
		
		mDvdModel[i]->getNode()->setPosition(posDvd);
		mDiscModel[i]->getNode()->setPosition(posDisc);
		posDvd.x += incDvdX;
		posDisc.x += incDiscX;
	}
}

Ogre::Vector3 FormSagaOverview::_getCenterSaga()
{
	if (mDvdModel.size()>1)
	{
		if (mDvdModel.size() % 2 == 0) // par
		{
			return mDvdModel[(mDvdModel.size() / 2) - 1]->getNode()->getPosition() + Ogre::Vector3(11.5f, 0, 0);
		}
		else
		{
			return mDvdModel[mDvdModel.size()/2]->getNode()->getPosition();
		}
	}

	return mDvdModel.back()->getNode()->getPosition();
}

