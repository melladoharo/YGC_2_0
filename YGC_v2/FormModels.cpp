#include "stdafx.h"
#include "FormModels.h"

FormModels::FormModels(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener /*= 0*/) :
FormBase(tray, oldListener),
mGameInfo(gInfo),
mState(FM_OVERVIEW),
mTarget(0),
mController(0),
mCurrentIndex(0),
mAudioPlayer(0),
mCtrlPressed(false), mAltPressed(false), mShiftPressed(false),
mStartVoice(false)
{
	// create target
	mTarget = mSceneMgr->getRootSceneNode()->createChildSceneNode();

	// load models from resources paths
	std::vector<sInfoResource> infoModels;
	mGameInfo->findModelsResources(infoModels);
	mGameInfo->findObjectsResources(infoModels);
	for (unsigned int i = 0; i < infoModels.size(); ++i)
		_loadModel(infoModels[i]);

	// options
	Ogre::Real padLeft = mScreenSize.x / 18.0f;
	Ogre::Real padTop = mScreenSize.y / 36.0f;
	Ogre::Real padRight = mScreenSize.x / 25.0f;
	
	// title
	Ogre::Real titleWidth = (mScreenSize.x / 2.0f) + padLeft;
	Ogre::Real titleCaptionSize = (mScreenSize.x > 1400) ? 44 : 32;
	Ogre::Real titleHeight = titleCaptionSize + padTop;
	SimpleText* stTitle = mTrayMgr->createSimpleText("FormModels/Text/Title", "", 
		"YgcFont/SemiBold/16", titleWidth, titleHeight, titleCaptionSize, 1);
	stTitle->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_RIGHT);
	stTitle->setTop(mScreenSize.y / 3.2f);
	stTitle->setLeft(-titleWidth);
	stTitle->hide();
	addWidgetToForm(stTitle);

	// decoration bar
	DecorWidget* decorBar = mTrayMgr->createDecorWidget("FormModels/DecorBar", "YgcGui/DecorBar");
	decorBar->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_RIGHT);
	decorBar->getOverlayElement()->setWidth((stTitle->getOverlayElement()->getWidth() - padRight) + 10);
	decorBar->setTop(stTitle->getTop() + titleCaptionSize / 1.2f);
	decorBar->setLeft(stTitle->getLeft() - 5);
	decorBar->hide();
	addWidgetToForm(decorBar);

	// description
	Ogre::Real overviewCaptionSize = (mScreenSize.x > 1400) ? 26 : 20;
	SimpleText* stOverview = mTrayMgr->createSimpleText("FormModels/Text/Desc", "",
		"YgcFont/SemiBold/16", titleWidth - padRight, 450, overviewCaptionSize, 8);
	stOverview->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_RIGHT);
	stOverview->setTop(stTitle->getTop() + padTop * 3);
	stOverview->setLeft(stTitle->getLeft());
	stOverview->hide();
	addWidgetToForm(stOverview);

	// slider
	Ogre::Real minValue = 0;
	Ogre::Real maxValue = mModels.size() - 1;
	Slider* sd = mTrayMgr->createSlider("FormModels/Slider", 370, minValue, maxValue, mModels.size());
	sd->getOverlayElement()->setVerticalAlignment(Ogre::GVA_BOTTOM);
	sd->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
	sd->setLeft(-(sd->getWidth() / 2));
	sd->setTop(-45);
	sd->hide();
	addWidgetToForm(sd);

	// set cam and view models
	_setFormToOverview();
}

FormModels::~FormModels()
{
	hideAllOptions();

	mTrayMgr->setDofEffectEnable(false);
	mSceneMgr->destroySceneNode(mTarget);
	if (mController) delete mController;
	if (mAudioPlayer) delete mAudioPlayer;

	for (unsigned int i = 0; i < mModels.size(); ++i)
		delete mModels[i].model;
}


bool FormModels::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (mCameraMan)
		mCameraMan->frameRenderingQueued(evt);

	// loop over the views of model playing the next animation view when the current ends
	if (mState == FM_DETAILS && !mSceneMgr->hasAnimation("FormModels/Anim/CameraView"))
	{
		mTrayMgr->enableFadeEffect();
		_playAnimation(mModels[mCurrentIndex]);
	}

	// stop voices
	if (mStartVoice && mAudioPlayer->endSound())
	{
		mModels[mCurrentIndex].currentVoice = 
			(mModels[mCurrentIndex].currentVoice == mModels[mCurrentIndex].voices.size() - 1) ? 0 : mModels[mCurrentIndex].currentVoice + 1;
		mStartVoice = false;
		mTrayMgr->setVolumenMiniPlayer(-500);
	}

	return FormBase::frameRenderingQueued(evt);
}

bool FormModels::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_LCONTROL)	mCtrlPressed = true;
	else if (arg.key == OIS::KC_LMENU)	mAltPressed = true;
	else if (arg.key == OIS::KC_LSHIFT)	mShiftPressed = true;

	if (mCameraMan)
		mCameraMan->injectKeyDown(arg);

	if (mController)
		mController->injectKeyDown(arg);

	// update the lineEdit widgets
	LineEdit* leName = dynamic_cast<LineEdit*>(mTrayMgr->getWidget("FormModels/LineEdit/Name"));
	if (leName)
	{
		leName->injectKeyPress(arg);
		SimpleText* stTitle = dynamic_cast<SimpleText*>(mTrayMgr->getWidget("FormModels/Text/Title"));
		if (stTitle) stTitle->setText(leName->getText());
	}
	LineEdit* leDesc = dynamic_cast<LineEdit*>(mTrayMgr->getWidget("FormModels/LineEdit/Desc"));
	if (leDesc)
	{
		leDesc->injectKeyPress(arg);
		SimpleText* stDesc = dynamic_cast<SimpleText*>(mTrayMgr->getWidget("FormModels/Text/Desc"));
		if (stDesc) stDesc->setText(leDesc->getText());
	}

	return FormBase::keyPressed(arg);
}

