#include "stdafx.h"
#include "GuiManager.h"

GuiManager::GuiManager(const Ogre::String& name, Ogre::RenderWindow* window, Ogre::Camera* camera, 
	OIS::Mouse* mouse, OIS::Keyboard* keyboard, GuiListener* listener) :
mName(name), 
mWindow(window), 
mCamera(camera),
mMouse(mouse),  
mKeyboard(keyboard),
mWidgetDeathRow(), 
mListener(listener), 
mExpandedMenu(0), mMediaPlayer(0), mLoadBar(0), mDialogDecorWindows(0), mDialogMessage(0),
mOk(0), mYes(0), mNo(0),
mDofEffect(0),
mAudioPlayer(0),
mRenderText(0),
mFadeAmount(0),
mAutoHidePlayer(0.0f),
mCursorWasVisible(false),
mCursorOverPlayer(false),
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
	mPlayerLayer = om.create(nameBase + "MediaPlayerLayer");
	mReviewLayer = om.create(nameBase + "ReviewLayer");
	
	mReviewLayer->setZOrder(330);
	mWindowsLayer->setZOrder(340);
	mWidgetsLayer->setZOrder(350);
	mMenuBarLayer->setZOrder(360);
	mBackdropLayer->setZOrder(370);
	mPlayerLayer->setZOrder(380);
	mPriorityLayer->setZOrder(390);
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
	mPlayerTray = (Ogre::OverlayContainer*)om.createOverlayElement("Panel", nameBase + "ContainerPlayer");
	mPlayerLayer->add2D(mPlayerTray);
	mPlayerLayer->show();
	mReviewTray = (Ogre::OverlayContainer*)om.createOverlayElement("Panel", nameBase + "ContainerReview");
	mReviewLayer->add2D(mReviewTray);
	mReviewLayer->show();

	// create the menu bar
	mMenuBar = new MenuBar;
	mMenuBarLayer->add2D((Ogre::OverlayContainer*)mMenuBar->getOverlayElement());
	mMenuBarLayer->show();
	mMenuBar->_assignListener(mListener);

	// create media player
	mMiniPlayer = new MediaPlayerMini("GuiManager/MiniPlayer", 80, 100);
	mMiniPlayer->setTop(80);
	mMiniPlayer->setLeft(-mMiniPlayer->getWidth() - 20);
	mMiniPlayer->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_RIGHT);
	mMiniPlayer->hide();
	mMiniPlayer->_assignListener(this);
	mMiniPlayer->assignSliderListener(this);	
	Ogre::String randomTrack = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Random_Track", "Yes");
	Ogre::String repeatTrack = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Repeat_Track", "Yes");
	mMiniPlayer->setRandom(randomTrack == "Yes" ? true : false);
	mMiniPlayer->setRepeat(repeatTrack == "Yes" ? true : false);
	mPlayerTray->addChild(mMiniPlayer->getOverlayElement());
	mTrackList = new TrackList("GuiManager/TrackList", Ogre::StringVector(), 20, mMiniPlayer->getTop() + mMiniPlayer->getHeight() + 10, 7);
	mTrackList->hide();
	mTrackList->_assignListener(this);
	mPlayerTray->addChild(mTrackList->getOverlayElement());

	// render texture [fade effect]
	mTextRtt = Ogre::TextureManager::getSingleton().createManual("GuiManager/Texture/Rtt", 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 
		mWindow->getWidth(), mWindow->getHeight(), 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);
	mMatRtt = Ogre::MaterialManager::getSingleton().create("GuiManager/Material/MatRtt", 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	mRenderText = mTextRtt->getBuffer()->getRenderTarget();
	mRenderText->setAutoUpdated(false);
	mRenderText->addViewport(mCamera);
	mMatRtt->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
	mMatRtt->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	mMatRtt->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	mMatRtt->getTechnique(0)->getPass(0)->createTextureUnitState("GuiManager/Texture/Rtt");

	showCursor();
}

