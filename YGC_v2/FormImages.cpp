#include "stdafx.h"
#include "FormImages.h"

FormImages::FormImages(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener) : 
FormBase(tray, oldListener),
mParentThumbs(mSceneMgr->getRootSceneNode()->createChildSceneNode()),
mGameInfo(gInfo),
mLastThumbOver(0)
{
	// find all images of the game
	mGameInfo->findScreenshotsResources(mImages);
	mIndexScreenshots = std::make_pair(0, mImages.size());
	mGameInfo->findWallResource(mImages);
	mIndexWallpapers = std::make_pair(mIndexScreenshots.second, mImages.size());
	mGameInfo->findConceptArtResources(mImages);
	mIndexConceptArt = std::make_pair(mIndexWallpapers.second, mImages.size());

	// load thumbnails...
	for (unsigned int i = 0; i < mImages.size(); ++i)
	{
		if (!boost::filesystem::is_regular_file(mImages[i].pathThumb))
			GameInfo::createThumbnail(mImages[i].path, mImages[i].pathThumb, 256);
		GameInfo::loadImageFromDisk(mImages[i].pathThumb, mImages[i].nameThumb, mGameInfo->getGroupName(), 0);
		mThumbs.push_back(new Thumbnail3D(mParentThumbs, "FormImages/Thumb/" + Ogre::StringConverter::toString(i),
			mGameInfo->getGroupName(), mImages[i].filename, mImages[i].nameThumb));
		mThumbs.back()->setIndex(mThumbs.size() - 1);
	}
	
	// ... and set in a grid
	Thumbnail3D::setThumbs3DInGrid(mThumbs, 3, 10.4f, 6.25f, 3);

	// material for original-selected images
	Ogre::MaterialPtr matImage = Ogre::MaterialManager::getSingleton().create(
		"FormImage/Mat/Original", mGameInfo->getGroupName());

	// slider for movement
	if (mThumbs.size() > 9)
	{
		Ogre::Real minValue = 0;
		Ogre::Real maxValue = mThumbs.back()->getPosition().x;
		Slider* sd = mTrayMgr->createSlider("FormImages/Slider", 370, minValue, maxValue, maxValue + 100);
		sd->getOverlayElement()->setVerticalAlignment(Ogre::GVA_BOTTOM);
		sd->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
		sd->setLeft(-(sd->getWidth() / 2));
		sd->setTop(-50);
		addWidgetToForm(sd);
	}

	// select menu 
	Ogre::StringVector items;
	items.push_back("All Images");
	items.push_back("Concept Art");
	items.push_back("Screenshots");
	items.push_back("Wallpapers");
	SelectMenu* sm = mTrayMgr->createSelectMenu("FormImages/SelMenu", items, 275.0f);
	sm->getOverlayElement()->setVerticalAlignment(Ogre::GVA_TOP);
	sm->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
	sm->setTop(120);
	sm->setLeft(-sm->getWidth() / 2);
	addWidgetToForm(sm);
}

FormImages::~FormImages()
{
	Ogre::MaterialManager::getSingleton().remove("FormImage/Mat/Original");
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		delete mThumbs[i];
	mSceneMgr->destroySceneNode(mParentThumbs);
}



bool FormImages::keyPressed(const OIS::KeyEvent &arg)
{
	return FormBase::keyPressed(arg);
}

bool FormImages::keyReleased(const OIS::KeyEvent &arg)
{
	return FormBase::keyReleased(arg);
}

bool FormImages::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;

	// mouse ray [intersection with objects]
	Ogre::Ray mouseRay = mTrayMgr->getCursorRay(mCamera);
	mRaySceneQuery->setRay(mouseRay);

	// execute query
	Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
	Ogre::RaySceneQueryResult::reverse_iterator iter = result.rbegin();

	Thumbnail3D* currentOver = 0;
	for (iter; iter != result.rend(); ++iter)
	{
		if (iter->movable->isVisible() && iter->movable->getName().find("Mesh/Thumbnail3D/") != Ogre::String::npos)
		{
			currentOver = Thumbnail3D::getThumbnail3dOver(mThumbs, iter->movable->getName());
		}
	}
	if (currentOver) mLastThumbOver = currentOver;
	else if (mLastThumbOver) { mLastThumbOver->setMouseUp(); mLastThumbOver = 0; }
	
	return FormBase::mouseMoved(arg);
}