bool FormModels::keyReleased(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_LCONTROL)	mCtrlPressed = false;
	else if (arg.key == OIS::KC_LMENU)	mAltPressed = false;
	else if (arg.key == OIS::KC_LSHIFT)	mShiftPressed = false;
	else if (arg.key == OIS::KC_C)
	{
		if (mState == FM_ZOOM) _setFormToZoom();
	}

	if (mCameraMan)
		mCameraMan->injectKeyUp(arg);

	if (mController)
		mController->injectKeyUp(arg);

	return FormBase::keyReleased(arg);
}

bool FormModels::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;

	if (mController)
		mController->injectMouseMove(arg);

	if (mLBPressed) // left button 
	{
		if (mState == FM_ZOOM || mState == FM_EDITCAMERA)
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
					Ogre::Real dist = (mCamera->getPosition() - mModels[mCurrentIndex].model->getNode()->_getDerivedPosition()).length();
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
				mModels[mCurrentIndex].model->getNode()->yaw(Ogre::Degree(arg.state.X.rel * 0.40f));
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

bool FormModels::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	if (mCameraMan && mAltPressed)
		mCameraMan->injectMouseDown(arg, id);

	if (mController)
		mController->injectMouseDown(arg, id);

	if (id == OIS::MouseButtonID::MB_Left)
	{
		// pass to the next view animation
		if (mState == FM_DETAILS)
		{
			destroyAnim("FormModels/Anim/CameraView");
			mTrayMgr->enableFadeEffect();
			_playAnimation(mModels[mCurrentIndex]);
		}

		if (mTimeDoubleClick > 0 && mTimeDoubleClick < 0.15f)
		{
			if (!mTrayMgr->isWindowDialogVisible()) // option dialog is hidden?
			{
				if (mState == FM_OVERVIEW)
				{
					if (!mModels.empty())
					{
						mTrayMgr->enableFadeEffect();
						_setFormToZoom();
					}
				}
				else if (mState == FM_ZOOM)
				{
					if (!mModels[mCurrentIndex].views.empty())
					{
						mTrayMgr->enableFadeEffect();
						_setFormToDetails();
					}
				}
			}
		}
	}
	else if (id == OIS::MouseButtonID::MB_Right)
	{
		
	}
	else if (id == OIS::MouseButtonID::MB_Middle)
	{
		
	}

	return FormBase::mousePressed(arg, id);;
}

bool FormModels::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;

	if (mCameraMan && mAltPressed)
		mCameraMan->injectMouseUp(arg, id);

	if (mController)
		mController->injectMouseUp(arg, id);

	if (mLBPressed)
	{
		
	}
	else if (mRBPressed)
	{
		if (mState == FM_ZOOM)
		{
			mTrayMgr->enableFadeEffect();
			_setFormToOverview();	
		}
		else if (mState == FM_DETAILS)
		{
			mTrayMgr->enableFadeEffect();
			_setFormToZoom();
		}
	}
	else if (mMBPressed)
	{
		if (mState == FM_OVERVIEW || mState == FM_ZOOM)
		{
			if (mCurrentIndex >= 0 && mCurrentIndex < mModels.size())
			{
				unsigned int currVoice = mModels[mCurrentIndex].currentVoice;
				if (currVoice >= 0 && currVoice < mModels[mCurrentIndex].voices.size())
				{
					// if is playing a track, pause
					if (mTrayMgr->isMiniPlaying())
					{
						mTrayMgr->setVolumenMiniPlayer(-5000);
					}

					if (mAudioPlayer) delete mAudioPlayer;
					mAudioPlayer = new DirectShowSound(const_cast<char *>(mModels[mCurrentIndex].voices[currVoice].c_str()));
					mAudioPlayer->Play();
					mAudioPlayer->setVolume(-500);
					mStartVoice = true;
				}
			}
		}
	}

	return FormBase::mouseReleased(arg, id);
}



