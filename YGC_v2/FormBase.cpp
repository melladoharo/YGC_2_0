#include "stdafx.h"
#include "FormBase.h"

FormBase::FormBase(GuiManager* tray, GuiListener* oldListener /*= 0*/) :
mSceneMgr(Ogre::Root::getSingletonPtr()->getSceneManager("YGC2/SceneMgr")),
mCamera(mSceneMgr->getCamera("YGC2/Camera")),
mNodeCamera(mCamera->getParentSceneNode()),
mTrayMgr(tray),
mOldListener(oldListener),
mRaySceneQuery(mSceneMgr->createRayQuery(Ogre::Ray())),
mScreenSize(Ogre::Root::getSingleton().getAutoCreatedWindow()->getViewport(0)->getActualWidth(), 
			Ogre::Root::getSingleton().getAutoCreatedWindow()->getViewport(0)->getActualHeight()),
mTimeBase(0.0f), mTimeDoubleClick(0.0f),
mLBPressed(false), mRBPressed(false), mMBPressed(false), mDoubleClick(false), mDragging(false)
{
	// add this frameListener
	Ogre::Root::getSingleton().addFrameListener(this);
	mTrayMgr->setListener(this);
	mTrayMgr->setMouseEventCallback(this);
	mTrayMgr->setKeyboardEventCallback(this);

	mIconSettings = (mTrayMgr->getWidget("FormBase/Icon/Settings"))
		? (DecorWidget*)mTrayMgr->getWidget("FormBase/Icon/Settings")
		: mTrayMgr->createDecorWidget("FormBase/Icon/Settings", "YgcGui/IconSettings");
	mIconSettings->hide();
}

FormBase::~FormBase()
{
	if (mRaySceneQuery)	mSceneMgr->destroyQuery(mRaySceneQuery);

	// destroy animation
	for (unsigned int i = 0; i < mAnimState.size(); ++i)
	{
		if (mSceneMgr->hasAnimationState(mAnimState[i]->getAnimationName()))
			mSceneMgr->destroyAnimationState(mAnimState[i]->getAnimationName());
		if (mSceneMgr->hasAnimation(mAnimState[i]->getAnimationName()))
			mSceneMgr->destroyAnimation(mAnimState[i]->getAnimationName());
	}

	// destroy widgets
	for (unsigned int i = 0; i < mWidgetsForm.size(); ++i)
		mTrayMgr->destroyWidget(mWidgetsForm[i]->getName());

	// remove this frameListener
	disableForm();
}




bool FormBase::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	mTrayMgr->frameRenderingQueued(evt);
	mTimeBase += evt.timeSinceLastFrame;
	mTimeDoubleClick += evt.timeSinceLastFrame;

	// update animations
	for (unsigned int i = 0; i < mAnimState.size(); ++i)
	{
		if (mAnimState[i]->getEnabled())
		{
			if (mAnimState[i]->hasEnded()) // animation finish?
			{
				Ogre::String nameAnim = mAnimState[i]->getAnimationName();
				mAnimState.erase(mAnimState.begin() + i);
				mSceneMgr->destroyAnimationState(nameAnim);
				mSceneMgr->destroyAnimation(nameAnim);
			}
			else
			{
				mAnimState[i]->addTime(evt.timeSinceLastFrame);
			}
		}
	}

	return true;
}

bool FormBase::keyPressed(const OIS::KeyEvent &arg)
{
	// only for debug 
	if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
		mTrayMgr->createScreenshot();
	else if (arg.key == OIS::KC_ESCAPE)
		mTrayMgr->shutDownApp();
	else if (arg.key == OIS::KC_O)
		showOptions();

	return true;
}

bool FormBase::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}

bool FormBase::mouseMoved(const OIS::MouseEvent &arg)
{
	// show the icon settings if cursor is over the right bottom corner
	if (mIconSettings->isVisible())
		mIconSettings->hide();

	if (arg.state.X.abs <= 50 && arg.state.Y.abs >= (mScreenSize.y - 50))
	{
		mIconSettings->show();
	}

	return true;
}

bool FormBase::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (id == OIS::MouseButtonID::MB_Left)
	{
		mLBPressed = true;
		if (mTimeDoubleClick > 0 && mTimeDoubleClick < 0.15f)
		{
			mDoubleClick = true;
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

	return true;
}

bool FormBase::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(mIconSettings->isVisible()) showOptions();

	mLBPressed = false;
	mRBPressed = false;
	mMBPressed = false;
	mTimeDoubleClick = 0.0f;
	mDoubleClick = false;
	mDragging = false;

	return true;
}

void FormBase::hide()
{
	for (unsigned int i = 0; i < mWidgetsForm.size(); ++i)
		mWidgetsForm[i]->hide();
}

void FormBase::show()
{
	for (unsigned int i = 0; i < mWidgetsForm.size(); ++i)
		mWidgetsForm[i]->show();
}


void FormBase::enableForm()
{
	// add this frameListener
	Ogre::Root::getSingleton().addFrameListener(this);
	mTrayMgr->setListener(this);
	mTrayMgr->setMouseEventCallback(this);
	mTrayMgr->setKeyboardEventCallback(this);
}

void FormBase::disableForm()
{
	// remove this frameListener
	Ogre::Root::getSingleton().removeFrameListener(this);
	Ogre::Root::getSingleton()._fireFrameStarted();
	mTrayMgr->setListener(NULL);
	mTrayMgr->setMouseEventCallback(NULL);
	mTrayMgr->setKeyboardEventCallback(NULL);
}


void FormBase::addWidgetToForm(Widget* w)
{
	mWidgetsForm.push_back(w);
}

