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
mCtrlPressed(false), mAltPressed(false), mShiftPressed(false)
{
	// find textures and load them
	mGameInfo->findTexturesCollector(mTextures);
	for (unsigned int i = 0; i < mTextures.size(); ++i)
		GameInfo::loadImageFromDisk(mTextures[i].path, mTextures[i].nameThumb, mGameInfo->getGroupName(), 0);

	// find models [.zip]
	mGameInfo->findModelsCollector(mZipModels);

	// add ygc models 
	mYgcModels.push_back("Dvd_Case.mesh"); 
	mYgcModels.push_back("mesh_CD.mesh");
	mYgcModels.push_back("mesh_openDvd.mesh");

	// find the .ini config file
	sInfoResource infoIni;
	mGameInfo->findIniCollector(infoIni);
	mPathIni = infoIni.path;

	// if not exist a .ini config file collector, then create one
	if (!boost::filesystem::is_regular_file(mPathIni))
	{
		mPathIni = mGameInfo->getPathGame() + "/Models/Collectors Edition/Collector.ini";
		CSimpleIniA iniFile;
		iniFile.SaveFile(mPathIni.c_str());
		_resetCameraCollection();
		_saveCollection(mPathIni);
	}
	_loadCollection(mPathIni);
	_setCameraCollection();
}

FormCollector::~FormCollector()
{
	// manual remove widgets
	hideAllOptions();

	if (mController) delete mController;
	if (mCameraMan) delete mCameraMan;
	if (mTarget) mSceneMgr->destroySceneNode(mTarget);

	for (unsigned int i = 0; i < mModels.size(); ++i)
		delete mModels[i].cmodel;
}



bool FormCollector::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_LCONTROL)	mCtrlPressed = true;
	else if (arg.key == OIS::KC_LMENU)	mAltPressed = true;
	else if (arg.key == OIS::KC_LSHIFT)	mShiftPressed = true;

	if (mController) mController->injectKeyDown(arg);

	return FormBase::keyPressed(arg);
}

bool FormCollector::keyReleased(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_LCONTROL)	mCtrlPressed = false;
	else if (arg.key == OIS::KC_LMENU)	mAltPressed = false;
	else if (arg.key == OIS::KC_LSHIFT)	mShiftPressed = false;
	else if (arg.key == OIS::KC_C) _setCameraCollection();

	if (mController) mController->injectKeyUp(arg);
	
	return FormBase::keyReleased(arg);
}

bool FormCollector::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;
	
	if (mController)
		mController->injectMouseMove(arg);
	
	if (mLBPressed) // left button 
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
				Ogre::Real dist = (mCamera->getPosition() - mTarget->getPosition()).length();
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

	return FormBase::mouseMoved(arg);;
}

bool FormCollector::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	if (mCameraMan && mAltPressed) 
		mCameraMan->injectMouseDown(arg, id);

	if (mController) 
		mController->injectMouseDown(arg, id);

	return FormBase::mousePressed(arg, id);;
}

bool FormCollector::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;
	
	if (mCameraMan && mAltPressed)
		mCameraMan->injectMouseUp(arg, id);

	if (mController) 
		mController->injectMouseUp(arg, id);

	return FormBase::mouseReleased(arg, id);;
}