void FormModels::buttonHit(Button* button)
{
	if (button->getName() == "FormModels/Button/CloseOptions")
	{
		hideOptions();
	}
	else if (button->getName() == "FormModels/Button/CloseOptionsEditModel")
	{
		LineEdit* leName = dynamic_cast<LineEdit*>(mTrayMgr->getWidget("FormModels/LineEdit/Name"));
		LineEdit* leDesc = dynamic_cast<LineEdit*>(mTrayMgr->getWidget("FormModels/LineEdit/Desc"));
		Ogre::String strName = (leName) ? leName->getText() : Ogre::StringUtil::BLANK;
		Ogre::String strDesc = (leDesc) ? leDesc->getText() : Ogre::StringUtil::BLANK;
		LineEdit::replaceNewLineEscapeToINI(strName);
		LineEdit::replaceNewLineEscapeToINI(strDesc);
		mModels[mCurrentIndex].simpleIni->SetValue("MODEL.INFO", "Name", strName.c_str());
		mModels[mCurrentIndex].simpleIni->SetValue("MODEL.INFO", "Overview", strDesc.c_str());
		mModels[mCurrentIndex].simpleIni->SaveFile(mModels[mCurrentIndex].pathIni.c_str());
		hideOptionEditModel();
		showOptions();
	}
	else if (button->getName() == "FormModels/Button/CloseOptionsEditPosition")
	{
		if (mController) delete mController;
		mController = 0;
		_saveModelStatusFromIni(mModels[mCurrentIndex].simpleIni, "MODEL.INITIAL.STATUS",
			mModels[mCurrentIndex].model, mModels[mCurrentIndex].pathIni);
		mModels[mCurrentIndex].model->getNode()->setInitialState();
		hideOptionEditPosition();
		showOptions();
	}
	else if (button->getName() == "FormModels/Button/WindowsEditPositionHide")
	{
		if (mTrayMgr->getWidget("FormModels/Window/EditPosition")->isVisible())
		{
			mTrayMgr->getWidget("FormModels/Window/EditPosition")->hide();
			mTrayMgr->getWidget("FormModels/SimpleText/EditPosition")->hide();
			Button* bttHide = dynamic_cast<Button*>(mTrayMgr->getWidget("FormModels/Button/WindowsEditPositionHide"));
			bttHide->setCaption("SHOW");
		}
		else
		{
			mTrayMgr->getWidget("FormModels/Window/EditPosition")->show();
			mTrayMgr->getWidget("FormModels/SimpleText/EditPosition")->show();
			Button* bttHide = dynamic_cast<Button*>(mTrayMgr->getWidget("FormModels/Button/WindowsEditPositionHide"));
			bttHide->setCaption("HIDE");
		}
	}
	else if (button->getName() == "FormModels/Button/EditPositionReset")
	{
		mModels[mCurrentIndex].model->getNode()->setPosition(Ogre::Vector3(-10, 0, -50));
		mModels[mCurrentIndex].model->getNode()->setOrientation(Ogre::Quaternion());
		mModels[mCurrentIndex].model->getNode()->setScale(1, 1, 1);
	}
	else if (button->getName() == "FormModels/Button/CloseOptionsViews")
	{
		hideOptionViews();
		showOptions();
		_saveCameraView(mModels[mCurrentIndex]);
		_setFormToZoom();
	}
	else if (button->getName() == "FormModels/Button/CloseOptionsEditCamera")
	{
		if (mController) delete mController;
		mController = 0;
		hideOptionCamera();
		showOptionViews(mModels[mCurrentIndex]);
		mState = FM_EDITVIEWS;
	}
	else if (button->getName() == "FormModels/Button/CloseOptionsEditTarget")
	{
		if (mController) delete mController;
		mController = 0;
		hideOptionTarget();
		showOptionViews(mModels[mCurrentIndex]);
		mState = FM_EDITVIEWS;
	}
}

void FormModels::sliderMoved(Slider* slider)
{
	if (mCurrentIndex != slider->getValue())
	{
		mTrayMgr->enableFadeEffect();
		_showModel(slider->getValue());
	}
}

void FormModels::sliderOptionsMoved(SliderOptions* slider)
{
	if (slider->getName() == "FormModels/Slider/Focus")
	{
		if (!mModels[mCurrentIndex].views.empty())
		{
			mModels[mCurrentIndex].views[mModels[mCurrentIndex].currentView].dofFocus = slider->getValue();
			mTrayMgr->setDofEffectFocus(slider->getValue());
		}
	}
}

void FormModels::labelHit(Label* label)
{
	if (label->getName() == "FormModels/Label/EditModel")
	{
		if (!mModels.empty())
		{
			hideOptions();
			showOptionEditModel();
		}
	}
	else if (label->getName() == "FormModels/Label/EditPosition")
	{
		if (!mModels.empty())
		{
			mController = new CModelController(mModels[mCurrentIndex].model->getNode());
			hideOptions();
			showOptionEditPosition();
		}
	}
	else if (label->getName() == "FormModels/Label/EditViews")
	{
		if (!mModels.empty())
		{
			hide();
			mModels[mCurrentIndex].model->show();
			mModels[mCurrentIndex].currentView = 0;
			hideOptions();
			showOptionViews(mModels[mCurrentIndex]);
			_setOrbitCamera();
			_showCameraView(mModels[mCurrentIndex]);
		}
	}
	else if (label->getName() == "FormModels/Label/NewView")
	{
		_saveCameraView(mModels[mCurrentIndex]);
		_setOrbitCamera();
		_newCameraView(mModels[mCurrentIndex]);
		_loadConfigWindowsViews(mModels[mCurrentIndex], true);
	}
	else if (label->getName() == "FormModels/Label/DeleteView")
	{
		if (!mModels[mCurrentIndex].views.empty())
		{
			_deleteCameraView(mModels[mCurrentIndex]);
			_showCameraView(mModels[mCurrentIndex]);
			_loadConfigWindowsViews(mModels[mCurrentIndex]);
		}
	}
	else if (label->getName() == "FormModels/Label/EditCamera")
	{
		if (!mModels[mCurrentIndex].views.empty())
		{
			if (mController) delete mController;
			mController = 0;
			mState = FM_EDITCAMERA;
			hideOptionViews();
			showOptionCamera();
		}
	}
	else if (label->getName() == "FormModels/Label/EditTarget")
	{
		if (!mModels[mCurrentIndex].views.empty())
		{
			mController = new CModelController(mTarget);
			hideOptionViews();
			showOptionTarget();
		}
	}
}

