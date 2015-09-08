#ifndef _GUIMANAGER_H__
#define _GUIMANAGER_H__

#include "Widget.h"
#include "Button.h"
#include "MenuBar.h"
#include "Thumbnail.h"
#include "Slider.h"
#include "SimpleText.h"
#include "DecorWidget.h"
#include "SelectMenu.h"
#include "TrackList.h"
#include "ProgressBar.h"
#include "MediaPlayer.h"
#include "MediaPlayerMini.h"
#include "CheckBox.h"
#include "ItemSelector.h"
#include "Label.h"
#include "DialogWindows.h"
#include "SliderOptions.h"
#include "FileExplorer.h"
#include "LineEdit.h"
#include "DepthOfFieldEffect.h"
#include "UtilsOgreDshow.h"
#include "ConfigReader.h"

class GuiManager : public GuiListener, public Ogre::ResourceGroupListener
{
public:
	GuiManager(const Ogre::String& name, Ogre::RenderWindow* window, Ogre::Camera* camera, 
		OIS::Mouse* mouse, OIS::Keyboard* keyboard, GuiListener* listener = 0);
	virtual ~GuiManager();

	// these methods get the underlying overlays and overlay elements
	Ogre::Overlay* getBackdropLayer() { return mBackdropLayer; }
	Ogre::Overlay* getCursorLayer() { return mCursorLayer; }
	Ogre::OverlayContainer* getBackdropContainer() { return mBackdrop; }
	Ogre::OverlayContainer* getCursorContainer() { return mCursor; }
	Ogre::OverlayElement* getCursorImage() { return mCursor->getChild(mCursor->getName() + "/CursorImage"); }

	// MenuBar
	void assignListenerToMenuBar(GuiListener* listener) { mMenuBar->_assignListener(listener); }
	void callListenerMenuBar() { mMenuBar->_forceCursorReleased(); }
	Ogre::Real getMenuBarHeight() { return mMenuBar->getHeight(); }
	void loadMenuBarMain(unsigned int selOption = 0) { if (mMenuBar) mMenuBar->createMenuMain(selOption); }
	void loadMenuBarGame() { if (mMenuBar) mMenuBar->createMenuGame(); }
	void loadMenuBarNewGame() { if (mMenuBar) mMenuBar->createMenuNewGame(); }
	void loadMenuBarSaga() { if (mMenuBar) mMenuBar->createMenuSaga(); }
	void loadMenuBarNewSaga() { if (mMenuBar) mMenuBar->createMenuNewSaga(); }

	// System
	void setMouseEventCallback(OIS::MouseListener* mouseListener) { mMouse->setEventCallback(mouseListener); }
	void setKeyboardEventCallback(OIS::KeyListener* keyboardListener) { mKeyboard->setEventCallback(keyboardListener); }
	void setListener(GuiListener* listener) { mListener = listener; }
	GuiListener* getListener() { return mListener; }
	bool isShutDownEnabled() { return mShutDown; }
	void shutDownApp() { mShutDown = true; }
	void createScreenshot() { mWindow->writeContentsToTimestampedFile("screenshot", ".jpg"); }

	// Depth of field effect
	void setDofEffectManager(DOFManager* dofmgr) { mDofEffect = dofmgr; }
	void setDofEffectEnable(bool enableDof) { if (mDofEffect) mDofEffect->setEnabled(enableDof); }
	bool getDofEffectEnable() { return (mDofEffect) ? mDofEffect->getEnabled() : false; }
	void setDofEffectFocus(Ogre::Real newFocus) { if (mDofEffect) mDofEffect->setFocus(newFocus); }
	Ogre::Real getDofEffectFocus() { return (mDofEffect) ? mDofEffect->getFocus() : 0; }
	void setDofEffectAperture(Ogre::Real newAperture) { if (mDofEffect) mDofEffect->setAperture(newAperture); }
	Ogre::Real getDofEffectAperture() { return (mDofEffect) ? mDofEffect->getAperture() : 0; }

	// media player and miniplayer
	MediaPlayerMini* getDefaultMiniPlayer() { return mMiniPlayer; }
	TrackList* getDefaultTrackList() { return mTrackList; }
	bool isMiniPlaying() { return (mAudioPlayer && mAudioPlayer->isPlaying()); }
	void playMiniPlayer() { if (mAudioPlayer && mAudioPlayer->isPaused()) mAudioPlayer->Play(); }
	void pauseMiniPlayer() { if (mAudioPlayer) mAudioPlayer->Pause(); }
	void stopMiniPlayer() { _stopTrack(); hideMiniPlayer(); }
	void startMiniPlayer() 
	{ 
		if (mMiniPlayer->isRandom()) mTrackList->selectRandomTrack();
		else mTrackList->selectTrack(0);
		_playTrack(mTrackList->getSelectedTrack()); 
	}

