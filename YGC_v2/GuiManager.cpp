#include "stdafx.h"
#include "GuiManager.h"

GuiManager::GuiManager(const Ogre::String& name, Ogre::RenderWindow* window, OIS::Mouse* mouse, OIS::Keyboard* keyboard, GuiListener* listener) :
mName(name), 
mWindow(window), 
mMouse(mouse),  
mKeyboard(keyboard),
mWidgetDeathRow(), 
mListener(listener), 
mExpandedMenu(0), mMediaPlayer(0), mLoadBar(0),
mDofEffect(0),
mCursorWasVisible(false),
mShutDown(false)
{
	mTimer = Ogre::Root::getSingleton().getTimer();
	mLastStatUpdateTime = 0;

	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();

	Ogre::String nameBase = mName + "/";
	std::replace(nameBase.begin(), nameBase.end(), ' ', '_');

	// create overlay layers for everything
	mBackdropLayer = om.create(nameBase + "BackdropLayer");
	mPriorityLayer = om.create(nameBase + "PriorityLayer");
	mCursorLayer = om.create(nameBase + "CursorLayer");
	mWidgetsLayer = om.create(nameBase + "WidgetsLayer");
	mMenuBarLayer = om.create(nameBase + "MenuBarLayer");
	mWindowsLayer = om.create(nameBase + "WindowsLayer");
	mBackdropLayer->setZOrder(100);
	mWindowsLayer->setZOrder(150);
	mWidgetsLayer->setZOrder(200);
	mPriorityLayer->setZOrder(300);
	mMenuBarLayer->setZOrder(380);
	mCursorLayer->setZOrder(400);

	// make backdrop and cursor overlay containers
	mCursor = (Ogre::OverlayContainer*)om.createOverlayElementFromTemplate("SdkTrays/Cursor", "Panel", nameBase + "Cursor");
	mCursorLayer->add2D(mCursor);
	mBackdrop = (Ogre::OverlayContainer*)om.createOverlayElement("Panel", nameBase + "Backdrop");
	mBackdropLayer->add2D(mBackdrop);
	mTray = (Ogre::OverlayContainer*)om.createOverlayElement("Panel", nameBase + "ContainerWidget");
	mWidgetsLayer->add2D(mTray);
	mWidgetsLayer->show();
	mTray->show();
	mDialogShade = (Ogre::OverlayContainer*)om.createOverlayElement("Panel", nameBase + "DialogShade");
	mDialogShade->hide();
	mPriorityLayer->add2D(mDialogShade);
	mPriorityLayer->show();
	mDialogWindows = (Ogre::OverlayContainer*)om.createOverlayElement("Panel", nameBase + "WindowsDialog");
	mWindowsLayer->add2D(mDialogWindows);
	mWindowsLayer->hide();

	// create the menu bar
	mMenuBar = new MenuBar;
	mMenuBarLayer->add2D((Ogre::OverlayContainer*)mMenuBar->getOverlayElement());
	mMenuBarLayer->show();
	mMenuBar->_assignListener(mListener);

	// create media player
	mMediaPlayer = createMediaPlayer("GuiManager/MediaPlayer");
	mMediaPlayer->hide();

	showCursor();
}

GuiManager::~GuiManager()
{
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();

	destroyAllWidgets();

	for (unsigned int i = 0; i < mWidgetDeathRow.size(); i++)   // delete widgets queued for destruction
	{
		delete mWidgetDeathRow[i];
	}
	mWidgetDeathRow.clear();

	om.destroy(mBackdropLayer);
	om.destroy(mPriorityLayer);
	om.destroy(mWindowsLayer);
	om.destroy(mWidgetsLayer);
	om.destroy(mCursorLayer);
	om.destroy(mMenuBarLayer);

	Widget::nukeOverlayElement(mBackdrop);
	Widget::nukeOverlayElement(mTray);
	Widget::nukeOverlayElement(mDialogWindows);
	Widget::nukeOverlayElement(mCursor);
	Widget::nukeOverlayElement(mDialogShade);
	Widget::nukeOverlayElement(mMenuBar->getOverlayElement());
}



void GuiManager::destroyWidget(Widget* widget)
{
	if (!widget) OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "Widget does not exist.", "TrayManager::destroyWidget");

	mTray->removeChild(widget->getName());
	mWidgets.erase(std::find(mWidgets.begin(), mWidgets.end(), widget));
	if (widget == mExpandedMenu) setExpandedMenu(0);
	widget->cleanup();
	mWidgetDeathRow.push_back(widget);
}