void FormModels::itemChanged(ItemSelector* selector)
{
	if (selector->getName() == "FormModels/Selector/Views")
	{
		// saves the changes before pass to the next (or previous) model
		_saveCameraView(mModels[mCurrentIndex]);

		// pass to next (or previous) model, and load its parameters in the windows configuration
		Ogre::String viewString = selector->getSelectedOption();
		mModels[mCurrentIndex].currentView = Ogre::StringConverter::parseInt(viewString.substr(viewString.size() - 1));
		_showCameraView(mModels[mCurrentIndex]);
		_loadConfigWindowsViews(mModels[mCurrentIndex]);
	}

	else if (selector->getName() == "FormModels/Selector/Dof")
	{
		if (!mModels[mCurrentIndex].views.empty())
		{
			// enable or disable the depth of field effect for the current view
			mTrayMgr->setDofEffectEnable((selector->getSelectedOption() == "Enable") ? true : false);
			mModels[mCurrentIndex].views[mModels[mCurrentIndex].currentView].dofEnable = mTrayMgr->getDofEffectEnable();
		}
	}
}



void FormModels::hide()
{
	// if menu bar changes from models to other form when the animation is playing, remove it
	destroyAnim("FormModels/Anim/CameraView");
	// hide (destroy) all the windows configuration visibles and effects applied to form
	hideAllOptions();
	mTrayMgr->setDofEffectEnable(false);
	// hide the rest of things
	FormBase::hide();
	if (!mModels.empty()) mModels[mCurrentIndex].model->hide();
	// hide logo
	Widget* logoGame = mTrayMgr->getWidget("FormOverview/Logo");
	if (logoGame) logoGame->hide();
}

void FormModels::show()
{
	// [see _showModel function]
	_setFormToOverview();
	// show logo
	Widget* logoGame = mTrayMgr->getWidget("FormOverview/Logo");
	if (logoGame) logoGame->show();
}



void FormModels::_loadModel(const sInfoResource& infoModel)
{
	// new model from zip
	Ogre::String rootPathModels = boost::filesystem::path(infoModel.path).remove_filename().generic_string();
	mModels.push_back(sModel());
	mModels.back().name = infoModel.caption;
	mModels.back().pathIni = rootPathModels + "/" + mModels.back().name + ".ini";
	mModels.back().model = new CModel(mModels.back().name + ".mesh");
	mModels.back().simpleIni = new CSimpleIniA;
	mModels.back().iniStatus = mModels.back().simpleIni->LoadFile(Ogre::String(mModels.back().pathIni).c_str());
	mModels.back().model->hide();
	mModels.back().currentView = 0;	
	mModels.back().currentVoice = 0;
	_loadModelStatusFromIni(mModels.back().simpleIni, "MODEL.INITIAL.STATUS", mModels.back().model);
	_loadCameraView(mModels.back());
	_loadModelVoices(mModels.back());

	// default values for new models
	if (!boost::filesystem::is_regular_file(mModels.back().pathIni))
	{
		mModels.back().simpleIni->SetValue("MODEL.INFO", "Name", "Name");
		mModels.back().simpleIni->SetValue("MODEL.INFO", "Overview", "Overview");
		mModels.back().model->setPosition(Ogre::Vector3(-10, 0, -50));
		_saveModelStatusFromIni(mModels.back().simpleIni, "MODEL.INITIAL.STATUS", mModels.back().model, mModels.back().pathIni);
	}

	// save status model
	mModels.back().model->getNode()->setInitialState();
}

void FormModels::_showModel(unsigned int index)
{
	if (index >= 0 && index < mModels.size())
	{
		// hide the current model
		mModels[mCurrentIndex].model->hide();

		// show the current model 
		mModels[index].model->show();
		mModels[index].model->getNode()->resetToInitialState(); // restore default rotation if yaw the model in zoom view [ctrl + clic]
		SimpleText* stTitle = dynamic_cast<SimpleText*>(mTrayMgr->getWidget("FormModels/Text/Title"));
		SimpleText* stDesc = dynamic_cast<SimpleText*>(mTrayMgr->getWidget("FormModels/Text/Desc"));
		Ogre::String strName = mModels[index].simpleIni->GetValue("MODEL.INFO", "Name", "Name");
		Ogre::String strDesc = mModels[index].simpleIni->GetValue("MODEL.INFO", "Overview", "Overview");
		LineEdit::replaceNewLineEscapeFromINI(strName);
		LineEdit::replaceNewLineEscapeFromINI(strDesc);
		if (stTitle) { stTitle->show(); stTitle->setText(strName); }
		if (stDesc) { stDesc->show(); stDesc->setText(strDesc); }
		Widget* decorBar = mTrayMgr->getWidget("FormModels/DecorBar");
		if (decorBar) decorBar->show();
		Widget* slider = mTrayMgr->getWidget("FormModels/Slider");
		if (slider && mModels.size() > 1) slider->show();
		mCurrentIndex = index;
	}
}



