#include "stdafx.h"
#include "FormGames.h"

FormGames::FormGames(const Ogre::String& pathGames, GuiManager* tray, GuiListener* oldListener) :
FormBase(tray, oldListener),
mFormSelector(0),
mParentThumbs(mSceneMgr->getRootSceneNode()->createChildSceneNode()),
mLastThumbOver(0)
{
	// initialize resource group 'FormGames'
	Ogre::ResourceGroupManager::getSingleton().createResourceGroup("Group/FormGames");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Group/FormGames");

	// iterate over the main path game and add them 
	for (boost::filesystem::directory_iterator it(pathGames), end; it != end; ++it)
	{
		if (boost::filesystem::is_directory(it->path()))
		{
			mGameInfo.push_back(new GameInfo(it->path().generic_string()));
			// create (if not) and load the thumbnail header game
			mInfoHeader.push_back(sInfoResource());
			mGameInfo.back()->findHeaderResource(mInfoHeader.back());
			if (mInfoHeader.back().path != Ogre::StringUtil::BLANK) // game has a header
			{
				if (!boost::filesystem::is_regular_file(mInfoHeader.back().pathThumb))
					GameInfo::createThumbnail(mInfoHeader.back().path, mInfoHeader.back().pathThumb, 256);
				GameInfo::loadImageFromDisk(mInfoHeader.back().pathThumb, mInfoHeader.back().nameThumb, "Group/FormGames");
			}
			// create the 3d thumbnail widget
			mThumbs.push_back(new Thumbnail3D(mParentThumbs, "FormGames/Thumb/Widget/" + Ogre::StringConverter::toString(mThumbs.size()),
				"Group/FormGames", mGameInfo.back()->getName(), mInfoHeader.back().nameThumb));
			mThumbs.back()->setIndex(mThumbs.size() - 1);
		}
	}

	// adjust thumbnail in a grid
	Thumbnail3D::setThumbs3DInGrid(mThumbs, 3, 11.5f, 6.25f, 3);
	resetCamera();

	// bottom slider
	if (mThumbs.size() > 9)
	{
		Ogre::Real minValue = 0;
		Ogre::Real maxValue = mThumbs.back()->getPosition().x;
		Slider* sd = mTrayMgr->createSlider("FormGames/Slider", 500, minValue, maxValue, maxValue + 100);
		sd->getOverlayElement()->setVerticalAlignment(Ogre::GVA_BOTTOM);
		sd->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
		sd->setLeft(-(sd->getWidth() / 2)); // center slider 
		sd->setTop(-50);
		addWidgetToForm(sd);
	}

	// top select menu
	Ogre::StringVector items;
	items.push_back("All");
	items.push_back("Installed");
	items.push_back("Genre");
	items.push_back("Year");
	SelectMenu* sm = mTrayMgr->createSelectMenu("FormGames/SelectMenu", items, 275.0f);
	sm->getOverlayElement()->setVerticalAlignment(Ogre::GVA_TOP);
	sm->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
	sm->setTop(120);
	sm->setLeft(-sm->getWidth() / 2);
	addWidgetToForm(sm);
}

FormGames::~FormGames()
{
	if (mFormSelector) delete mFormSelector;
	for (unsigned int i = 0; i < mGameInfo.size(); ++i)
		delete mGameInfo[i];
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		delete mThumbs[i];
	mSceneMgr->destroySceneNode(mParentThumbs);
	Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("Group/FormGames");
}



bool FormGames::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	FormBase::frameRenderingQueued(evt);

	if (mFormSelector && mFormSelector->isFinished())
	{
		if (mFormSelector) delete mFormSelector;
		mFormSelector = 0;
		mTrayMgr->assignListenerToMenuBar(mOldListener);
		mTrayMgr->setListener(this);
		mTrayMgr->setMouseEventCallback(this);
		mTrayMgr->setKeyboardEventCallback(this);
		mTrayMgr->loadMenuBarMain();
		show();
	}

	return true;
}

bool FormGames::keyPressed(const OIS::KeyEvent &arg)
{
	return FormBase::keyPressed(arg);
}

bool FormGames::keyReleased(const OIS::KeyEvent &arg)
{
	return FormBase::keyReleased(arg);
}

bool FormGames::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;

	// mouse ray [intersection with objects]
	Ogre::Ray mouseRay = mTrayMgr->getCursorRay(mCamera);
	mRaySceneQuery->setRay(mouseRay);

	// execute query
	Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
	Ogre::RaySceneQueryResult::reverse_iterator iter = result.rbegin();
	
	// thumbnail3d intersection 
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

	return FormBase::mouseMoved(arg);;
}

bool FormGames::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	return FormBase::mousePressed(arg, id);;
}

bool FormGames::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;

	if (id == OIS::MouseButtonID::MB_Left)
	{
		// clin on a thumbnail, then open the game selected
		if (mLastThumbOver)
		{
			unsigned int index = mLastThumbOver->getIndex();
			mLastThumbOver->setMouseUp();
			mLastThumbOver = 0;

			hide();
			mFormSelector = new FormSelectorGame(mGameInfo[index], mTrayMgr, this);
		}
	}

	return FormBase::mouseReleased(arg, id);;
}



void FormGames::hide()
{
	FormBase::hide();
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		mThumbs[i]->hide();
}

void FormGames::show()
{
	// correct position to see this form correctly
	resetCamera();

	FormBase::show();
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		mThumbs[i]->show();
}



void FormGames::sliderMoved(Slider* slider)
{
	mParentThumbs->setPosition(-slider->getValue(), 0, 0);
}

void FormGames::itemSelected(SelectMenu* menu)
{
	if (menu->getSelectedItem() == "All")
	{
		_filterGamesAll();
	}
	else if (menu->getSelectedItem() == "Installed")
	{
		_filterGamesByInstalled();
	}
}

void FormGames::showOptions()
{

}




void FormGames::_removeThumbs()
{
	for (unsigned int i = 0; i < mThumbs.size(); ++i)
		delete mThumbs[i];
	mThumbs.clear();
}

void FormGames::_adjustThumbs()
{
	if (!mThumbs.empty())
	{
		// adjust thumbnail in a grid
		Thumbnail3D::setThumbs3DInGrid(mThumbs, 3, 11.5f, 6.25f, 3);

		Slider* slider = (Slider *)mTrayMgr->getWidget("FormGames/Slider");
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

void FormGames::_filterGamesAll()
{
	_removeThumbs();

	for (unsigned int i = 0; i < mGameInfo.size(); ++i)
	{
		// create the 3d thumbnail widget
		mThumbs.push_back(new Thumbnail3D(mParentThumbs, "FormGames/Thumb/Widget/" + Ogre::StringConverter::toString(mThumbs.size()),
			"Group/FormGames", mGameInfo[i]->getName(), mInfoHeader[i].nameThumb));
		mThumbs.back()->setIndex(i);
	}

	_adjustThumbs();
}

void FormGames::_filterGamesByInstalled()
{
	_removeThumbs();

	for (unsigned int i = 0; i < mGameInfo.size(); ++i)
	{
		if (mGameInfo[i]->getValueOption("GAME.INFO", "Name", "???") == "Mass Effect 2")
		{
			// create the 3d thumbnail widget
			mThumbs.push_back(new Thumbnail3D(mParentThumbs, "FormGames/Thumb/Widget/" + Ogre::StringConverter::toString(mThumbs.size()),
				"Group/FormGames", mGameInfo[i]->getName(), mInfoHeader[i].nameThumb));
			mThumbs.back()->setIndex(i);
		}
	}
	
	_adjustThumbs();
}

