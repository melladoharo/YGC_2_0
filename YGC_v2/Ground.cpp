/* Ground class [Transparent - Not visible]
*/

#include "Stdafx.h"
#include "Ground.h"

//-------------------------------------------------------------------------------------
Ground::Ground() :
mSceneMgr(Ogre::Root::getSingletonPtr()->getSceneManager("YGC2/SceneMgr")),
mPlane(0),
mNamePlane("YGC_Plane_Ground"),
mEntity(0),
mMaterial(0),
mNode(0)
{
	// Create plane in Y axis
	mPlane = new Ogre::Plane(Ogre::Vector3::UNIT_Y, 0.0f);

	Ogre::MeshManager::getSingleton().createPlane(mNamePlane,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		(*mPlane), 500, 500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	// Scene node for plane
	mEntity = mSceneMgr->createEntity(mNamePlane);
	mEntity->setCastShadows(false);
	mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mNode->attachObject(mEntity);

	// Material for new plane (transparent)
	mMaterial = Ogre::MaterialManager::getSingleton().create("YGC_Mat_Ground",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	mMaterial->setAmbient(Ogre::ColourValue(0, 0, 0, 0));
	mMaterial->setDiffuse(Ogre::ColourValue(0, 0, 0, 0));
	mMaterial->setSpecular(Ogre::ColourValue(0, 0, 0, 0));
	mMaterial->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	mEntity->setMaterial(mMaterial);

	Ogre::LogManager::getSingleton().logMessage("YGC: Ground created.");
}

//-------------------------------------------------------------------------------------
Ground::~Ground()
{
	// Elimina las entidades y el nodo
	mNode->detachAllObjects();
	mSceneMgr->destroyEntity(mEntity);
	mSceneMgr->destroySceneNode(mNode);

	// Destruye el material creado
	Ogre::MaterialManager::getSingleton().remove(mMaterial->getName());

	Ogre::LogManager::getSingletonPtr()->logMessage("YGC: Ground destroyed.");
}