void FormCollector::buttonHit(Button* button)
{
	if (button->getName() == "FormCollector/Button/CloseOptions")
	{
		hideOptions();
	}
	else if (button->getName() == "FormCollector/Button/CloseOptionsItems")
	{
		_saveCollection(mPathIni);
		hideOptionsItems();
		showOptions();
		if (!mModels.empty()) mModels[mCurrentIndex].cmodel->getNode()->showBoundingBox(false);
	}
	else if (button->getName() == "FormCollector/Button/CloseOptionsCamera")
	{
		_saveCollection(mPathIni);
		hideOptionsCamera();
		showOptions();
		if (mController) delete mController;
		mController = 0;
	}
	else if (button->getName() == "FormCollector/Button/CloseOptionsEditPosition")
	{
		if (mController) delete mController;
		mController = 0;
		if (!mModels.empty()) mModels[mCurrentIndex].cmodel->getNode()->showBoundingBox(true);
		hideOptionEditPosition();
		showOptionsItems();
	}
	else if (button->getName() == "FormCollector/Button/WindowsEditPositionHide")
	{
		if (mTrayMgr->getWidget("FormCollector/Window/EditPosition")->isVisible())
		{
			mTrayMgr->getWidget("FormCollector/Window/EditPosition")->hide();
			mTrayMgr->getWidget("FormCollector/SimpleText/EditPosition")->hide();
			Button* bttHide = dynamic_cast<Button*>(mTrayMgr->getWidget("FormCollector/Button/WindowsEditPositionHide"));
			bttHide->setCaption("SHOW");
		}
		else
		{
			mTrayMgr->getWidget("FormCollector/Window/EditPosition")->show();
			mTrayMgr->getWidget("FormCollector/SimpleText/EditPosition")->show();
			Button* bttHide = dynamic_cast<Button*>(mTrayMgr->getWidget("FormCollector/Button/WindowsEditPositionHide"));
			bttHide->setCaption("HIDE");
		}
	}
	else if (button->getName() == "FormCollector/Button/EditPositionReset")
	{
		_resetItem(mCurrentIndex);
	}
}

void FormCollector::labelHit(Label* label)
{
	if (label->getName() == "FormCollector/Label/EditItems")
	{
		hideOptions();
		showOptionsItems();
	}
	else if (label->getName() == "FormCollector/Label/EditCamera")
	{
		hideOptions();
		showOptionsCamera();
	}
	/*-- Edit camera options ------------------------------------------------------------------*/
	else if (label->getName() == "FormCollector/Label/CameraPosition")
	{
		if (mController) delete mController;
		mController = 0;		
	}
	else if (label->getName() == "FormCollector/Label/CameraTarget")
	{
		if (mController) delete mController;
		mController = new CModelController(mTarget, 3);
	}
	else if (label->getName() == "FormCollector/Label/CameraReset")
	{
		if (mController) delete mController;
		mController = 0;
		_resetCameraCollection();
	}
	/*-- edit items options ------------------------------------------------------------------*/
	else if (label->getName() == "FormCollector/Label/NewItem")
	{
		_newItem();
		_loadConfigWindowsItem(mCurrentIndex, true);
	}
	else if (label->getName() == "FormCollector/Label/ItemPosition")
	{
		if (!mModels.empty())
		{
			_editItem(mCurrentIndex);
			hideOptionsItems();
			showOptionEditPosition();
		}
	}
	else if (label->getName() == "FormCollector/Label/DeleteItem")
	{
		_removeItem(mCurrentIndex);
		_loadConfigWindowsItem(mCurrentIndex);
	}
}

void FormCollector::itemChanged(ItemSelector* selector)
{
	if (selector->getName() == "FormCollector/Selector/Items")
	{
		mCurrentIndex = Ogre::StringConverter::parseInt(selector->getSelectedOption());
		_loadConfigWindowsItem(mCurrentIndex);
	}
	else if (selector->getName() == "FormCollector/Selector/Models")
	{
		if (!mModels.empty())
		{
			Ogre::Vector3 posModel = mTarget->getPosition();
			delete mModels[mCurrentIndex].cmodel;
			mModels[mCurrentIndex].meshName = selector->getSelectedOption();
			mModels[mCurrentIndex].cmodel = new CModel(selector->getSelectedOption());
			mModels[mCurrentIndex].cmodel->getNode()->setPosition(posModel.x, 0, posModel.z);
			mModels[mCurrentIndex].textureName.clear();
			for (unsigned int i = 0; i < mModels[mCurrentIndex].cmodel->getEntity()->getNumSubEntities(); ++i)
				mModels[mCurrentIndex].textureName.push_back("No texture");
			mModels[mCurrentIndex].isYgcModel = (selector->getSelectedIndex() < mYgcModels.size()) ? true : false;

			_loadConfigWindowsItem(mCurrentIndex);
		}
	}
	else if (selector->getName() == "FormCollector/Selector/TextName")
	{
		ItemSelector* selSubmesh = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormCollector/Selector/SubMesh"));
		unsigned int subMesh = Ogre::StringConverter::parseInt(selSubmesh->getSelectedOption());
		mModels[mCurrentIndex].textureName[subMesh] = selector->getSelectedOption();

		if (selector->getSelectedOption() == "No texture")
		{
			mModels[mCurrentIndex].cmodel->restoreMaterial(subMesh);
		}
		else
		{
			int indexSelected = selector->getSelectedIndex();
			indexSelected--; // "No texture" option is the first element on 'mTextures'
			mModels[mCurrentIndex].cmodel->applyNewText(mTextures[indexSelected].nameThumb, subMesh);
		}
	}
}



