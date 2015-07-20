#include "stdafx.h"
#include "FormCollector.h"

FormCollector::FormCollector(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener /*= 0*/) :
FormBase(tray, oldListener),
mGameInfo(gInfo),
mCameraMan(0),
mTarget(mSceneMgr->getRootSceneNode()->createChildSceneNode()),
mController(0),
mPathIni(Ogre::StringUtil::BLANK),
mCurrentIndex(0),
mEditCamera(false)
{
	// find the .ini config file
	sInfoResource infoIni;
	mGameInfo->findIniCollector(infoIni);
	mPathIni = infoIni.path;

	_loadCollection(mPathIni);
	_setCameraCollection();
}

FormCollector::~FormCollector()
{
	// manual remove widgets
	hideOptions();
	hideOptionObject(); 
	hideOptionsCamera();

	for (unsigned int i = 0; i < mModels.size(); ++i)
		delete mModels[i].cmodel;

	if (mController) delete mController;
	if (mCameraMan) delete mCameraMan;
	if (mTarget) mSceneMgr->destroySceneNode(mTarget);
}



bool FormCollector::keyPressed(const OIS::KeyEvent &arg)
{
	if (mController) mController->injectKeyDown(arg);

	if (arg.key == OIS::KC_E)
	{
		_editModel(mCurrentIndex);
	}
	else if (arg.key == OIS::KC_TAB)
	{
		mCurrentIndex = (mCurrentIndex == mModels.size() - 1) ? 0 : mCurrentIndex + 1;
		_editModel(mCurrentIndex);
	}
	else if (arg.key == OIS::KC_RETURN)
	{
		if (mController) delete mController;
		mController = 0;
		_saveCollection(mPathIni);
	}

	return FormBase::keyPressed(arg);
}

bool FormCollector::keyReleased(const OIS::KeyEvent &arg)
{
	if (mController) mController->injectKeyUp(arg);
	
	return FormBase::keyReleased(arg);
}

bool FormCollector::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;
	if (mCameraMan && mEditCamera) mCameraMan->injectMouseMove(arg);
	if (mController) mController->injectMouseMove(arg);
	
	return FormBase::mouseMoved(arg);;
}

bool FormCollector::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;
	if (mCameraMan && mEditCamera) mCameraMan->injectMouseDown(arg, id);
	if (mController) mController->injectMouseDown(arg, id);

	return FormBase::mousePressed(arg, id);;
}

bool FormCollector::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;
	if (mCameraMan && mEditCamera) mCameraMan->injectMouseUp(arg, id);
	if (mController) mController->injectMouseUp(arg, id);

	return FormBase::mouseReleased(arg, id);;
}



void FormCollector::buttonHit(Button* button)
{
	/*-- General options ------------------------------------------------------------------*/
	if (button->getName() == "FormCollector/Button/EditObjects")
	{
		_editModel(mCurrentIndex);
		hideOptions();
		showOptionObject();
	}
	else if (button->getName() == "FormCollector/Button/EditCamera")
	{
		hideOptions();
		showOptionsCamera();
	}
	else if (button->getName() == "FormCollector/Button/CloseOptions")
	{
		hideOptions();
	}
	/*-- Edit camera options ------------------------------------------------------------------*/
	else if (button->getName() == "FormCollector/Button/CameraPos")
	{
		if (mController) delete mController;
		mController = 0;
		mEditCamera = true;
	}
	else if (button->getName() == "FormCollector/Button/CameraTarget")
	{
		if (!mController)
		{
			mController = new CModelController(mTarget);
		}
		mEditCamera = false;
	}
	else if (button->getName() == "FormCollector/Button/ResetCamera")
	{
		resetCamera();
		mTarget->setPosition(Ogre::Vector3(0, 10, 30));
		mCameraMan->setTarget(mTarget);
		mCameraMan->setYawPitchDist(Ogre::Degree(0), Ogre::Degree(5), 35);
		mEditCamera = false;
	}
	else if (button->getName() == "FormCollector/Button/CloseEditCamera")
	{
		_saveCollection(mPathIni);
		hideOptionsCamera();
		if (mController) delete mController;
		mController = 0;
		mEditCamera = false;
	}
	/*-- Edit object options ------------------------------------------------------------------*/
	else if (button->getName() == "FormCollector/Button/AddObject")
	{

	}
	else if (button->getName() == "FormCollector/Button/RemoveObject")
	{
		_removeObject(mCurrentIndex);
		mCurrentIndex = 0;
		_editModel(mCurrentIndex);
	}
	else if (button->getName() == "FormCollector/Button/NextObject")
	{
		mCurrentIndex = (mCurrentIndex == mModels.size() - 1) ? 0 : mCurrentIndex + 1;
		_editModel(mCurrentIndex);
	}
	else if (button->getName() == "FormCollector/Button/PreviousObject")
	{
		mCurrentIndex = (mCurrentIndex == 0) ? mModels.size() - 1 : mCurrentIndex - 1;
		_editModel(mCurrentIndex);
	}
	else if (button->getName() == "FormCollector/Button/ResetObject")
	{
		_resetObject(mCurrentIndex);
	}
	else if (button->getName() == "FormCollector/Button/CloseEditObjects")
	{
		_saveCollection(mPathIni);
		hideOptionObject();
		if (mController) delete mController;
		mController = 0;
	}
}