GuiManager::~GuiManager()
{
	// save miniPlayer status [random and repeat]
	ConfigReader::getSingletonPtr()->getReader()->SetValue("SYSTEM", "Random_Track", mMiniPlayer->isRandom() ? "Yes" : "No" );
	ConfigReader::getSingletonPtr()->getReader()->SetValue("SYSTEM", "Repeat_Track", mMiniPlayer->isRepeatMedia() ? "Yes" : "No");
	ConfigReader::getSingletonPtr()->saveConfig();

	if (mAudioPlayer) delete mAudioPlayer;

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
	om.destroy(mPlayerLayer);
	om.destroy(mReviewLayer);

	Widget::nukeOverlayElement(mBackdrop);
	Widget::nukeOverlayElement(mTray);
	Widget::nukeOverlayElement(mDialogWindows);
	Widget::nukeOverlayElement(mCursor);
	Widget::nukeOverlayElement(mDialogShade);
	Widget::nukeOverlayElement(mPlayerTray);
	Widget::nukeOverlayElement(mMenuBar->getOverlayElement());
	Widget::nukeOverlayElement(mReviewTray);
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
void GuiManager::destroyMediaPlayer(MediaPlayer* player)
{
	if (!player) OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "Widget does not exist.", "TrayManager::destroyWidget");

	mPlayerTray->removeChild(player->getName());
	player->cleanup();
}

void GuiManager::destroyReviewText(SimpleText* text)
{
	if (!text) OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "Widget does not exist.", "TrayManager::destroyWidget");

	mReviewTray->removeChild(text->getName());
	mWidgets.erase(std::find(mWidgets.begin(), mWidgets.end(), text));
	text->cleanup();
	mWidgetDeathRow.push_back(text);
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

	// fade effect
	if (isBackdropVisible() && mFadeAmount > 0)
	{
		mMatRtt->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(
			Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, mFadeAmount);
		mFadeAmount -= 0.2f;
		if (mFadeAmount <= 0)
			hideBackdrop();
	}

	// audio player
	if (mAudioPlayer && mAudioPlayer->isReady())
	{
		if (mAudioPlayer->endSound())
		{
			if (!mMiniPlayer->isRepeatMedia())
			{
				if (mMiniPlayer->isRandom()) mTrackList->selectRandomTrack();
				else mTrackList->selectNextTrack();
			}
			_playTrack(mTrackList->getSelectedTrack());
		}

		mMiniPlayer->setSliderValue(mAudioPlayer->getActualTime(), mAudioPlayer->getDuration(), false);
	}

	if (mMiniPlayer->isVisible() && mAutoHidePlayer < 0)
		hideMiniPlayer();

	if (evt.timeSinceLastFrame < 0.5f && !mCursorOverPlayer) // :((
		mAutoHidePlayer -= evt.timeSinceLastFrame;

	return true;
}