void FormCollector::hide()
{
	hideAllOptions();
	FormBase::hide();
	for (unsigned int i = 0; i < mModels.size(); ++i)
		mModels[i].cmodel->hide();
	Widget* logoGame = mTrayMgr->getWidget("FormOverview/Logo");
	if (logoGame) logoGame->hide();
}

void FormCollector::show()
{
	FormBase::hide();
	for (unsigned int i = 0; i < mModels.size(); ++i)
		mModels[i].cmodel->show();
	_setCameraCollection();
	Widget* logoGame = mTrayMgr->getWidget("FormOverview/Logo");
	if (logoGame) logoGame->show();
}



void FormCollector::_resetCameraCollection()
{
	resetCamera();
	mTarget->setPosition(Ogre::Vector3(0, 15, -30));
	if (mCameraMan) delete mCameraMan;
	mCameraMan = new OgreBites::SdkCameraMan(mCamera);
	mCameraMan->setStyle(OgreBites::CS_ORBIT);
	mCameraMan->setTarget(mTarget);
	mCameraMan->setYawPitchDist(Ogre::Degree(0), Ogre::Degree(5), 55);
}

void FormCollector::_setCameraCollection()
{
	resetCamera();
	mTarget->setPosition(mSceneView.targetPos);
	if (mCameraMan) delete mCameraMan;
	mCameraMan = new OgreBites::SdkCameraMan(mCamera);
	mCameraMan->setStyle(OgreBites::CS_ORBIT);
	mCameraMan->setTarget(mTarget);
	mCameraMan->setYawPitchDist(Ogre::Degree(0), Ogre::Degree(5), 55);

	// apply the camera properties saved
	mCamera->setPosition(mSceneView.cameraPos);
	mCamera->setOrientation(mSceneView.cameraRot);
}



void FormCollector::_newItem()
{
	ItemSelector* selModels = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormCollector/Selector/Models"));
	Ogre::Vector3 posModel = mTarget->getPosition();
	mModels.push_back(sCollectorObject());
	mModels.back().meshName = mYgcModels.front();
	mModels.back().cmodel = new CModel(mModels.back().meshName);
	mModels.back().cmodel->getNode()->setPosition(posModel.x, 0, posModel.z);
	for (unsigned int i = 0; i < mModels.back().cmodel->getEntity()->getNumSubEntities(); ++i)
		mModels.back().textureName.push_back("No texture");
	mModels.back().isYgcModel = true;

	mCurrentIndex = mModels.size() - 1;
	_loadConfigWindowsItem(mCurrentIndex, true);
}

void FormCollector::_removeItem(unsigned int index)
{
	if (index >= 0 && index < mModels.size())
	{
		if (mController) delete mController;
		mController = 0;
		delete mModels[index].cmodel;
		mModels.erase(mModels.begin() + index);

		mCurrentIndex = 0;
	}
}

void FormCollector::_editItem(unsigned int index)
{
	if (index >= 0 && index < mModels.size())
	{
		if (mController) delete mController;
		mController = new CModelController(mModels[index].cmodel->getNode(), 3);
	}
}

void FormCollector::_resetItem(unsigned int index)
{
	if (index >= 0 && index < mModels.size())
	{
		mModels[index].cmodel->getNode()->setPosition(0, 0, -30);
		mModels[index].cmodel->getNode()->setOrientation(Ogre::Quaternion());
		mModels[index].cmodel->getNode()->setScale(1, 1, 1);
		_editItem(index);
	}
}