	// VISIBILITY
	void showWidgets() { mWidgetsLayer->show(); }
	void hideWidgets() { mWidgetsLayer->hide(); }
	void showMenuBar() { mMenuBar->show(); }
	void hideMenuBar() { mMenuBar->hide(); }
	void showMiniPlayer() { mAutoHidePlayer = 3; mMiniPlayer->show(); mTrackList->hide(); }
	void hideMiniPlayer() { mMiniPlayer->hide(); mTrackList->hide(); }
	void showTrackList() { mTrackList->show(); }
	void hideTrackList() { mTrackList->hide(); }
	void showAll() { showBackdrop(); showMenuBar(); showWidgets(); showCursor(); }
	void hideAll() { hideBackdrop(); hideMenuBar(); hideWidgets(); hideCursor(); }
	bool isCursorVisible() { return mCursorLayer->isVisible(); }
	bool isBackdropVisible() { return mBackdropLayer->isVisible(); }
	bool isMenuBarVisible() { return mMenuBar->isVisible(); }
	bool isWidgetsVisible() { return mWidgetsLayer->isVisible(); }
	bool isWindowDialogVisible() { return mWindowsLayer->isVisible(); }
	bool isDialogVisible() { return mDialogDecorWindows != 0; }
	bool isMiniPlayerVisible() { return mMiniPlayer->isVisible(); }

	/*-----------------------------------------------------------------------------
	| Displays specified material on backdrop, or the last material used if
	| none specified. Good for pause menus like in the browser.
	-----------------------------------------------------------------------------*/
	void showBackdrop(const Ogre::String& materialName = Ogre::StringUtil::BLANK)
	{
		if (materialName != Ogre::StringUtil::BLANK) mBackdrop->setMaterialName(materialName);
		mBackdropLayer->show();
	}
	void hideBackdrop() { mBackdropLayer->hide(); }
	void autoResizeBackdrop();
	
	
	/*	Displays specified material on cursor, or the last material used if
		none specified. Used to change cursor type	*/
	void showCursor(const Ogre::String& materialName = Ogre::StringUtil::BLANK);

	void hideCursor();

	void showProgressBar();
	void hideProgressBar();
	void setProgressBarProgress(Ogre::Real progress); 
	void setProgressBarCaption(const Ogre::DisplayString& caption);

	/*-----------------------------------------------------------------------------
	| Updates cursor position based on unbuffered mouse state. This is necessary
	| because if the tray manager has been cut off from mouse events for a time,
	| the cursor position will be out of date.
	-----------------------------------------------------------------------------*/
	void refreshCursor();

	// Gets a widget by name.
	Widget* getWidget(const Ogre::String& name);
	
	// Gets the number of widgets in total.
	unsigned int getNumWidgets() { return mWidgets.size(); }

	/*-----------------------------------------------------------------------------
	| Destroys a widget.
	-----------------------------------------------------------------------------*/
	void destroyWidget(Widget* widget);

	void destroyWidget(const Ogre::String& name)
	{
		destroyWidget(getWidget(name));
	}

	void destroyAllWidgets()
	{
		// destroy every widget
		for (unsigned int i = 0; i < mWidgets.size(); i++)  
		{
			destroyWidget(mWidgets[i]);
		}
	}

	void destroyDialogWindow(Widget* windows);
	void destroyDialogWindow(const Ogre::String& name) { destroyDialogWindow(getWidget(name)); }

	// Listener
	void buttonHit(Button* button);
	void mediaPlayerMiniHit(MediaPlayerMini* miniPlayer);
	void trackListHit(TrackList* track);
	void sliderOptionsMoved(SliderOptions* slider);

	// Fade effect
	void enableFadeEffect()
	{
		mRenderText->update();
		mMatRtt->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(
			Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, 1);
		showBackdrop(mMatRtt->getName());
		autoResizeBackdrop();
		mFadeAmount = 1;
	}