void FormCollector::hide()
{
	FormBase::hide();
	for (unsigned int i = 0; i < mModels.size(); ++i)
		mModels[i].cmodel->hide();
}

void FormCollector::show()
{
	FormBase::hide();
	for (unsigned int i = 0; i < mModels.size(); ++i)
		mModels[i].cmodel->show();
	_setCameraCollection();
}



void FormCollector::_setCameraCollection()
{
	resetCamera();
	mTarget->setPosition(mSceneView.targetPos);
	if (!mCameraMan)
		mCameraMan = new OgreBites::SdkCameraMan(mCamera);
	mCameraMan->setStyle(OgreBites::CS_ORBIT);
	mCameraMan->setTarget(mTarget);
	mCameraMan->setYawPitchDist(Ogre::Degree(0), Ogre::Degree(5), 35);

	// apply the camera properties saved
	mNodeCamera->setPosition(0, 0, 0);
	mCamera->setPosition(mSceneView.cameraPos);
	mCamera->setOrientation(mSceneView.cameraRot);
}

void FormCollector::_editModel(unsigned int index)
{
	if (index >= 0 && index < mModels.size())
	{
		if (mController) delete mController;
		mController = new CModelController(mModels[index].cmodel->getNode(), 5);
	}
}

void FormCollector::_removeObject(unsigned int index)
{
	if (index >= 0 && index < mModels.size())
	{
		if (mController) delete mController;
		mController = 0;
		delete mModels[index].cmodel;
		mModels.erase(mModels.begin() + index);
	}
}

void FormCollector::_resetObject(unsigned int index)
{
	if (index >= 0 && index < mModels.size())
	{
		mModels[index].cmodel->getNode()->setPosition(0, 0, 0);
		mModels[index].cmodel->getNode()->setOrientation(1, 0, 0, 0);
		mModels[index].cmodel->getNode()->setScale(1, 1, 1);
	}
}

bool FormCollector::_loadCollection(const Ogre::String& pathIni)
{
	if (boost::filesystem::is_regular_file(pathIni))
	{
		CSimpleIniA* simpleIni = new CSimpleIniA;
		SI_Error iniStatus = simpleIni->LoadFile(pathIni.c_str());
		if (iniStatus == SI_OK)
		{
			// get all sections 
			CSimpleIniA::TNamesDepend sections;
			simpleIni->GetAllSections(sections);
			CSimpleIniA::TNamesDepend::const_iterator i;
			for (i = sections.begin(); i != sections.end(); ++i)
			{
				Ogre::String sectionName = i->pItem;
				if (sectionName.find("COLLECTOR.OPTIONS") != Ogre::String::npos)
				{
					mSceneView.cameraPos = Ogre::Vector3(
						simpleIni->GetDoubleValue(sectionName.c_str(), "Camera_Pos_x", 0),
						simpleIni->GetDoubleValue(sectionName.c_str(), "Camera_Pos_y", 0),
						simpleIni->GetDoubleValue(sectionName.c_str(), "Camera_Pos_z", 0));
					mSceneView.cameraRot = Ogre::Quaternion(
						simpleIni->GetDoubleValue(sectionName.c_str(), "Camera_Rot_w", 1),
						simpleIni->GetDoubleValue(sectionName.c_str(), "Camera_Rot_x", 0),
						simpleIni->GetDoubleValue(sectionName.c_str(), "Camera_Rot_y", 0),
						simpleIni->GetDoubleValue(sectionName.c_str(), "Camera_Rot_z", 0));
					mSceneView.targetPos = Ogre::Vector3(
						simpleIni->GetDoubleValue(sectionName.c_str(), "Target_Pos_x", 0),
						simpleIni->GetDoubleValue(sectionName.c_str(), "Target_Pos_y", 0),
						simpleIni->GetDoubleValue(sectionName.c_str(), "Target_Pos_z", 0));
					mSceneView.dofEnable = simpleIni->GetBoolValue(sectionName.c_str(), "Dof_Effect", false);
					mSceneView.dofFocus = simpleIni->GetDoubleValue(sectionName.c_str(), "Dof_Focus", 2);
				}
				else if (sectionName.find("COLLECTOR.OBJECT.") != Ogre::String::npos)
				{
					mModels.push_back(sCollectorObject());
					mModels.back().meshName = simpleIni->GetValue(sectionName.c_str(), "Mesh", "0");
					mModels.back().textureName = simpleIni->GetValue(sectionName.c_str(), "Texture", "0");
					mModels.back().cmodel = new CModel(mModels.back().meshName);
					_loadModelStatusFromIni(simpleIni, sectionName, mModels.back().cmodel);
					if (mModels.back().textureName != "0")
					{
						Ogre::String pathTexture = mGameInfo->getPathGame() + "/Models/Collectors Edition/" + mModels.back().textureName;
						Ogre::String nameTexture = "FormCollector/Texture/Object" + Ogre::StringConverter::toString(mModels.size());
						GameInfo::loadImageFromDisk(pathTexture, nameTexture, mGameInfo->getGroupName(), 2);
						mModels.back().cmodel->applyNewText(nameTexture, 0);
					}
				}
			}
			delete simpleIni;
			return true; // ok
		}
		return false; // error
	}
	return false; // error
}