void FormCollector::_loadConfigWindowsItem(unsigned int currentModel, bool newModel /*= false*/)
{
	ItemSelector* selItems = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormCollector/Selector/Items"));
	ItemSelector* selModels = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormCollector/Selector/Models"));
	ItemSelector* selSubmesh = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormCollector/Selector/SubMesh"));
	ItemSelector* selTexture = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormCollector/Selector/TextName"));

	Ogre::StringVector noItems;		noItems.push_back("No items");
	Ogre::StringVector noSubMesh;	noSubMesh.push_back("No submesh");
	Ogre::StringVector noTexture;	noTexture.push_back("No texture");

	Ogre::StringVector itemsCollector;
	for (unsigned int i = 0; i < mModels.size(); ++i)
	{
		itemsCollector.push_back(Ogre::StringConverter::toString(i));
		mModels[i].cmodel->getNode()->showBoundingBox(false);
	}
	Ogre::StringVector itemsModels;
	for (unsigned int i = 0; i < mYgcModels.size(); ++i)
		itemsModels.push_back(mYgcModels[i]);
	for (unsigned int i = 0; i < mZipModels.size(); ++i)
		itemsModels.push_back(mZipModels[i].caption + ".mesh");
	Ogre::StringVector itemsSubEnt;
	if (!mModels.empty())
		for (unsigned int i = 0; i < mModels[mCurrentIndex].cmodel->getEntity()->getNumSubEntities(); i++)
			itemsSubEnt.push_back(Ogre::StringConverter::toString(i));
	Ogre::StringVector itemTextures;
	itemTextures.push_back("No texture");
	for (unsigned int i = 0; i < mTextures.size(); ++i)
		itemTextures.push_back(mTextures[i].filename);

	selSubmesh->setItems(itemsSubEnt);
	selItems->setItems(itemsCollector);
	selModels->setItems(itemsModels);
	selTexture->setItems(itemTextures);

	// new item
	if (newModel)
	{
		selItems->selectOption(Ogre::StringConverter::toString(currentModel), false);
		mModels[currentModel].cmodel->getNode()->showBoundingBox(true);
	}
	// empty collection - show default view
	else if (mModels.empty())
	{
		selItems->setItems(noItems);
		selSubmesh->setItems(noSubMesh);
		selTexture->setItems(noTexture);
	}
	// zip model?
	else if (!mModels[currentModel].isYgcModel)
	{
		selItems->selectOption(Ogre::StringConverter::toString(currentModel), false);
		selModels->selectOption(mModels[currentModel].meshName, false);
		selSubmesh->setItems(noSubMesh);
		selTexture->setItems(noTexture);
		mModels[currentModel].cmodel->getNode()->showBoundingBox(true);
	}
	// show the current item
	else
	{
		selItems->selectOption(Ogre::StringConverter::toString(currentModel), false);
		selModels->selectOption(mModels[currentModel].meshName, false);
		selTexture->selectOption(mModels[currentModel].textureName[0], false);
		mModels[currentModel].cmodel->getNode()->showBoundingBox(true);
	}
}



bool FormCollector::_loadCollection(const Ogre::String& pathIni)
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
				mModels.back().cmodel = new CModel(mModels.back().meshName);
				mModels.back().isYgcModel = simpleIni->GetBoolValue(sectionName.c_str(), "YGC_Model", "true");
				// find textures
				CSimpleIniA::TNamesDepend keys;
				simpleIni->GetAllKeys(sectionName.c_str(), keys);
				CSimpleIniA::TNamesDepend::const_iterator j;
				for (j = keys.begin(); j != keys.end(); ++j)
				{
					Ogre::String keyName = j->pItem;
					if (keyName.find("Texture_") != Ogre::String::npos)
					{
						mModels.back().textureName.push_back(simpleIni->GetValue(sectionName.c_str(), keyName.c_str(), "No texture"));
						if (mModels.back().textureName.back() != "No texture")
							mModels.back().cmodel->applyNewText(_getNameResText(mModels.back().textureName.back()), mModels.back().textureName.size() - 1);
					}
				}

				_loadModelStatusFromIni(simpleIni, sectionName, mModels.back().cmodel);
			}
		}
		delete simpleIni;
		return true; // ok
	}
	return false; // error
}