bool GuiManager::injectMouseMove(const OIS::MouseEvent& evt)
{
	if (!mCursorLayer->isVisible()) return false;   // don't process if cursor layer is invisible

	Ogre::Vector2 cursorPos(evt.state.X.abs, evt.state.Y.abs);
	mCursor->setPosition(cursorPos.x, cursorPos.y);
	
	// miniplayer visibility
	mCursorOverPlayer = false;
	if (!mTrackList->isEmpty())
	{
		if (!mMiniPlayer->isVisible())
		{
			if (cursorPos.x >= mWindow->getWidth() - 10 && cursorPos.y > 60 && cursorPos.y < 170)
			{
				mMiniPlayer->show();
				mAutoHidePlayer = 3;
				mCursorOverPlayer = true;
			}
		}
		else if (mMiniPlayer->isCursorOver(mMiniPlayer->getOverlayElement(), cursorPos, - 20) || 
			(mTrackList->isVisible() && mTrackList->isCursorOver(mTrackList->getOverlayElement(), cursorPos, - 20)))
		{
			mCursorOverPlayer = true;
		}
	}
	
	if (mExpandedMenu)   // only check top priority widget until it passes on
	{
		mExpandedMenu->_cursorMoved(cursorPos);
		return true;
	}

	if (mDialogDecorWindows)   // only check top priority widget until it passes on
	{
		if (mOk)
		{
			mOk->_cursorMoved(cursorPos);
		}
		else
		{
			mYes->_cursorMoved(cursorPos);
			mNo->_cursorMoved(cursorPos);
		}
		return true;
	}

	mMenuBar->_cursorMoved(cursorPos);
	if (mMenuBar->isMouseOver())
		return true;

	mMiniPlayer->_cursorMoved(cursorPos);
	mTrackList->_cursorMoved(cursorPos);

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

	if (mDialogDecorWindows)   // only check top priority widget until it passes on
	{		
		if (mOk)
		{
			mOk->_cursorReleased(cursorPos);
		}
		else
		{
			mYes->_cursorReleased(cursorPos);
			// very important to check if second button still exists, because first button could've closed the popup
			if (mNo) mNo->_cursorReleased(cursorPos);
		}
		return true;
	}

	mMenuBar->_cursorReleased(cursorPos);
	if (mMenuBar->isMouseOver())
		return true;

	mMiniPlayer->_cursorReleased(cursorPos);
	mTrackList->_cursorReleased(cursorPos);

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

	if (mDialogDecorWindows)   // only check top priority widget until it passes on
	{
		if (mOk)
		{
			mOk->_cursorPressed(cursorPos);
		}
		else
		{
			mYes->_cursorPressed(cursorPos);
			mNo->_cursorPressed(cursorPos);
		}
		return true;
	}

	mMenuBar->_cursorPressed(cursorPos);
	if (mMenuBar->isMouseOver())
		return true;

	mMiniPlayer->_cursorPressed(cursorPos);
	mTrackList->_cursorPressed(cursorPos);

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




void GuiManager::buttonHit(Button* button)
{
	if (mListener)
	{
		if (button == mOk) mListener->okDialogClosed(mDialogMessage->getText());
		else mListener->yesNoDialogClosed(mDialogMessage->getText(), button == mYes);
	}
	closeDialog();
}

void GuiManager::sliderOptionsMoved(SliderOptions* slider)
{
	if (mAudioPlayer && mAudioPlayer->isPlaying())
	{
		mAudioPlayer->forwardTime(slider->getValue());
	}
}

void GuiManager::trackListHit(TrackList* track)
{
	_playTrack(track->getSelectedTrack());
}

void GuiManager::mediaPlayerMiniHit(MediaPlayerMini* miniPlayer)
{
	switch (miniPlayer->getSelectedAction())
	{
	case (MINI_PLAY):
		if (mAudioPlayer) 
			mAudioPlayer->Play();
		else 
		{
			if (mMiniPlayer->isRandom()) mTrackList->selectRandomTrack();
			else mTrackList->selectTrack(0);
			_playTrack(mTrackList->getSelectedTrack()); 
		}
		break;
	case (MINI_PAUSE):
		if (mAudioPlayer) mAudioPlayer->Pause();
		break;
	case(MINI_STOP) :
		_stopTrack();
		break;
	case(MINI_NEXT) :
		if (mMiniPlayer->isRandom()) mTrackList->selectRandomTrack();
		else mTrackList->selectNextTrack();
		_playTrack(mTrackList->getSelectedTrack());
		break;
	case(MINI_PREV):
		if (mMiniPlayer->isRandom()) mTrackList->selectRandomTrack();
		else mTrackList->selectPrevTrack();
		_playTrack(mTrackList->getSelectedTrack());
		break;
	case(MINI_VOLUME) :
		if (mAudioPlayer)
		{
			if (mMiniPlayer->isSilence()) mAudioPlayer->setVolume(-10000);
			else mAudioPlayer->setVolume(-500);
		}
		break;
	case (MINI_TRACKLIST):
		mTrackList->getOverlayElement()->setHorizontalAlignment(mMiniPlayer->getOverlayElement()->getHorizontalAlignment());
		mTrackList->setTop(mMiniPlayer->getTop() + mMiniPlayer->getHeight() + 10);
		mTrackList->setLeft(mMiniPlayer->getLeft());
		if (mTrackList->isVisible()) mTrackList->hide();
		else mTrackList->show();
		break;
	}
}



void GuiManager::showOkDialog(const Ogre::DisplayString& caption, const Ogre::DisplayString& message)
{
	if (mDialogDecorWindows)
	{
		mDialogDecorWindows->setHeaderCaption(caption);
		mDialogMessage->setText(message);

		if (mOk) return;
		else
		{
			mYes->cleanup();
			mNo->cleanup();
			delete mYes;
			delete mNo;
			mYes = 0;
			mNo = 0;
		}
	}
	else
	{
		// give widgets a chance to reset in case they're in the middle of something
		for (unsigned int i = 0; i < mWidgets.size(); i++)
			mWidgets[i]->_focusLost();

		mDialogShade->show();
		mDialogDecorWindows = new DialogWindows("GuiManager/OkDialog/Windows", caption, 1, 1, 550, 250);
		mDialogShade->addChild(mDialogDecorWindows->getOverlayElement());
		mDialogDecorWindows->getOverlayElement()->setVerticalAlignment(Ogre::GVA_CENTER);
		mDialogDecorWindows->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
		mDialogDecorWindows->getOverlayElement()->setLeft(-mDialogDecorWindows->getOverlayElement()->getWidth() / 2);
		mDialogDecorWindows->getOverlayElement()->setTop(-mDialogDecorWindows->getOverlayElement()->getHeight() / 2);

		mDialogMessage = new SimpleText("GuiManager/OkDialog/Message", message, "YgcFont/SemiBold/16", 19, 520, 250, 7);
		mDialogShade->addChild(mDialogMessage->getOverlayElement());
		mDialogMessage->getOverlayElement()->setVerticalAlignment(Ogre::GVA_CENTER);
		mDialogMessage->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
		mDialogMessage->getOverlayElement()->setLeft(mDialogDecorWindows->getLeft() + 12);
		mDialogMessage->getOverlayElement()->setTop(mDialogDecorWindows->getTop() + 30);
		mDialogDecorWindows->getOverlayElement()->setHeight((mDialogMessage->getNumLines() * (19 + 7)) + 30 + 10);
	}
	mOk = new Button("GuiManager/OkDialog/OkButton", "OK", 1, 1, 60);
	mOk->_assignListener(this);
	mDialogShade->addChild(mOk->getOverlayElement());
	mOk->getOverlayElement()->setVerticalAlignment(Ogre::GVA_CENTER);
	mOk->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
	mOk->getOverlayElement()->setLeft(mDialogDecorWindows->getLeft());
	mOk->getOverlayElement()->setTop(mDialogDecorWindows->getOverlayElement()->getTop() + mDialogDecorWindows->getOverlayElement()->getHeight() + 5);
}

void GuiManager::showYesNoDialog(const Ogre::DisplayString& caption, const Ogre::DisplayString& question)
{
	if (mDialogDecorWindows)
	{
		mDialogDecorWindows->setHeaderCaption(caption);
		mDialogMessage->setText(question);

		if (mOk)
		{
			mOk->cleanup();
			delete mOk;
			mOk = 0;
		}
		else return;
	}
	else
	{
		// give widgets a chance to reset in case they're in the middle of something
		for (unsigned int i = 0; i < mWidgets.size(); i++)
			mWidgets[i]->_focusLost();

		mDialogShade->show();
		mDialogDecorWindows = new DialogWindows("GuiManager/OkDialog/Windows", caption, 1, 1, 550, 250);
		mDialogShade->addChild(mDialogDecorWindows->getOverlayElement());
		mDialogDecorWindows->getOverlayElement()->setVerticalAlignment(Ogre::GVA_CENTER);
		mDialogDecorWindows->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
		mDialogDecorWindows->getOverlayElement()->setLeft(-mDialogDecorWindows->getOverlayElement()->getWidth() / 2);
		mDialogDecorWindows->getOverlayElement()->setTop(-mDialogDecorWindows->getOverlayElement()->getHeight() / 2);

		mDialogMessage = new SimpleText("GuiManager/OkDialog/Message", question, "YgcFont/SemiBold/16", 19, 520, 250, 7);
		mDialogShade->addChild(mDialogMessage->getOverlayElement());
		mDialogMessage->getOverlayElement()->setVerticalAlignment(Ogre::GVA_CENTER);
		mDialogMessage->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
		mDialogMessage->getOverlayElement()->setLeft(mDialogDecorWindows->getLeft() + 12);
		mDialogMessage->getOverlayElement()->setTop(mDialogDecorWindows->getTop() + 30);
		mDialogDecorWindows->getOverlayElement()->setHeight((mDialogMessage->getNumLines() * (19 + 7)) + 30 + 10);
	}

	mYes = new Button("GuiManager/OkDialog/YesButton", "YES", 1, 1, 60);
	mYes->_assignListener(this);
	mDialogShade->addChild(mYes->getOverlayElement());
	mYes->getOverlayElement()->setVerticalAlignment(Ogre::GVA_CENTER);
	mYes->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
	mYes->getOverlayElement()->setLeft(mDialogDecorWindows->getLeft());
	mYes->getOverlayElement()->setTop(mDialogDecorWindows->getOverlayElement()->getTop() + mDialogDecorWindows->getOverlayElement()->getHeight() + 5);

	mNo = new Button("GuiManager/OkDialog/NoButton", "NO", 1, 1, 60);
	mNo->_assignListener(this);
	mDialogShade->addChild(mNo->getOverlayElement());
	mNo->getOverlayElement()->setVerticalAlignment(Ogre::GVA_CENTER);
	mNo->getOverlayElement()->setHorizontalAlignment(Ogre::GHA_CENTER);
	mNo->getOverlayElement()->setLeft(mDialogDecorWindows->getLeft() + 65);
	mNo->getOverlayElement()->setTop(mDialogDecorWindows->getOverlayElement()->getTop() + mDialogDecorWindows->getOverlayElement()->getHeight() + 5);
}

void GuiManager::closeDialog()
{
	if (mDialogDecorWindows)
	{
		if (mOk)
		{
			mOk->cleanup();
			delete mOk;
			mOk = 0;
		}
		else
		{
			mYes->cleanup();
			mNo->cleanup();
			delete mYes;
			delete mNo;
			mYes = 0;
			mNo = 0;
		}

		mDialogShade->hide();

		mDialogMessage->cleanup();
		delete mDialogMessage;
		mDialogMessage = 0;
		mDialogDecorWindows->cleanup();
		delete mDialogDecorWindows;
		mDialogDecorWindows = 0;
		hideBackdrop();
	}
}


void GuiManager::lauchGame(const Ogre::String& pathGame)
{
	SHELLEXECUTEINFO shellExInfo;
	shellExInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shellExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shellExInfo.hwnd = NULL;
	shellExInfo.lpVerb = "open";
	shellExInfo.lpFile = pathGame.c_str();
	shellExInfo.lpParameters = NULL;
	shellExInfo.lpDirectory = NULL;
	shellExInfo.nShow = SW_SHOWNORMAL;
	shellExInfo.hInstApp = NULL;

	// lanza el proceso
	ShellExecuteEx(&shellExInfo);

	mShutDown = true;
}


void GuiManager::_playTrack(const Ogre::String& pathTrack)
{
	if (mAudioPlayer) delete mAudioPlayer;
	mAudioPlayer = new DirectShowSound(const_cast<char *>(pathTrack.c_str()));
	mAudioPlayer->Play();
	mAudioPlayer->setVolume(-500);

	if (mAudioPlayer->isPlaying())
	{
		mMiniPlayer->setSliderRange(0, mAudioPlayer->getDuration(), mAudioPlayer->getDuration() + 1);
		mMiniPlayer->setCaptionTrack(boost::filesystem::path(pathTrack).stem().generic_string());
	}
}

void GuiManager::_stopTrack()
{
	if (mAudioPlayer) { mAudioPlayer->Stop(); delete mAudioPlayer; }
	mAudioPlayer = 0;
	mMiniPlayer->setSliderValue(0, 0, false);
	mMiniPlayer->setCaptionTrack("No track");
	mTrackList->deselectAllTracks();
}