void FormModels::_setOrbitCamera()
{
	// size model
	//mModels[mCurrentIndex].model->getNode()->showBoundingBox(true);
	Ogre::AxisAlignedBox aaBox(Ogre::AxisAlignedBox(mModels[mCurrentIndex].model->getNode()->_getWorldAABB()));
	mNodeCamera->setPosition(0, 0, 0);
	mTarget->setPosition(aaBox.getCenter());
	mTarget->translate(0, 1, 0);
	if (!mCameraMan)
		mCameraMan = new OgreBites::SdkCameraMan(mCamera);
	mCameraMan->setStyle(OgreBites::CS_ORBIT);
	mCameraMan->setTarget(mTarget);
	mCameraMan->setYawPitchDist(Ogre::Degree(0), Ogre::Degree(0), aaBox.getSize().y + 10);
}

void FormModels::_setFormToZoom()
{
	if (!mAnimState.empty())
		destroyAnim(mAnimState.back()->getAnimationName());
	if (mController)
		delete mController;
	mController = 0;
	mTrayMgr->setDofEffectEnable(false);
	FormBase::hide();
	_setOrbitCamera();
	mState = FM_ZOOM;
}

void FormModels::_setFormToOverview()
{
	if (mCameraMan) delete mCameraMan;
	mCameraMan = 0;
	mCamera->setAutoTracking(false);
	mCamera->setPosition(0, 0, 0);
	mCamera->setOrientation(mNodeCamera->getOrientation());
	mNodeCamera->setPosition(0.0f, 12.0f, -5.0f);
	_showModel(mCurrentIndex);
	mState = FM_OVERVIEW;
}

void FormModels::_setFormToDetails()
{
	mModels[mCurrentIndex].currentView = 0;
	_showCameraView(mModels[mCurrentIndex]);
	_playAnimation(mModels[mCurrentIndex]);
	mState = FM_DETAILS;
}



bool FormModels::_loadCameraView(sModel& model)
{
	if (model.iniStatus == SI_OK)
	{
		CSimpleIniA::TNamesDepend sections;
		model.simpleIni->GetAllSections(sections);
		for (CSimpleIniA::TNamesDepend::const_iterator i = sections.begin(); i != sections.end(); ++i)
		{
			// only the sections contains this name 'CAMERA.VIEW.' 
			Ogre::String sectionName = i->pItem;
			if (sectionName.find("CAMERA.VIEW.") != Ogre::String::npos)
			{
				model.views.push_back(sView());
				model.views.back().cameraPos = Ogre::Vector3(
					model.simpleIni->GetDoubleValue(sectionName.c_str(), "Camera_Pos_x", 0),
					model.simpleIni->GetDoubleValue(sectionName.c_str(), "Camera_Pos_y", 0),
					model.simpleIni->GetDoubleValue(sectionName.c_str(), "Camera_Pos_z", 0));
				model.views.back().cameraRot = Ogre::Quaternion(
					model.simpleIni->GetDoubleValue(sectionName.c_str(), "Camera_Rot_w", 1),
					model.simpleIni->GetDoubleValue(sectionName.c_str(), "Camera_Rot_x", 0),
					model.simpleIni->GetDoubleValue(sectionName.c_str(), "Camera_Rot_y", 0),
					model.simpleIni->GetDoubleValue(sectionName.c_str(), "Camera_Rot_z", 0));
				model.views.back().targetPos = Ogre::Vector3(
					model.simpleIni->GetDoubleValue(sectionName.c_str(), "Target_Pos_x", 0),
					model.simpleIni->GetDoubleValue(sectionName.c_str(), "Target_Pos_y", 0),
					model.simpleIni->GetDoubleValue(sectionName.c_str(), "Target_Pos_z", 0));
				model.views.back().dofEnable = model.simpleIni->GetBoolValue(sectionName.c_str(), "Dof_Effect", false);
				model.views.back().dofFocus = model.simpleIni->GetDoubleValue(sectionName.c_str(), "Dof_Focus", 2);
			}
		}
		return true; // ok
	}
	return false; // error
}

bool FormModels::_saveCameraView(sModel& model)
{
	if (model.iniStatus == SI_OK && model.currentView < model.views.size())
	{
		Ogre::String sectionKey = "CAMERA.VIEW." + Ogre::StringConverter::toString(model.currentView);

		model.views[model.currentView].cameraPos = mCamera->getPosition();
		model.views[model.currentView].cameraRot = mCamera->getOrientation();
		model.views[model.currentView].targetPos = mTarget->getPosition();

		model.simpleIni->SetDoubleValue(sectionKey.c_str(), "Camera_Pos_x", model.views[model.currentView].cameraPos.x);
		model.simpleIni->SetDoubleValue(sectionKey.c_str(), "Camera_Pos_y", model.views[model.currentView].cameraPos.y);
		model.simpleIni->SetDoubleValue(sectionKey.c_str(), "Camera_Pos_z", model.views[model.currentView].cameraPos.z);
		model.simpleIni->SetDoubleValue(sectionKey.c_str(), "Camera_Rot_w", model.views[model.currentView].cameraRot.w);
		model.simpleIni->SetDoubleValue(sectionKey.c_str(), "Camera_Rot_x", model.views[model.currentView].cameraRot.x);
		model.simpleIni->SetDoubleValue(sectionKey.c_str(), "Camera_Rot_y", model.views[model.currentView].cameraRot.y);
		model.simpleIni->SetDoubleValue(sectionKey.c_str(), "Camera_Rot_z", model.views[model.currentView].cameraRot.z);
		model.simpleIni->SetDoubleValue(sectionKey.c_str(), "Target_Pos_x", model.views[model.currentView].targetPos.x);
		model.simpleIni->SetDoubleValue(sectionKey.c_str(), "Target_Pos_y", model.views[model.currentView].targetPos.y);
		model.simpleIni->SetDoubleValue(sectionKey.c_str(), "Target_Pos_z", model.views[model.currentView].targetPos.z);
		model.simpleIni->SetBoolValue(sectionKey.c_str(), "Dof_Effect", model.views[model.currentView].dofEnable);
		model.simpleIni->SetDoubleValue(sectionKey.c_str(), "Dof_Focus", model.views[model.currentView].dofFocus);

		SI_Error saveStatus = model.simpleIni->SaveFile(model.pathIni.c_str());
		if (saveStatus == SI_OK)
			return true; // ok
	}
	return false; // error
}

