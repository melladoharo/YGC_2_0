#include "stdafx.h"
#include "ModelController.h"

CModelController::CModelController(Ogre::SceneNode* node, Ogre::Real acceleration /*= 1.0f*/) : 
mSceneMgr(Ogre::Root::getSingletonPtr()->getSceneManager("YGC2/SceneMgr")),
mNode(node),
mActionAxis(Ogre::Vector3::UNIT_X),
mAccel(acceleration),
mTranslate(true), mRotate(false), mScale(false), mDragging(false)
{

	mEntSphere = mSceneMgr->createEntity("mesh_sphere.mesh");
	mNode->attachObject(mEntSphere);

	mNode->showBoundingBox(true);
	mNode->_update(true, true);
	Ogre::AxisAlignedBox aaBox(Ogre::AxisAlignedBox(mNode->_getWorldAABB()));
	_createLine(aaBox.getCenter(), mActionAxis, Ogre::ColourValue::Blue);
}

CModelController::~CModelController()
{
	mSceneMgr->destroyEntity(mEntSphere);
	mNode->showBoundingBox(false);

	_removeLine();
}



void CModelController::injectKeyDown(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_G)
	{
		mTranslate = true;
		mRotate = false;
		mScale = false;
	}
	else if (arg.key == OIS::KC_R)
	{
		mTranslate = false;
		mRotate = true;
		mScale = false;
	}
	else if (arg.key == OIS::KC_S)
	{
		mTranslate = false;
		mRotate = false;
		mScale = true;
		mActionAxis = Ogre::Vector3::UNIT_SCALE;
		_removeLine();
	}
	else if (arg.key == OIS::KC_X)
	{
		mActionAxis = Ogre::Vector3::UNIT_X;
		_removeLine();
		Ogre::AxisAlignedBox aaBox(Ogre::AxisAlignedBox(mNode->_getWorldAABB()));
		_createLine(aaBox.getCenter(), mActionAxis, Ogre::ColourValue::Blue);
	}
	else if (arg.key == OIS::KC_Y)
	{
		mActionAxis = Ogre::Vector3::UNIT_Y;
		_removeLine();
		Ogre::AxisAlignedBox aaBox(Ogre::AxisAlignedBox(mNode->_getWorldAABB()));
		_createLine(aaBox.getCenter(), mActionAxis, Ogre::ColourValue::Red);
	}
	else if (arg.key == OIS::KC_Z)
	{
		mActionAxis = Ogre::Vector3::UNIT_Z;
		_removeLine();
		Ogre::AxisAlignedBox aaBox(Ogre::AxisAlignedBox(mNode->_getWorldAABB()));
		_createLine(aaBox.getCenter(), mActionAxis, Ogre::ColourValue::Green);
	}
}

void CModelController::injectKeyUp(const OIS::KeyEvent &arg)
{

}

void CModelController::injectMouseMove(const OIS::MouseEvent &arg)
{
	if (mDragging)
	{
		if (mTranslate)
			mNode->translate(mActionAxis * (arg.state.X.rel * 0.020f * mAccel), Ogre::Node::TS_WORLD);
		else if (mRotate)
			mNode->rotate(Ogre::Quaternion(Ogre::Degree(arg.state.X.rel * 0.5f), mActionAxis), Ogre::Node::TS_WORLD);
		else
			mNode->setScale(mNode->getScale() + (mActionAxis * (arg.state.X.rel * 0.020f)));
	}
}

void CModelController::injectMouseDown(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (id == OIS::MouseButtonID::MB_Left)
	{
		mDragging = true;
	}
}

void CModelController::injectMouseUp(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	mDragging = false;
}




void CModelController::_createLine(const Ogre::Vector3& position, const Ogre::Vector3 axis, const Ogre::ColourValue& color)
{
	Ogre::String nameObject = "ManualObject/Line/";
	Ogre::String nameNode = "ManualObject/Node";
	Ogre::String nameMaterial = "Mat/ManualObject/Line";

	// new manual object
	Ogre::ManualObject* newLine = mSceneMgr->createManualObject(nameObject);
	newLine->setCastShadows(false);
	Ogre::SceneNode* nodeLine = mSceneMgr->getRootSceneNode()->createChildSceneNode(nameNode);

	// new material
	Ogre::MaterialPtr matLine = Ogre::MaterialManager::getSingleton().create(nameMaterial,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	matLine->setReceiveShadows(false);
	matLine->getTechnique(0)->setLightingEnabled(true);
	matLine->getTechnique(0)->getPass(0)->setDiffuse(color);
	matLine->getTechnique(0)->getPass(0)->setAmbient(color);
	matLine->getTechnique(0)->getPass(0)->setSelfIllumination(color);

	// new line 
	Ogre::Vector3 vecLine(axis.x * 1000, axis.y * 1000, axis.z * 1000);
	newLine->begin(nameMaterial, Ogre::RenderOperation::OT_LINE_LIST);
	newLine->position(position.x - vecLine.x, position.y - vecLine.y, position.z - vecLine.z);
	newLine->position(position.x + vecLine.x, position.y + vecLine.y, position.z + vecLine.z);
	newLine->end();
	nodeLine->attachObject(newLine);
}

void CModelController::_removeLine()
{
	Ogre::String nameObject = "ManualObject/Line/";
	Ogre::String nameNodo = "ManualObject/Node";
	Ogre::String nameMaterial = "Mat/ManualObject/Line";

	// delete the line (if exists)
	if (mSceneMgr->hasManualObject(nameObject))
	{
		Ogre::MaterialManager::getSingletonPtr()->unload(nameMaterial);
		Ogre::MaterialManager::getSingletonPtr()->remove(nameMaterial);
		mSceneMgr->destroyManualObject(nameObject);
		mSceneMgr->destroySceneNode(nameNodo);
	}
}