bool FormCollector::_saveCollection(const Ogre::String& pathIni)
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
			if (mModels[i].isYgcModel)
			{
				for (unsigned int j = 0; j < mModels[i].cmodel->getEntity()->getNumSubEntities(); ++j)
				{
					Ogre::String nameText = "Texture_" + Ogre::StringConverter::toString(j);
					simpleIni->SetValue(sectionName.c_str(), nameText.c_str(), mModels[i].textureName[j].c_str());
				}
			}
			simpleIni->SetBoolValue(sectionName.c_str(), "YGC_Model", mModels[i].isYgcModel);
			_saveModelStatusFromIni(simpleIni, sectionName, mModels[i].cmodel, pathIni);
		}
		simpleIni->SaveFile(pathIni.c_str());
		delete simpleIni;
		return true; // ok
	}
	return false; // error
}



Ogre::String FormCollector::_getNameResText(const Ogre::String& nameText)
{
	for (unsigned int i = 0; i < mTextures.size(); ++i)
		if (mTextures[i].filename == nameText)
			return mTextures[i].nameThumb;

	return Ogre::StringUtil::BLANK;
}



void FormCollector::hideAllOptions()
{
	hideOptions();
	hideOptionsItems();
	hideOptionsCamera();
	hideOptionEditPosition();
}

void FormCollector::hideOptions()
{
	if (mTrayMgr->getWidget("FormCollector/Window/Options"))
	{
		mTrayMgr->destroyDialogWindow("FormCollector/Window/Options");
		mTrayMgr->destroyWidget("FormCollector/Label/EditItems");
		mTrayMgr->destroyWidget("FormCollector/Label/EditCamera");
		mTrayMgr->destroyWidget("FormCollector/Label/OptionsHelp");
		mTrayMgr->destroyWidget("FormCollector/Button/CloseOptions");
	}
}

void FormCollector::showOptions()
{
	// all submenu options are hiddens?
	if (!mTrayMgr->isWindowDialogVisible())
	{
		unsigned int numOptions = 4;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 350;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		mTrayMgr->createDialogWindow("FormCollector/Window/Options", "COLLECTORS OPTIONS", left, top, width, height); top += sepOptions / 2;
		mTrayMgr->createLabel("FormCollector/Label/EditItems", "EDIT ITEMS", left, top, width, 23); top += sepOptions;
		mTrayMgr->createLabel("FormCollector/Label/EditCamera", "EDIT CAMERA", left, top, width, 23);	top += sepOptions;
		mTrayMgr->createLabel("FormCollector/Label/OptionsHelp", "HELP", left, top, width, 23); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormCollector/Button/CloseOptions", "BACK", left, top, 60);
	}
}

void FormCollector::hideOptionsItems()
{
	if (mTrayMgr->getWidget("FormCollector/Window/EditItems"))
	{
		mTrayMgr->destroyDialogWindow("FormCollector/Window/EditItems");
		mTrayMgr->destroyWidget("FormCollector/Selector/Items");
		mTrayMgr->destroyWidget("FormCollector/Selector/Models");
		mTrayMgr->destroyWidget("FormCollector/Selector/SubMesh");
		mTrayMgr->destroyWidget("FormCollector/Selector/TextName");
		mTrayMgr->destroyWidget("FormCollector/Label/NewItem");
		mTrayMgr->destroyWidget("FormCollector/Label/DeleteItem");
		mTrayMgr->destroyWidget("FormCollector/Label/ItemPosition");
		mTrayMgr->destroyWidget("FormCollector/Button/CloseOptionsItems");
	}
}