void GuiManager::destroyDialogWindow(Widget* windows)
{
	if (!windows) OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "Widget does not exist.", "TrayManager::destroyWidget");

	mDialogWindows->removeChild(windows->getName());
	mWindowsLayer->hide();
	mWidgets.erase(std::find(mWidgets.begin(), mWidgets.end(), windows));
	windows->cleanup();
	mWidgetDeathRow.push_back(windows);
}



void GuiManager::setExpandedMenu(SelectMenu* m)
{
	if (!mExpandedMenu && m)
	{
		Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)m->getOverlayElement();
		Ogre::OverlayContainer* eb = (Ogre::OverlayContainer*)c->getChild(m->getName() + "/MenuExpandedBox");
		eb->_update();
		eb->setPosition
			((unsigned int)(eb->_getDerivedLeft() * Ogre::OverlayManager::getSingleton().getViewportWidth()),
			(unsigned int)(eb->_getDerivedTop() * Ogre::OverlayManager::getSingleton().getViewportHeight()));
		c->removeChild(eb->getName());
		mPriorityLayer->add2D(eb);
	}
	else if (mExpandedMenu && !m)
	{
		Ogre::OverlayContainer* eb = mPriorityLayer->getChild(mExpandedMenu->getName() + "/MenuExpandedBox");
		mPriorityLayer->remove2D(eb);
		((Ogre::OverlayContainer*)mExpandedMenu->getOverlayElement())->addChild(eb);
	}

	mExpandedMenu = m;
}



bool GuiManager::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	for (unsigned int i = 0; i < mWidgetDeathRow.size(); i++)
	{
		delete mWidgetDeathRow[i];
	}
	mWidgetDeathRow.clear();

	return true;
}

bool GuiManager::injectMouseMove(const OIS::MouseEvent& evt)
{
	if (!mCursorLayer->isVisible()) return false;   // don't process if cursor layer is invisible

	Ogre::Vector2 cursorPos(evt.state.X.abs, evt.state.Y.abs);
	mCursor->setPosition(cursorPos.x, cursorPos.y);

	if (mExpandedMenu)   // only check top priority widget until it passes on
	{
		mExpandedMenu->_cursorMoved(cursorPos);
		return true;
	}

	mMenuBar->_cursorMoved(cursorPos);
	if (mMenuBar->isMouseOver())
		return true;

	if (!mWidgetsLayer->isVisible()) return false;
	for (unsigned int i = 0; i < mWidgets.size(); i++)
	{
		
		Widget* w = mWidgets[i];
		if (!w->getOverlayElement()->isVisible()) continue;
		w->_cursorMoved(cursorPos);    // send event to widget
	}

	return false;
}

bool GuiManager::injectMouseUp(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	if (!mCursorLayer->isVisible() || !mWidgetsLayer->isVisible() || id != OIS::MB_Left) return false;

	Ogre::Vector2 cursorPos(mCursor->getLeft(), mCursor->getTop());

	if (mExpandedMenu)   // only check top priority widget until it passes on
	{
		mExpandedMenu->_cursorReleased(cursorPos);
		return true;
	}

	mMenuBar->_cursorReleased(cursorPos);
	if (mMenuBar->isMouseOver())
		return true;

	for (unsigned int i = 0; i < mWidgets.size(); i++)
	{
		Widget* w = mWidgets[i];
		if (!w->getOverlayElement()->isVisible()) continue;
		w->_cursorReleased(cursorPos);    // send event to widget
	}

	return false;
}

bool GuiManager::injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	// only process left button when stuff is visible
	if (!mCursorLayer->isVisible() || !mWidgetsLayer->isVisible() || id != OIS::MB_Left) return false;

	Ogre::Vector2 cursorPos(mCursor->getLeft(), mCursor->getTop());

	if (mExpandedMenu)   // only check top priority widget until it passes on
	{
		mExpandedMenu->_cursorPressed(cursorPos);
		if (!mExpandedMenu->isExpanded()) setExpandedMenu(0);
		return true;
	}

	mMenuBar->_cursorPressed(cursorPos);
	if (mMenuBar->isMouseOver())
		return true;

	for (unsigned int i = 0; i < mWidgets.size(); i++)
	{
		Widget* w = mWidgets[i];
		if (!w->getOverlayElement()->isVisible()) continue;
		w->_cursorPressed(cursorPos);    // send event to widget

		SelectMenu* m = dynamic_cast<SelectMenu*>(w);
		if (m && m->isExpanded())       // a menu has begun a top priority session
		{
			setExpandedMenu(m);
			return true;
		}
	}

	return false;
}