void FormModels::_newCameraView(sModel& model)
{
	model.views.push_back(sView());
	model.currentView = model.views.size() - 1;
	model.views.back().targetPos = mTarget->getPosition();
	model.views.back().cameraPos = Ogre::Vector3(0, 0, 0);
	model.views.back().cameraRot = Ogre::Quaternion(1, 0, 0, 0);
	model.views.back().dofEnable = false;
	model.views.back().dofFocus = 0;
	mTrayMgr->setDofEffectEnable(false);
	mTrayMgr->setDofEffectFocus(0);
}

bool FormModels::_deleteCameraView(sModel& model)
{
	if (model.currentView >= 0 && model.currentView < model.views.size())
	{
		Ogre::String sectionKey = "CAMERA.VIEW." + Ogre::StringConverter::toString(model.currentView);
		if (model.iniStatus == SI_OK)
		{
			model.simpleIni->Delete(sectionKey.c_str(), NULL);
			model.simpleIni->SaveFile(model.pathIni.c_str());
			model.views.erase(model.views.begin() + model.currentView);
			model.currentView = 0;
			return true; // ok
		} 
		return false; // error
	}
	return false; // error
}

void FormModels::_showCameraView(sModel& model)
{
	if (model.currentView >= 0 && model.currentView < model.views.size())
	{
		mNodeCamera->setPosition(0, 0, 0);
		mNodeCamera->setOrientation(Ogre::Quaternion());
		mCamera->setPosition(model.views[model.currentView].cameraPos);
		mCamera->setOrientation(model.views[model.currentView].cameraRot);
		mTarget->setPosition(model.views[model.currentView].targetPos);
		mTrayMgr->setDofEffectEnable(model.views[model.currentView].dofEnable);
		mTrayMgr->setDofEffectFocus(model.views[model.currentView].dofFocus);
	}
	else
	{
		_setOrbitCamera();
	}
}

void FormModels::_loadConfigWindowsViews(sModel& model, bool newView /*= false*/)
{
	SliderOptions* slFocus = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormModels/Slider/Focus"));
	ItemSelector* selDof = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormModels/Selector/Dof"));
	ItemSelector* selViews = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormModels/Selector/Views"));
	Ogre::StringVector itemsViews;
	for (unsigned int i = 0; i < model.views.size(); ++i)
		itemsViews.push_back("View " + Ogre::StringConverter::toString(i));
	selViews->setItems(itemsViews);

	if (newView)
	{
		slFocus->setValue(0, false);
		selDof->selectOption("Disable", false);
		selViews->selectOption(itemsViews.back(), false);
	}
	// empty views - show default view
	else if (model.views.empty())
	{
		slFocus->setValue(0, false);
		selDof->selectOption("Disable", false);
		selViews->setNullOption("No views");
	}
	// show the current view options
	else
	{
		slFocus->setValue(model.views[model.currentView].dofFocus, false);
		selDof->selectOption((model.views[model.currentView].dofEnable) ? "Enable" : "Disable", false);
		selViews->selectOption(itemsViews[model.currentView], false);
	}
}

void FormModels::_playAnimation(sModel& model)
{
	if (model.currentView >= 0 && model.currentView < model.views.size())
	{
		// set the camera to current view
		_showCameraView(model);
		Ogre::Vector3 destCam = mNodeCamera->getPosition();
		destCam += mCamera->getRight() * Ogre::Math::RangeRandom(1.75f, 2.05f) * (Ogre::Math::RangeRandom(0, 2) > 1 ? 1 : -1);
		destCam += mCamera->getUp() * Ogre::Math::RangeRandom(0.1f, 0.3f) * (Ogre::Math::RangeRandom(0, 2) > 1 ? 1 : -1);
		destCam += mCamera->getDirection() * Ogre::Math::RangeRandom(0.1f, 1.5f);
		createAnimNode("FormModels/Anim/CameraView", Ogre::Math::RangeRandom(8, 12), mNodeCamera,
			mNodeCamera->getPosition(), mNodeCamera->getOrientation(),
			destCam, mNodeCamera->getOrientation());
		// prepares the next animation to the next view
		model.currentView = (model.currentView == model.views.size() - 1) ? 0 : model.currentView + 1;
	}
}



bool FormModels::_loadModelVoices(sModel& model)
{
	if (model.iniStatus == SI_OK)
	{
		// find voices
		CSimpleIniA::TNamesDepend keys;
		model.simpleIni->GetAllKeys("MODEL.VOICES", keys);
		CSimpleIniA::TNamesDepend::const_iterator i;
		for (i = keys.begin(); i != keys.end(); i++)
		{
			// full path voice
			Ogre::String nameVoice = model.simpleIni->GetValue("MODEL.VOICES", i->pItem, "Voice");
			model.voices.push_back(mGameInfo->getPathGame() + "/Models/Characters/Voices/" + nameVoice);
		}
		return true;
	}
	return false;
}