bool FormImages::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	return FormBase::mousePressed(arg, id);
}

bool FormImages::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;

	if (id == OIS::MouseButtonID::MB_Left)
	{
		if (mLastThumbOver)
		{
			unsigned int index = mLastThumbOver->getIndex();
			mLastThumbOver->setMouseUp();
			mLastThumbOver = 0;

			if (!Ogre::TextureManager::getSingleton().resourceExists(mImages[index].name))
				GameInfo::loadImageFromDisk(mImages[index].path, mImages[index].name, mGameInfo->getGroupName());
			Ogre::MaterialPtr matOriginal = Ogre::MaterialManager::getSingleton().getByName("FormImage/Mat/Original");
			matOriginal->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
			matOriginal->getTechnique(0)->getPass(0)->createTextureUnitState(mImages[index].name);

			mTrayMgr->showBackdrop(matOriginal->getName());
			mTrayMgr->autoResizeBackdrop();
			mTrayMgr->hideMenuBar();
			mTrayMgr->hideWidgets();
			hide();
		}
	}
	else if (id == OIS::MouseButtonID::MB_Right)
	{
		if (!mTrayMgr->isWidgetsVisible())
		{
			mTrayMgr->hideBackdrop();
			mTrayMgr->showMenuBar();
			mTrayMgr->showWidgets();
			show();
		}
	}

	return FormBase::mouseReleased(arg, id);
}



void FormImages::hide()
{
	FormBase::hide();
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		mThumbs[i]->hide();
}

void FormImages::show()
{
	// correct position to see this form correctly
	resetCamera();

	FormBase::show();
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		mThumbs[i]->show();
}



void FormImages::sliderMoved(Slider* slider)
{
	mParentThumbs->setPosition(-slider->getValue(), 0, 0);
}

void FormImages::itemSelected(SelectMenu* menu)
{
	if (menu->getSelectedItem() == "All Images")
	{
		_filterImageByIndex(std::make_pair(0, mImages.size()));
	}
	else if (menu->getSelectedItem() == "Concept Art")
	{
		_filterImageByIndex(mIndexConceptArt);
	}
	else if (menu->getSelectedItem() == "Screenshots")
	{
		_filterImageByIndex(mIndexScreenshots);
	}
	else if (menu->getSelectedItem() == "Wallpapers")
	{
		_filterImageByIndex(mIndexWallpapers);
	}
}

void FormImages::showOptions()
{

}



void FormImages::_filterImageByIndex(std::pair<unsigned int, unsigned int> indexFilter)
{
	mParentThumbs->detachAllObjects();
	mParentThumbs->setPosition(0, 0, 0);
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		delete mThumbs[i];
	mThumbs.clear();

	for (unsigned int i = indexFilter.first; i < indexFilter.second; ++i)
	{
		mThumbs.push_back(new Thumbnail3D(mParentThumbs, "FormImages/Thumb/" + Ogre::StringConverter::toString(i),
			mGameInfo->getGroupName(), mImages[i].filename, mImages[i].nameThumb));
		mThumbs.back()->setIndex(i);
	}

	if (!mThumbs.empty())
	{
		// adjust thumbnail in a grid
		Thumbnail3D::setThumbs3DInGrid(mThumbs, 3, 10.4f, 6.25f, 3);

		Slider* slider = (Slider *)mTrayMgr->getWidget("FormImages/Slider");
		if (slider)
		{
			if (mThumbs.size() > 9)
			{
				Ogre::Real minValue = 0;
				Ogre::Real maxValue = mThumbs.back()->getPosition().x;
				slider->show();
				slider->setRange(minValue, maxValue, maxValue + 100, false);
			}
			else
			{
				slider->hide();
			}
		}
	}
}