void FormBase::removeWidgetFromForm(Widget* w)
{
	//mWidgetsForm
}

void FormBase::createAnimNode(const Ogre::String& nameAnim, Ogre::Real timeAnim, Ogre::SceneNode* node, const Ogre::Vector3& posOri, const Ogre::Quaternion& quaOri, const Ogre::Vector3& posDest, const Ogre::Quaternion& quaDest)
{
	// abort if exists the animation
	if (mSceneMgr->hasAnimation(nameAnim))
		return;

	Ogre::Animation* animNode = mSceneMgr->createAnimation(nameAnim, timeAnim);
	animNode->setInterpolationMode(Ogre::Animation::IM_LINEAR);
	
	Ogre::NodeAnimationTrack* animTrack = animNode->createNodeTrack(0, node);
	animTrack->createNodeKeyFrame(0);
	animTrack->getNodeKeyFrame(0)->setTranslate(posOri);
	animTrack->getNodeKeyFrame(0)->setRotation(quaOri);
	animTrack->createNodeKeyFrame(timeAnim);
	animTrack->getNodeKeyFrame(1)->setTranslate(posDest);
	animTrack->getNodeKeyFrame(1)->setRotation(quaDest);

	mAnimState.push_back(mSceneMgr->createAnimationState(nameAnim));
	mAnimState.back()->setLoop(false);
	mAnimState.back()->setEnabled(true);
}

void FormBase::destroyAnim(const Ogre::String& nameAnim)
{
	for (unsigned int i = 0; i < mAnimState.size(); ++i)
	{
		Ogre::String nameAnim = mAnimState[i]->getAnimationName();
		mAnimState.erase(mAnimState.begin() + i);
		mSceneMgr->destroyAnimationState(nameAnim);
		mSceneMgr->destroyAnimation(nameAnim);
	}
}

void FormBase::resetCamera()
{
	mCamera->setAutoTracking(false);
	mCamera->setPosition(0, 0, 0);
	mCamera->setOrientation(Ogre::Quaternion(1, 0, 0, 0));
	mNodeCamera->setPosition(0, 0, 0);
	mNodeCamera->setOrientation(Ogre::Quaternion(1, 0, 0, 0));
}



void FormBase::_correctCaption(Ogre::String& caption)
{
	if (caption.size() > 2)
	{
		for (unsigned int i = 0; i < caption.size() - 1; ++i)
		{
			if (caption[i] == '\\' && caption[i + 1] == 'n')
			{
				caption.erase(i + caption.begin());
				caption.erase(i + caption.begin());
				caption.insert(i, "\n");
			}
		}
	}
}

void FormBase::_loadModelStatusFromIni(CSimpleIniA* simpleIni, const Ogre::String& sectionKey, CModel* model)
{
	Ogre::Vector3 posModel(
		simpleIni->GetDoubleValue(sectionKey.c_str(), "Position_x", 0),
		simpleIni->GetDoubleValue(sectionKey.c_str(), "Position_y", 0),
		simpleIni->GetDoubleValue(sectionKey.c_str(), "Position_z", 0));
	model->getNode()->setPosition(posModel);

	Ogre::Quaternion quaModel(
		simpleIni->GetDoubleValue(sectionKey.c_str(), "Rotation_w", 1),
		simpleIni->GetDoubleValue(sectionKey.c_str(), "Rotation_x", 0),
		simpleIni->GetDoubleValue(sectionKey.c_str(), "Rotation_y", 0),
		simpleIni->GetDoubleValue(sectionKey.c_str(), "Rotation_z", 0));
	model->getNode()->setOrientation(quaModel);

	Ogre::Vector3 sclModel(
		simpleIni->GetDoubleValue(sectionKey.c_str(), "Scale_x", 1),
		simpleIni->GetDoubleValue(sectionKey.c_str(), "Scale_y", 1),
		simpleIni->GetDoubleValue(sectionKey.c_str(), "Scale_z", 1));
	model->getNode()->setScale(sclModel);
}

bool FormBase::_saveModelStatusFromIni(CSimpleIniA* simpleIni, const Ogre::String& sectionKey, CModel* model, const Ogre::String& pathIni)
{
	Ogre::Vector3 posModel = model->getPosition();
	simpleIni->SetDoubleValue(sectionKey.c_str(), "Position_x", posModel.x);
	simpleIni->SetDoubleValue(sectionKey.c_str(), "Position_y", posModel.y);
	simpleIni->SetDoubleValue(sectionKey.c_str(), "Position_z", posModel.z);

	Ogre::Quaternion quaModel = model->getNode()->getOrientation();
	simpleIni->SetDoubleValue(sectionKey.c_str(), "Rotation_w", quaModel.w);
	simpleIni->SetDoubleValue(sectionKey.c_str(), "Rotation_x", quaModel.x);
	simpleIni->SetDoubleValue(sectionKey.c_str(), "Rotation_y", quaModel.y);
	simpleIni->SetDoubleValue(sectionKey.c_str(), "Rotation_z", quaModel.z);

	Ogre::Vector3 sclModel = model->getNode()->getScale();
	simpleIni->SetDoubleValue(sectionKey.c_str(), "Scale_x", sclModel.x);
	simpleIni->SetDoubleValue(sectionKey.c_str(), "Scale_y", sclModel.y);
	simpleIni->SetDoubleValue(sectionKey.c_str(), "Scale_z", sclModel.z);

	SI_Error saveStatus = simpleIni->SaveFile(pathIni.c_str());
	if (saveStatus == SI_OK)
		return true; // ok

	return false;
}