void FormModels::hideAllOptions()
{
	hideOptions();
	hideOptionEditModel();
	hideOptionEditPosition();
	hideOptionViews();
	hideOptionTarget();
	hideOptionCamera();
}

void FormModels::hideOptions()
{
	if (mTrayMgr->getWidget("FormModels/Window/Options"))
	{
		mTrayMgr->destroyDialogWindow("FormModels/Window/Options");
		mTrayMgr->destroyWidget("FormModels/Label/EditModel");
		mTrayMgr->destroyWidget("FormModels/Label/EditPosition");
		mTrayMgr->destroyWidget("FormModels/Label/EditViews");
		mTrayMgr->destroyWidget("FormModels/Label/Help");
		mTrayMgr->destroyWidget("FormModels/Button/CloseOptions");
	}
}

void FormModels::showOptions()
{
	// don´t show the main windows options when the anim is playing, so remove it
	if (mState == FM_DETAILS) 
	{ 
		destroyAnim("FormModels/Anim/CameraView");
		_setFormToZoom(); 
	}
	
	// all submenu options are hiddens?
	if (!mTrayMgr->isWindowDialogVisible())
	{
		unsigned int numOptions = 5;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 350;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		mTrayMgr->createDialogWindow("FormModels/Window/Options", "MODELS OPTIONS", left, top, width, height);	top += sepOptions / 2;
		mTrayMgr->createLabel("FormModels/Label/EditModel", "EDIT MODEL", left, top, width, 23);			top += sepOptions;
		mTrayMgr->createLabel("FormModels/Label/EditPosition", "EDIT POSITION", left, top, width, 23);	top += sepOptions;
		mTrayMgr->createLabel("FormModels/Label/EditViews", "EDIT VIEWS", left, top, width, 23);			top += sepOptions;
		mTrayMgr->createLabel("FormModels/Label/Help", "HELP", left, top, width, 23);	top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormModels/Button/CloseOptions", "BACK", left, top, 60);
	}
}

void FormModels::hideOptionEditModel()
{
	if (mTrayMgr->getWidget("FormModels/Window/EditModel"))
	{
		mTrayMgr->destroyDialogWindow("FormModels/Window/EditModel");
		mTrayMgr->destroyWidget("FormModels/LineEdit/Name");
		mTrayMgr->destroyWidget("FormModels/LineEdit/Desc");
		mTrayMgr->destroyWidget("FormModels/Button/CloseOptionsEditModel");
	}
}

void FormModels::showOptionEditModel()
{
	unsigned int numOptions = 3;
	Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
	Ogre::Real left = 50;
	Ogre::Real width = 425;
	Ogre::Real height = numOptions * sepOptions;
	Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;

	mTrayMgr->createDialogWindow("FormModels/Window/EditModel", "EDIT MODEL", left, top, width, height);	 top += sepOptions / 2;
	LineEdit* leName = mTrayMgr->createLineEdit("FormModels/LineEdit/Name", "Name       ", "", left, top, width); top += sepOptions;
	LineEdit* leDesc = mTrayMgr->createLineEdit("FormModels/LineEdit/Desc", "Description", "", left, top, width); top = mScreenSize.y - sepWindow - sepButton;
	mTrayMgr->createButton("FormModels/Button/CloseOptionsEditModel", "BACK", left, top, 60);

	if (!mModels.empty())
	{
		SimpleText* stTitle = dynamic_cast<SimpleText*>(mTrayMgr->getWidget("FormModels/Text/Title"));
		SimpleText* stDesc = dynamic_cast<SimpleText*>(mTrayMgr->getWidget("FormModels/Text/Desc"));
		leName->setText(stTitle ? stTitle->getText() : Ogre::StringUtil::BLANK);
		leDesc->setText(stDesc ? stDesc->getText() : Ogre::StringUtil::BLANK);
	}
}

void FormModels::hideOptionEditPosition()
{
	if (mTrayMgr->getWidget("FormModels/Window/EditPosition"))
	{
		mTrayMgr->destroyDialogWindow("FormModels/Window/EditPosition");
		mTrayMgr->destroyWidget("FormModels/SimpleText/EditPosition");
		mTrayMgr->destroyWidget("FormModels/Button/CloseOptionsEditPosition");
		mTrayMgr->destroyWidget("FormModels/Button/WindowsEditPositionHide");
		mTrayMgr->destroyWidget("FormModels/Button/EditPositionReset");
	}
}

void FormModels::showOptionEditPosition()
{
	if (!mTrayMgr->getWidget("FormModels/Window/EditPosition"))
	{
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 420;
		Ogre::String descCamera = 
			"To modify the model you must click and drag with the left mouse button. "
			"The keys G, R and S control the actions, translation, rotation and scale respectively, while the keys X, Y and Z control the axis.";
		SimpleText* desc = mTrayMgr->createSimpleText("FormModels/SimpleText/EditPosition", descCamera, "YgcFont/SemiBold/16", width - 30, 250, 19, 9);
		Ogre::Real height = desc->getNumLines() * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;

		mTrayMgr->createDialogWindow("FormModels/Window/EditPosition", "EDIT CAMERA POSITION", left, top, width, height);		top += sepOptions / 2;
		desc->setLeft(left + 12); desc->setTop(top); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormModels/Button/CloseOptionsEditPosition", "ACCEPT", left, top, 65);
		mTrayMgr->createButton("FormModels/Button/WindowsEditPositionHide", "HIDE", left + 70, top, 60);
		mTrayMgr->createButton("FormModels/Button/EditPositionReset", "RESET", left + 70 + 65, top, 65);
	}
}