bool FormCollector::_saveCollection(const Ogre::String& pathIni)
{
	if (boost::filesystem::is_regular_file(pathIni))
	{
		CSimpleIniA* simpleIni = new CSimpleIniA;
		SI_Error iniStatus = simpleIni->LoadFile(pathIni.c_str());
		simpleIni->Reset(); // remove the previous content

		if (iniStatus == SI_OK)
		{
			Ogre::String sectionName = "COLLECTOR.OPTIONS";
			mSceneView.cameraPos = mCamera->getPosition();
			mSceneView.cameraRot = mCamera->getOrientation();
			mSceneView.targetPos = mTarget->getPosition();
			mSceneView.dofEnable = mTrayMgr->getDofEffectEnable();
			mSceneView.dofFocus = mTrayMgr->getDofEffectFocus();
			simpleIni->SetDoubleValue(sectionName.c_str(), "Camera_Pos_x", mSceneView.cameraPos.x);
			simpleIni->SetDoubleValue(sectionName.c_str(), "Camera_Pos_y", mSceneView.cameraPos.y);
			simpleIni->SetDoubleValue(sectionName.c_str(), "Camera_Pos_z", mSceneView.cameraPos.z);
			simpleIni->SetDoubleValue(sectionName.c_str(), "Camera_Rot_w", mSceneView.cameraRot.w);
			simpleIni->SetDoubleValue(sectionName.c_str(), "Camera_Rot_x", mSceneView.cameraRot.x);
			simpleIni->SetDoubleValue(sectionName.c_str(), "Camera_Rot_y", mSceneView.cameraRot.y);
			simpleIni->SetDoubleValue(sectionName.c_str(), "Camera_Rot_z", mSceneView.cameraRot.z);
			simpleIni->SetDoubleValue(sectionName.c_str(), "Target_Pos_x", mSceneView.targetPos.x);
			simpleIni->SetDoubleValue(sectionName.c_str(), "Target_Pos_y", mSceneView.targetPos.y);
			simpleIni->SetDoubleValue(sectionName.c_str(), "Target_Pos_z", mSceneView.targetPos.z);
			simpleIni->SetBoolValue(sectionName.c_str(), "Dof_Effect", mSceneView.dofEnable);
			simpleIni->SetDoubleValue(sectionName.c_str(), "Dof_Focus", mSceneView.dofFocus);

			for (unsigned int i = 0; i < mModels.size(); ++i)
			{
				sectionName = "COLLECTOR.OBJECT." + Ogre::StringConverter::toString(i);
				
				simpleIni->SetValue(sectionName.c_str(), "Mesh", mModels[i].meshName.c_str());
				simpleIni->SetValue(sectionName.c_str(), "Texture", mModels[i].textureName.c_str());
				_saveModelStatusFromIni(simpleIni, sectionName, mModels[i].cmodel, pathIni);
			}
			delete simpleIni;
			return true; // ok
		}
		return false; // error
	}
	return false; // error
}