	/*-----------------------------------------------------------------------------
	| Process frame events. Updates frame statistics widget set and deletes
	| all widgets queued for destruction.
	-----------------------------------------------------------------------------*/
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	void windowUpdate()
	{
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS && OGRE_PLATFORM != OGRE_PLATFORM_NACL
		mWindow->update();
#endif
	}

	void resourceGroupScriptingStarted(const Ogre::String& groupName, size_t scriptCount)
	{
		//mLoadInc = mGroupInitProportion / scriptCount;
		//mLoadBar->setCaption("Parsing...");
		windowUpdate();
	}

	void scriptParseStarted(const Ogre::String& scriptName, bool& skipThisScript)
	{
		//mLoadBar->setComment(scriptName);
		windowUpdate();
	}

	void scriptParseEnded(const Ogre::String& scriptName, bool skipped)
	{
		//mLoadBar->setProgress(mLoadBar->getProgress() + mLoadInc);
		windowUpdate();
	}

	void resourceGroupScriptingEnded(const Ogre::String& groupName) {}

	void resourceGroupLoadStarted(const Ogre::String& groupName, size_t resourceCount)
	{
		//mLoadInc = mGroupLoadProportion / resourceCount;
		//mLoadBar->setCaption("Loading...");
		windowUpdate();
	}

	void resourceLoadStarted(const Ogre::ResourcePtr& resource)
	{
		//mLoadBar->setComment(resource->getName());
		windowUpdate();
	}

	void resourceLoadEnded()
	{
		//mLoadBar->setProgress(mLoadBar->getProgress() + mLoadInc);
		windowUpdate();
	}

	void worldGeometryStageStarted(const Ogre::String& description)
	{
		//mLoadBar->setComment(description);
		windowUpdate();
	}

	void worldGeometryStageEnded()
	{
		//mLoadBar->setProgress(mLoadBar->getProgress() + mLoadInc);
		windowUpdate();
	}

	void resourceGroupLoadEnded(const Ogre::String& groupName) {}