void FormModels::hideOptionViews()
{
	if (mTrayMgr->getWidget("FormModels/Window/ViewsOptions"))
	{
		mTrayMgr->destroyDialogWindow("FormModels/Window/ViewsOptions");
		mTrayMgr->destroyWidget("FormModels/Selector/Views");
		mTrayMgr->destroyWidget("FormModels/Selector/Dof");
		mTrayMgr->destroyWidget("FormModels/Slider/Focus");
		mTrayMgr->destroyWidget("FormModels/Label/NewView");
		mTrayMgr->destroyWidget("FormModels/Label/DeleteView");
		mTrayMgr->destroyWidget("FormModels/Label/EditCamera");
		mTrayMgr->destroyWidget("FormModels/Label/EditTarget");
		mTrayMgr->destroyWidget("FormModels/Button/CloseOptionsViews");
	}
}

void FormModels::showOptionViews(sModel& model)
{
	if (!mTrayMgr->getWidget("FormModels/Window/ViewsOptions"))
	{
		unsigned int numOptions = 8;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 420;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		Ogre::StringVector itemsEnable; itemsEnable.push_back("Enable"); itemsEnable.push_back("Disable");
		Ogre::StringVector itemsViews;
		mTrayMgr->createDialogWindow("FormModels/Window/ViewsOptions", "EDIT VIEWS", left, top, width, height);		top += sepOptions / 2;
		mTrayMgr->createItemSelector("FormModels/Selector/Views", "Current view", itemsViews, left, top, width);		top += sepOptions;
		mTrayMgr->createItemSelector("FormModels/Selector/Dof", "Depth of field", itemsEnable, left, top, width);	top += sepOptions;
		mTrayMgr->createSliderOptions("FormModels/Slider/Focus", "Focus", left, top, width, 0, 10, 100);				top += sepOptions;
		mTrayMgr->createLabel("FormModels/Label/NewView", "NEW VIEW", left, top, width, 23);							top += sepOptions;
		mTrayMgr->createLabel("FormModels/Label/DeleteView", "DELETE VIEW", left, top, width, 23);					top += sepOptions;
		mTrayMgr->createLabel("FormModels/Label/EditCamera", "CAMERA POSITION", left, top, width, 23);				top += sepOptions;
		mTrayMgr->createLabel("FormModels/Label/EditTarget", "TARGET POSITION", left, top, width, 23);				top = mScreenSize.y - sepWindow - sepButton; 
		mTrayMgr->createButton("FormModels/Button/CloseOptionsViews", "BACK", left, top, 60);
		_loadConfigWindowsViews(model);
	}
}

void FormModels::hideOptionCamera()
{
	if (mTrayMgr->getWidget("FormModels/Window/EditCamera"))
	{
		mTrayMgr->destroyDialogWindow("FormModels/Window/EditCamera");
		mTrayMgr->destroyWidget("Desc/EditCamera");
		mTrayMgr->destroyWidget("FormModels/Button/CloseOptionsEditCamera");
	}
}

void FormModels::showOptionCamera()
{
	if (!mTrayMgr->getWidget("FormModels/Window/EditCamera"))
	{
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 420;
		Ogre::String descCamera = "Modify the camera position click and dragging the left mouse button and pressing left alt key. Press left alt and left ctrl keys to make zoom to object.";
		SimpleText* desc = mTrayMgr->createSimpleText("Desc/EditCamera", descCamera, "YgcFont/SemiBold/16", width - 30, 250, 19, 9);
		Ogre::Real height = desc->getNumLines() * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;

		mTrayMgr->createDialogWindow("FormModels/Window/EditCamera", "EDIT CAMERA POSITION", left, top, width, height);		top += sepOptions / 2;
		desc->setLeft(left + 12); desc->setTop(top); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormModels/Button/CloseOptionsEditCamera", "ACCEPT", left, top, 60);
	}
}

void FormModels::hideOptionTarget()
{
	if (mTrayMgr->getWidget("FormModels/Window/EditTarget"))
	{
		mTrayMgr->destroyDialogWindow("FormModels/Window/EditTarget");
		mTrayMgr->destroyWidget("Desc/EditTarget");
		mTrayMgr->destroyWidget("FormModels/Button/CloseOptionsEditTarget");
	}
}

void FormModels::showOptionTarget()
{
	if (!mTrayMgr->getWidget("FormModels/Window/EditTarget"))
	{
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 450;
		Ogre::String descTarget = "To modify the target you must click and drag with the left mouse button. The keys G, R and S control the actions, translation, rotation and scale respectively, while the keys X, Y and Z control the axis.";
		SimpleText* desc = mTrayMgr->createSimpleText("Desc/EditTarget", descTarget, "YgcFont/SemiBold/16", width - 30, 250, 19, 9);
		Ogre::Real height = desc->getNumLines() * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;

		mTrayMgr->createDialogWindow("FormModels/Window/EditTarget", "EDIT TARGET POSITION", left, top, width, height);		top += sepOptions / 2;
		desc->setLeft(left + 12); desc->setTop(top + 12); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormModels/Button/CloseOptionsEditTarget", "ACCEPT", left, top, 60);
	}
}

