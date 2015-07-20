/* Background class
*/
#include "Stdafx.h"
#include "Background.h"

//-------------------------------------------------------------------------------------
Background::Background() :
mSceneMgr(Ogre::Root::getSingletonPtr()->getSceneManager("YGC2/SceneMgr")),
mMaterial(0),
mRect(0),
mNode(0)
{
	// Create background material
	mMaterial = Ogre::MaterialManager::getSingleton().create("YGC_Mat_Background", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	mMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("sdk_background.png");
	mMaterial->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
	mMaterial->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	mMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false);

	// Create background rectangle covering the whole screen
	mRect = new Ogre::Rectangle2D(true);
	mRect->setCorners(-1.0, 1.0, 1.0, -1.0);
	mRect->setMaterial(mMaterial->getName());

	// Render the background before everything else
	mRect->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);

	// Use infinite AAB to always stay visible
	Ogre::AxisAlignedBox aabInf;
	aabInf.setInfinite();
	mRect->setBoundingBox(aabInf);

	// Attach background to the scene
	mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mNode->attachObject(mRect);

	Ogre::LogManager::getSingleton().logMessage("YGC: Background created.");
}

//-------------------------------------------------------------------------------------
Background::~Background()
{
	mNode->detachAllObjects();
	if(mRect) delete mRect;
	mSceneMgr->destroySceneNode(mNode);

	Ogre::TextureManager::getSingleton().remove(mMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getName());
	Ogre::MaterialManager::getSingleton().remove(mMaterial->getName());

	Ogre::LogManager::getSingleton().logMessage("YGC: Background destroyed.");
}