void GuiManager::refreshCursor()
{
#if (OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0) || (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS)
	// TODO:
	// the position should be based on the orientation, for now simply return
	return;
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
	std::vector<OIS::MultiTouchState> states = mMouse->getMultiTouchStates();
	if (states.size() > 0)
		mCursor->setPosition(states[0].X.abs, states[0].Y.abs);
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
	// TODO: handle cursor positioning
#else
	mCursor->setPosition(mMouse->getMouseState().X.abs, mMouse->getMouseState().Y.abs);
#endif
}

void GuiManager::autoResizeBackdrop()
{
	Ogre::Real sizeTexX = mBackdrop->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureDimensions().first;
	Ogre::Real sizeTexY = mBackdrop->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureDimensions().second;
	Ogre::Real maxSizeX = Ogre::Root::getSingleton().getAutoCreatedWindow()->getViewport(0)->getActualWidth();
	Ogre::Real maxSizeY = Ogre::Root::getSingleton().getAutoCreatedWindow()->getViewport(0)->getActualHeight();
	mBackdrop->setMetricsMode(Ogre::GMM_PIXELS);
	mBackdrop->setLeft(0);
	mBackdrop->setTop(0);

	if (sizeTexX > sizeTexY)
	{
		mBackdrop->setWidth(maxSizeX);
		mBackdrop->setHeight((sizeTexY * maxSizeX) / sizeTexX);
	}
	else
	{
		mBackdrop->setWidth((sizeTexX * maxSizeY) / sizeTexY);
		mBackdrop->setHeight(maxSizeY);
	}
	if (mBackdrop->getWidth() < maxSizeX) mBackdrop->setLeft((maxSizeX - mBackdrop->getWidth()) / 2);
	if (mBackdrop->getHeight() < maxSizeY) mBackdrop->setTop((maxSizeY - mBackdrop->getHeight()) / 2);
}

Widget* GuiManager::getWidget(const Ogre::String& name)
{
	for (unsigned int i = 0; i < mWidgets.size(); i++)
	{
		if (mWidgets[i]->getName() == name) return mWidgets[i];
	}
	return 0;
}


void GuiManager::showCursor(const Ogre::String& materialName /*= Ogre::StringUtil::BLANK*/)
{
	if (materialName != Ogre::StringUtil::BLANK) getCursorImage()->setMaterialName(materialName);

	if (!mCursorLayer->isVisible())
	{
		mCursorLayer->show();
		refreshCursor();
	}
}

void GuiManager::hideCursor()
{
	mCursorLayer->hide();

	// give widgets a chance to reset in case they're in the middle of something
	for (unsigned int i = 0; i < mWidgets.size(); i++)
	{
		mWidgets[i]->_focusLost();
	}

	setExpandedMenu(0);
}

void GuiManager::showProgressBar()
{
	//if (mDialog) closeDialog();
	if (mLoadBar) hideProgressBar();

	mLoadBar = new ProgressBar("GuiManager/ProgressBar", 450);
	Ogre::OverlayElement* e = mLoadBar->getOverlayElement();
	mDialogShade->addChild(e);
	mDialogShade->show();
}

void GuiManager::hideProgressBar()
{
	if (mLoadBar)
	{
		mLoadBar->cleanup();
		delete mLoadBar;
		mLoadBar = 0;

		mDialogShade->hide();
	}
}

void GuiManager::setProgressBarProgress(Ogre::Real progress)
{
	if (mLoadBar)
	{
		mLoadBar->setProgress(progress);
		Ogre::Root::getSingleton().getAutoCreatedWindow()->update(true);
	}
}

void GuiManager::setProgressBarCaption(const Ogre::DisplayString& caption)
{
	if (mLoadBar)
	{
		mLoadBar->setCaption(caption);
		Ogre::Root::getSingleton().getAutoCreatedWindow()->update(true);
	}
}

