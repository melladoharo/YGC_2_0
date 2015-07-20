#include "stdafx.h"
#include "CModel.h"

CModel::CModel(const Ogre::String& nameModel, Ogre::SceneNode* parent /*= NULL*/) :
mSceneMgr(Ogre::Root::getSingletonPtr()->getSceneManager("YGC2/SceneMgr")),
mParentNode(parent),
mVisible(true)
{
	unsigned short src, dest;
	Ogre::MeshPtr meshptr = Ogre::MeshManager::getSingletonPtr()->load(nameModel,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
	if (!meshptr->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
		meshptr->buildTangentVectors(Ogre::VES_TANGENT, src, dest, true, true);

	// Creating new model
	mEnt = mSceneMgr->createEntity(nameModel);
	mEnt->setCastShadows(true);
	mNode = (mParentNode) ? mParentNode->createChildSceneNode() : mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mNode->attachObject(mEnt);
	mNode->setVisible(mVisible);
}


CModel::~CModel()
{
	mNode->detachAllObjects();
	mSceneMgr->destroyEntity(mEnt);
	mSceneMgr->destroySceneNode(mNode);
}

void CModel::applyNewText(const Ogre::String& newText, unsigned int subEnt)
{
	mMat = mEnt->getSubEntity(subEnt)->getMaterial();
	mMat = mMat->clone("mat_resYGC_" + newText);
	mMat->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
	mMat->getTechnique(0)->getPass(0)->createTextureUnitState(newText);
	//mMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(newText);
	mEnt->getSubEntity(subEnt)->setMaterial(mMat);
}