void FormCollector::showOptions()
{
	if (!mTrayMgr->getWidget("FormCollector/Button/AddObject") &&
		!mTrayMgr->getWidget("FormCollector/Button/CameraPos")) // submenus are hidden?
	{
		if (!mTrayMgr->getWidget("FormCollector/Button/EditObjects")) // is visible? 
		{
			//topButton=screenHeight - (num_buttons*sep_buttons) - set_buttons;
			Ogre::Real sizeButton = 170, leftButton = 35, topButton = 0, sepButton = 40;
			topButton = mScreenSize.y - (3 * sepButton) - sepButton;

			mTrayMgr->createButton("FormCollector/Button/EditObjects", "Edit Objects", leftButton, topButton, sizeButton);
			topButton += sepButton;
			mTrayMgr->createButton("FormCollector/Button/EditCamera", "Edit Camera", leftButton, topButton, sizeButton);
			topButton += sepButton;
			mTrayMgr->createButton("FormCollector/Button/CloseOptions", "Close", leftButton, topButton, sizeButton);
		}
	}
}

void FormCollector::hideOptions()
{
	if (mTrayMgr->getWidget("FormCollector/Button/EditObjects"))
	{
		mTrayMgr->destroyWidget("FormCollector/Button/EditObjects");
		mTrayMgr->destroyWidget("FormCollector/Button/EditCamera");
		mTrayMgr->destroyWidget("FormCollector/Button/CloseOptions");
	}
}

void FormCollector::showOptionObject()
{
	if (!mTrayMgr->getWidget("FormCollector/Button/AddObject"))
	{
		Ogre::Real sizeButton = 170, leftButton = 35, topButton = 0, sepButton = 40;
		topButton = mScreenSize.y - (6 * sepButton) - sepButton;

		mTrayMgr->createButton("FormCollector/Button/AddObject", "New Object", leftButton, topButton, sizeButton);
		topButton += sepButton;
		mTrayMgr->createButton("FormCollector/Button/RemoveObject", "Remove Object", leftButton, topButton, sizeButton);
		topButton += sepButton;
		mTrayMgr->createButton("FormCollector/Button/NextObject", "Next Object", leftButton, topButton, sizeButton);
		topButton += sepButton;
		mTrayMgr->createButton("FormCollector/Button/PreviousObject", "Previous Object", leftButton, topButton, sizeButton);
		topButton += sepButton;
		mTrayMgr->createButton("FormCollector/Button/ResetObject", "Reset Object", leftButton, topButton, sizeButton);
		topButton += sepButton;
		mTrayMgr->createButton("FormCollector/Button/CloseEditObjects", "Close", leftButton, topButton, sizeButton);
	}
}

void FormCollector::hideOptionObject()
{
	if (mTrayMgr->getWidget("FormCollector/Button/AddObject"))
	{
		mTrayMgr->destroyWidget("FormCollector/Button/AddObject");
		mTrayMgr->destroyWidget("FormCollector/Button/RemoveObject");
		mTrayMgr->destroyWidget("FormCollector/Button/NextObject");
		mTrayMgr->destroyWidget("FormCollector/Button/PreviousObject");
		mTrayMgr->destroyWidget("FormCollector/Button/ResetObject");
		mTrayMgr->destroyWidget("FormCollector/Button/CloseEditObjects");
	}
}

void FormCollector::showOptionsCamera()
{
	if (!mTrayMgr->getWidget("FormCollector/Button/CameraPos"))
	{
		Ogre::Real sizeButton = 170, leftButton = 35, topButton = 0, sepButton = 40;
		topButton = mScreenSize.y - (4 * sepButton) - sepButton;

		mTrayMgr->createButton("FormCollector/Button/CameraPos", "Camera Position", leftButton, topButton, sizeButton);
		topButton += sepButton;
		mTrayMgr->createButton("FormCollector/Button/CameraTarget", "Camera Target", leftButton, topButton, sizeButton);
		topButton += sepButton;
		mTrayMgr->createButton("FormCollector/Button/ResetCamera", "Reset Camera", leftButton, topButton, sizeButton);
		topButton += sepButton;
		mTrayMgr->createButton("FormCollector/Button/CloseEditCamera", "Close", leftButton, topButton, sizeButton);
	}
}

void FormCollector::hideOptionsCamera()
{
	if (mTrayMgr->getWidget("FormCollector/Button/CameraPos"))
	{
		mTrayMgr->destroyWidget("FormCollector/Button/CameraPos");
		mTrayMgr->destroyWidget("FormCollector/Button/CameraTarget");
		mTrayMgr->destroyWidget("FormCollector/Button/ResetCamera");
		mTrayMgr->destroyWidget("FormCollector/Button/CloseEditCamera");
	}
}