	/*-----------------------------------------------------------------------------
	| Updates cursor position. Returns true if the event was
	| consumed and should not be passed on to other handlers.
	-----------------------------------------------------------------------------*/
	bool injectMouseMove(const OIS::MouseEvent& evt);
	/*-----------------------------------------------------------------------------
	| Processes mouse button down events. Returns true if the event was
	| consumed and should not be passed on to other handlers.
	-----------------------------------------------------------------------------*/
	bool injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
	/*-----------------------------------------------------------------------------
	| Processes mouse button up events. Returns true if the event was
	| consumed and should not be passed on to other handlers.
	-----------------------------------------------------------------------------*/
	bool injectMouseUp(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
	

	Button* createButton(const Ogre::String& name, const Ogre::String& caption, Ogre::Real left, Ogre::Real top, Ogre::Real width = 0)
	{
		Button* b = new Button(name, caption, left, top, width);
		mWidgets.push_back(b);
		mTray->addChild(mWidgets.back()->getOverlayElement());
		b->_assignListener(mListener);
		return b;
	}

	Thumbnail* createThumbnail(const Ogre::String& name, const Ogre::String& nameGroupRes, const Ogre::String& caption, const Ogre::String& textName, int size)
	{
		Thumbnail* t = new Thumbnail(name, nameGroupRes, caption, textName, size);
		mWidgets.push_back(t);
		mTray->addChild(mWidgets.back()->getOverlayElement());
		t->_assignListener(mListener);
		return t;
	}

	Slider* createSlider(const Ogre::String& name, Ogre::Real width, Ogre::Real minValue, Ogre::Real maxValue, unsigned int snaps)
	{
		Slider* s = new Slider(name, width, minValue, maxValue, snaps);
		mWidgets.push_back(s);
		mTray->addChild(mWidgets.back()->getOverlayElement());
		s->_assignListener(mListener);
		return s;
	}

	SimpleText* createSimpleText(const Ogre::String& name, const Ogre::String& caption, const Ogre::String& nameFont,
		Ogre::Real width, Ogre::Real height, int oveCharHeight, Ogre::Real oveLineSep)
	{
		SimpleText* st = new SimpleText(name, caption, nameFont, oveCharHeight, width, height, oveLineSep);
		mWidgets.push_back(st);
		mTray->addChild(mWidgets.back()->getOverlayElement());
		st->_assignListener(mListener);
		return st;
	}

	DecorWidget* createDecorWidget(const Ogre::String& name, const Ogre::String& templateName)
	{
		DecorWidget* dw = new DecorWidget(name, templateName);
		mWidgets.push_back(dw);
		mTray->addChild(mWidgets.back()->getOverlayElement());
		//dw->_assignListener(mListener);
		return dw;
	}

	SelectMenu* createSelectMenu(const Ogre::String& name, const Ogre::StringVector& items, Ogre::Real width)
	{
		SelectMenu* sm = new SelectMenu(name, items, width);
		mWidgets.push_back(sm);
		mTray->addChild(mWidgets.back()->getOverlayElement());
		sm->_assignListener(mListener);
		return sm;
	}

	CheckBox* createCheckBox(const Ogre::String& name, const Ogre::DisplayString& caption, Ogre::Real left, Ogre::Real top, Ogre::Real width)
	{
		CheckBox* cb = new CheckBox(name, caption, left, top, width);
		mWidgets.push_back(cb);
		mTray->addChild(mWidgets.back()->getOverlayElement());
		cb->_assignListener(mListener);
		return cb;
	}

	ItemSelector* createItemSelector(const Ogre::String& name, const Ogre::DisplayString& caption, const Ogre::StringVector& items, Ogre::Real left, Ogre::Real top, Ogre::Real width)
	{
		ItemSelector* is = new ItemSelector(name, caption, items, left, top, width);
		mWidgets.push_back(is);
		mTray->addChild(mWidgets.back()->getOverlayElement());
		is->_assignListener(mListener);
		return is;
	}

	Label* createLabel(const Ogre::String& name, const Ogre::DisplayString& caption, Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real charHeight)
	{
		Label* lbl = new Label(name, caption, left, top, width, charHeight);
		mWidgets.push_back(lbl);
		mTray->addChild(mWidgets.back()->getOverlayElement());
		lbl->_assignListener(mListener);
		return lbl;
	}

	SliderOptions* createSliderOptions(const Ogre::String& name, const Ogre::String& caption, Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real minValue, Ogre::Real maxValue, unsigned int snaps)
	{
		SliderOptions* so = new SliderOptions(name, caption, left, top, width, minValue, maxValue, snaps);
		mWidgets.push_back(so);
		mTray->addChild(mWidgets.back()->getOverlayElement());
		so->_assignListener(mListener);
		return so;
	}

	LineEdit* createLineEdit(const Ogre::String& name, const Ogre::String& caption, const Ogre::String& message, Ogre::Real left, Ogre::Real top, Ogre::Real width)
	{
		LineEdit* le = new LineEdit(name, caption, message, left, top, width);
		mWidgets.push_back(le);
		mTray->addChild(mWidgets.back()->getOverlayElement());
		le->_assignListener(mListener);
		return le;
	}

	FileExplorer* createFileExplorer(const Ogre::String& name, const Ogre::String& path, Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real height)
	{
		FileExplorer* fe = new FileExplorer(name, path, left, top, width, height);
		mWidgets.push_back(fe);
		mTray->addChild(mWidgets.back()->getOverlayElement());
		fe->_assignListener(mListener);
		return fe;
	}

	ProgressBar* createProgressBar(const Ogre::String& name, Ogre::Real width)
	{
		ProgressBar* pb = new ProgressBar(name, width);
		mWidgets.push_back(pb);
		mTray->addChild(mWidgets.back()->getOverlayElement());
		return pb;
	}

	// manual widget
	MediaPlayer* createMediaPlayer(const Ogre::String& namePlayer)
	{
		MediaPlayer* mp = new MediaPlayer(namePlayer);
		//mWidgets.push_back(mp);
		mPlayerTray->addChild(mp->getOverlayElement());
		mp->_assignListener(mListener);
		return mp;
	}
	void destroyMediaPlayer(MediaPlayer* player);

	MediaPlayerMini* createMediaPlayerMini(const Ogre::String& name, Ogre::Real left, Ogre::Real top)
	{
		MediaPlayerMini* mpm = new MediaPlayerMini(name, left, top);
		mWidgets.push_back(mpm);
		mTray->addChild(mWidgets.back()->getOverlayElement());
		mpm->_assignListener(mListener);
		return mpm;
	}

	TrackList* createTrackList(const Ogre::String& name, const Ogre::StringVector& tracks, Ogre::Real left, Ogre::Real top, unsigned int maxItemShown)
	{
		TrackList* tl = new TrackList(name, tracks, left, top, maxItemShown);
		mWidgets.push_back(tl);
		mTray->addChild(mWidgets.back()->getOverlayElement());
		tl->_assignListener(mListener);
		return tl;
	}

	DialogWindows* createDialogWindow(const Ogre::String& name, const Ogre::DisplayString& caption, Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real height)
	{
		DialogWindows* dw = new DialogWindows(name, caption, left, top, width, height);
		mWidgets.push_back(dw);
		mDialogWindows->addChild(dw->getOverlayElement());
		mWindowsLayer->show();
		return dw;
	}
	
	// Pops up a message dialog with an OK button
	void showOkDialog(const Ogre::DisplayString& caption, const Ogre::DisplayString& message);
	// Pops up a question dialog with Yes and No buttons
	void showYesNoDialog(const Ogre::DisplayString& caption, const Ogre::DisplayString& question);
	// Hides whatever dialog is currently showing
	void closeDialog();

	// Returns a 3D ray into the scene that is directly underneath the cursor.
	Ogre::Ray getCursorRay(Ogre::Camera* cam)
	{
		return screenToScene(cam, Ogre::Vector2(mCursor->_getLeft(), mCursor->_getTop()));
	}

	// converts a 2D screen coordinate (in pixels) to a 3D ray into the scene.
	static Ogre::Ray screenToScene(Ogre::Camera* cam, const Ogre::Vector2& pt)
	{
		return cam->getCameraToViewportRay(pt.x, pt.y);
	}
	// converts a 3D scene position to a 2D screen position (in relative screen size, 0.0-1.0).
	static Ogre::Vector2 sceneToScreen(Ogre::Camera* cam, const Ogre::Vector3& pt)
	{
		Ogre::Vector3 result = cam->getProjectionMatrix() * cam->getViewMatrix() * pt;
		return Ogre::Vector2((result.x + 1) / 2, (-result.y + 1) / 2);
	}

protected:
	void _playTrack(const Ogre::String& pathTrack);
	void _stopTrack();
	void setExpandedMenu(SelectMenu* m);
	
	Ogre::String mName;                   // name of this tray system
	Ogre::RenderWindow* mWindow;          // render window
	Ogre::Camera* mCamera;				  // camera
	OIS::Mouse* mMouse;                   // mouse device
	OIS::Keyboard* mKeyboard;			  // keyboard device
	Ogre::Overlay* mBackdropLayer;        // backdrop layer
	Ogre::Overlay* mPriorityLayer;        // top priority layer
	Ogre::Overlay* mWidgetsLayer;         // widget layer
	Ogre::Overlay* mCursorLayer;          // cursor layer
	Ogre::Overlay* mMenuBarLayer;		  // menu bar layer
	Ogre::Overlay* mWindowsLayer;
	Ogre::Overlay* mPlayerLayer;		  // media player layer
	Ogre::OverlayContainer* mBackdrop;    // backdrop
	Ogre::OverlayContainer* mCursor;      // cursor
	Ogre::OverlayContainer* mTray;
	Ogre::OverlayContainer* mDialogWindows;
	Ogre::OverlayContainer* mDialogShade; // top priority dialog shade
	Ogre::OverlayContainer* mPlayerTray;
	WidgetList mWidgets;			      // widgets
	WidgetList mWidgetDeathRow;           // widget queue for deletion
	GuiListener* mListener;				  // tray listener
	MenuBar* mMenuBar;
	MediaPlayer* mMediaPlayer;
	MediaPlayerMini* mMiniPlayer;
	TrackList* mTrackList;
	SelectMenu* mExpandedMenu;            // top priority expanded menu widget
	ProgressBar* mLoadBar;				  // progress bar widget
	DialogWindows* mDialogDecorWindows;
	Button* mOk, *mYes, *mNo;
	SimpleText* mDialogMessage;
	DOFManager* mDofEffect;				  // Depth of field effect
	DirectShowSound* mAudioPlayer;
	Ogre::RenderTexture* mRenderText;
	Ogre::TexturePtr mTextRtt;
	Ogre::MaterialPtr mMatRtt;
	Ogre::Timer* mTimer;                  // Root::getSingleton().getTimer()
	Ogre::Real mFadeAmount;				  // [0 - 1]  [not-visible - visible]
	Ogre::Real mAutoHidePlayer;
	unsigned long mLastStatUpdateTime;    // The last time the stat text were updated
	bool mCursorWasVisible;               // cursor state before showing dialog
	bool mCursorOverPlayer;
	bool mShutDown;
};

#endif // #define _GUIMANAGER_H__