void FormCollector::showOptionsItems()
{
	// all submenu options are hiddens?
	if (!mTrayMgr->getWidget("FormCollector/Window/EditItems"))
	{
		unsigned int numOptions = 8;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 385;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		mTrayMgr->createDialogWindow("FormCollector/Window/EditItems", "ITEM OPTIONS", left, top, width, height);		top += sepOptions / 2;
		mTrayMgr->createItemSelector("FormCollector/Selector/Items", "Item", Ogre::StringVector(), left, top, width);	top += sepOptions;
		mTrayMgr->createItemSelector("FormCollector/Selector/Models", "Mesh", Ogre::StringVector(), left, top, width);	top += sepOptions;
		mTrayMgr->createItemSelector("FormCollector/Selector/SubMesh", "SubMesh", Ogre::StringVector(), left, top, width);	top += sepOptions;
		mTrayMgr->createItemSelector("FormCollector/Selector/TextName", "Texture", Ogre::StringVector(), left, top, width);	top += sepOptions;
		mTrayMgr->createLabel("FormCollector/Label/NewItem", "NEW ITEM", left, top, width, 23);							top += sepOptions;
		mTrayMgr->createLabel("FormCollector/Label/DeleteItem", "DELETE ITEM", left, top, width, 23);					top += sepOptions;
		mTrayMgr->createLabel("FormCollector/Label/ItemPosition", "EDIT POSITION", left, top, width, 23);				top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormCollector/Button/CloseOptionsItems", "BACK", left, top, 60);
		_loadConfigWindowsItem(mCurrentIndex);
	}
}

void FormCollector::hideOptionsCamera()
{
	if (mTrayMgr->getWidget("FormCollector/Window/EditCamera"))
	{
		mTrayMgr->destroyDialogWindow("FormCollector/Window/EditCamera");
		mTrayMgr->destroyWidget("FormCollector/Label/CameraPosition");
		mTrayMgr->destroyWidget("FormCollector/Label/CameraTarget");
		mTrayMgr->destroyWidget("FormCollector/Label/CameraReset");
		mTrayMgr->destroyWidget("FormCollector/Button/CloseOptionsCamera");
	}
}

void FormCollector::showOptionsCamera()
{
	// all submenu options are hiddens?
	if (!mTrayMgr->getWidget("FormCollector/Window/EditCamera"))
	{
		unsigned int numOptions = 4;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 350;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		mTrayMgr->createDialogWindow("FormCollector/Window/EditCamera", "CAMERA OPTIONS", left, top, width, height); top += sepOptions / 2;
		mTrayMgr->createLabel("FormCollector/Label/CameraPosition", "EDIT POSITION", left, top, width, 23); top += sepOptions;
		mTrayMgr->createLabel("FormCollector/Label/CameraTarget", "EDIT TARGET", left, top, width, 23);	top += sepOptions;
		mTrayMgr->createLabel("FormCollector/Label/CameraReset", "RESET", left, top, width, 23); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormCollector/Button/CloseOptionsCamera", "BACK", left, top, 60);
	}
}

void FormCollector::hideOptionEditPosition()
{
	if (mTrayMgr->getWidget("FormCollector/Window/EditPosition"))
	{
		mTrayMgr->destroyDialogWindow("FormCollector/Window/EditPosition");
		mTrayMgr->destroyWidget("FormCollector/SimpleText/EditPosition");
		mTrayMgr->destroyWidget("FormCollector/Button/CloseOptionsEditPosition");
		mTrayMgr->destroyWidget("FormCollector/Button/WindowsEditPositionHide");
		mTrayMgr->destroyWidget("FormCollector/Button/EditPositionReset");
	}
}

void FormCollector::showOptionEditPosition()
{
	if (!mTrayMgr->getWidget("FormCollector/Window/EditPosition"))
	{
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 420;
		Ogre::String descCamera =
			"To modify the model you must click and drag with the left mouse button. "
			"The keys G, R and S control the actions, translation, rotation and scale respectively, while the keys X, Y and Z control the axis.";
		SimpleText* desc = mTrayMgr->createSimpleText("FormCollector/SimpleText/EditPosition", descCamera, "YgcFont/SemiBold/16", width - 30, 250, 19, 9);
		Ogre::Real height = desc->getNumLines() * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;

		mTrayMgr->createDialogWindow("FormCollector/Window/EditPosition", "EDIT CAMERA POSITION", left, top, width, height);		top += sepOptions / 2;
		desc->setLeft(left + 12); desc->setTop(top); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormCollector/Button/CloseOptionsEditPosition", "ACCEPT", left, top, 65);
		mTrayMgr->createButton("FormCollector/Button/WindowsEditPositionHide", "HIDE", left + 70, top, 60);
		mTrayMgr->createButton("FormCollector/Button/EditPositionReset", "RESET", left + 70 + 65, top, 65);
	}
}

