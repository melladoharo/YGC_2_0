#include "stdafx.h"
#include "MediaPlayer.h"

MediaPlayer::MediaPlayer(const Ogre::String& namePlayer) : 
mNameMediaPlayer(namePlayer),
mCurrentAction(Ogre::StringUtil::BLANK),
mFont(Ogre::FontManager::getSingletonPtr()->getByName("YgcFont/SemiBold/21"))
{
	// create background for elements
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	mElement = om.createOverlayElement("Panel", mNameMediaPlayer);
	mElement->setMetricsMode(Ogre::GMM_PIXELS);
	mElement->setWidth(Ogre::Root::getSingleton().getAutoCreatedWindow()->getViewport(0)->getActualWidth());
	mElement->setHeight(42);
	mElement->setHorizontalAlignment(Ogre::GHA_LEFT);
	mElement->setVerticalAlignment(Ogre::GVA_BOTTOM);
	mElement->setTop(-mElement->getHeight());
	mElement->setMaterialName("YgcGui/Thumbnail");
	Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;

	// media buttons
	mBttPlay.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNameMediaPlayer + "/Button/Play");
	mBttPlay.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttPlay.panel->setWidth(32);
	mBttPlay.panel->setHeight(32);
	mBttPlay.panel->setTop(5);
	mBttPlay.panel->setLeft(8);
	mBttPlay.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPlay/Up");
	mBttPlay.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPlay/Over");
	mBttPlay.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPlay/Down");
	mBttPlay.panel->setMaterialName(mBttPlay.matUp->getName());
	mBttPlay.currentState = BS_UP;
	mBttPlay.action = "Play";
	c->addChild(mBttPlay.panel);

	mBttStop.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNameMediaPlayer + "/Button/Pause");
	mBttStop.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttStop.panel->setWidth(32);
	mBttStop.panel->setHeight(32);
	mBttStop.panel->setTop(mBttPlay.panel->getTop());
	mBttStop.panel->setLeft(mBttPlay.panel->getLeft() + mBttPlay.panel->getWidth() + 7);
	mBttStop.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniStop/Up");
	mBttStop.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniStop/Over");
	mBttStop.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniStop/Down");
	mBttStop.panel->setMaterialName(mBttStop.matUp->getName());
	mBttStop.currentState = BS_UP;
	mBttStop.action = "Stop";
	c->addChild(mBttStop.panel);

	mBttPrevious.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNameMediaPlayer + "/Button/Previous");
	mBttPrevious.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttPrevious.panel->setWidth(32);
	mBttPrevious.panel->setHeight(32);
	mBttPrevious.panel->setTop(mBttPlay.panel->getTop());
	mBttPrevious.panel->setLeft(mBttStop.panel->getLeft() + mBttStop.panel->getWidth() + 7);
	mBttPrevious.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPrevious/Up");
	mBttPrevious.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPrevious/Over");
	mBttPrevious.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPrevious/Down");
	mBttPrevious.panel->setMaterialName(mBttPrevious.matUp->getName());
	mBttPrevious.currentState = BS_UP;
	mBttPrevious.action = "Previous";
	c->addChild(mBttPrevious.panel);

	mBttNext.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNameMediaPlayer + "/Button/Next");
	mBttNext.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttNext.panel->setWidth(32);
	mBttNext.panel->setHeight(32);
	mBttNext.panel->setTop(mBttPlay.panel->getTop());
	mBttNext.panel->setLeft(mBttPrevious.panel->getLeft() + mBttPrevious.panel->getWidth() + 7);
	mBttNext.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniNext/Up");
	mBttNext.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniNext/Over");
	mBttNext.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniNext/Down");
	mBttNext.panel->setMaterialName(mBttNext.matUp->getName());
	mBttNext.currentState = BS_UP;
	mBttNext.action = "Next";
	c->addChild(mBttNext.panel);

	mTextTime = (Ogre::TextAreaOverlayElement*)om.createOverlayElement("TextArea", mNameMediaPlayer + "/TextArea/Time");
	mTextTime->setMetricsMode(Ogre::GMM_PIXELS);
	mTextTime->setCaption("99:99 | 99:99");
	mTextTime->setFontName(mFont->getName());
	mTextTime->setCharHeight(21);
	mTextTime->setColour(Ogre::ColourValue(0.84f, 0.85f, 0.84f));
	c->addChild(mTextTime);

	mBttVolume.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNameMediaPlayer + "/Button/Silence");
	mBttVolume.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttVolume.panel->setWidth(32);
	mBttVolume.panel->setHeight(32);
	mBttVolume.panel->setTop(mBttPlay.panel->getTop());
	mBttVolume.panel->setLeft(mElement->getWidth() - 32 - 8);
	mBttVolume.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniVol/Up");
	mBttVolume.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniVol/Over");
	mBttVolume.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniVol/Down");
	mBttVolume.panel->setMaterialName(mBttVolume.matUp->getName());
	mBttVolume.currentState = BS_UP;
	mBttVolume.action = "Silence";
	c->addChild(mBttVolume.panel);

	// positions
	Ogre::Real sizeTimer = sizeInPixels(mTextTime->getCaption(), mTextTime->getFontName(), mTextTime->getCharHeight(), mTextTime->getSpaceWidth());
	mTextTime->setLeft(mBttVolume.panel->getLeft() - sizeTimer - 26);
	mTextTime->setTop(mBttStop.panel->getTop() + 9);
	// time slider
	Ogre::Real sliderWidth = mTextTime->getLeft() - mBttNext.panel->getLeft() - mBttNext.panel->getWidth() - 30;
	mTimeSlider = new Slider(mNameMediaPlayer + "/Slider", sliderWidth, 0, 100, 100);
	mTimeSlider->setTop(mBttPlay.panel->getTop() + 11);
	mTimeSlider->setLeft(mBttNext.panel->getLeft() + mBttNext.panel->getWidth() + 15);
	c->addChild(mTimeSlider->getOverlayElement());
}

MediaPlayer::~MediaPlayer()
{
}



void MediaPlayer::_cursorMoved(const Ogre::Vector2& cursorPos)
{
	mTimeSlider->_cursorMoved(cursorPos);

	if (mElement->isVisible())
	{
		if (isCursorOver(mBttPlay.panel, cursorPos))
		{
			if (mBttPlay.currentState == BS_UP) setState(BS_OVER, mBttPlay);
		}
		else
		{
			if (mBttPlay.currentState != BS_UP) setState(BS_UP, mBttPlay);
		}

		if (isCursorOver(mBttStop.panel, cursorPos))
		{
			if (mBttStop.currentState == BS_UP) setState(BS_OVER, mBttStop);
		}
		else
		{
			if (mBttStop.currentState != BS_UP) setState(BS_UP, mBttStop);
		}

		if (isCursorOver(mBttPrevious.panel, cursorPos))
		{
			if (mBttPrevious.currentState == BS_UP) setState(BS_OVER, mBttPrevious);
		}
		else
		{
			if (mBttPrevious.currentState != BS_UP) setState(BS_UP, mBttPrevious);
		}

		if (isCursorOver(mBttNext.panel, cursorPos))
		{
			if (mBttNext.currentState == BS_UP) setState(BS_OVER, mBttNext);
		}
		else
		{
			if (mBttNext.currentState != BS_UP) setState(BS_UP, mBttNext);
		}

		if (isCursorOver(mBttVolume.panel, cursorPos))
		{
			if (mBttVolume.currentState == BS_UP) setState(BS_OVER, mBttVolume); return;
		}
		else
		{
			if (mBttVolume.currentState != BS_UP) setState(BS_UP, mBttVolume);
		}
	}
}

void MediaPlayer::_cursorPressed(const Ogre::Vector2& cursorPos)
{
	mTimeSlider->_cursorPressed(cursorPos);

	if (isCursorOver(mBttPlay.panel, cursorPos)) setState(BS_DOWN, mBttPlay);
	if (isCursorOver(mBttStop.panel, cursorPos)) setState(BS_DOWN, mBttStop);
	if (isCursorOver(mBttPrevious.panel, cursorPos)) setState(BS_DOWN, mBttPrevious);
	if (isCursorOver(mBttNext.panel, cursorPos)) setState(BS_DOWN, mBttNext);
	if (isCursorOver(mBttVolume.panel, cursorPos)) setState(BS_DOWN, mBttVolume);
}

void MediaPlayer::_cursorReleased(const Ogre::Vector2& cursorPos)
{
	mTimeSlider->_cursorReleased(cursorPos);

	if (mBttPlay.currentState == BS_DOWN)
	{ 
		if (mBttPlay.matDown->getName() == "YgcGui/MiniPlay/Down")
		{
			mBttPlay.action = "Pause";
			mBttPlay.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPause/Up");
			mBttPlay.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPause/Over");
			mBttPlay.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPause/Down");
		}
		else
		{
			mBttPlay.action = "Play";
			mBttPlay.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPlay/Up");
			mBttPlay.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPlay/Over");
			mBttPlay.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPlay/Down");
		}
		setState(BS_OVER, mBttPlay);
		if (mListener) mListener->mediaPlayerHit(this); 
	}
	if (mBttStop.currentState == BS_DOWN)
		{ setState(BS_OVER, mBttStop); if (mListener) mListener->mediaPlayerHit(this); }
	if (mBttPrevious.currentState == BS_DOWN)
		{ setState(BS_OVER, mBttPrevious); if (mListener) mListener->mediaPlayerHit(this); }
	if (mBttNext.currentState == BS_DOWN)
		{ setState(BS_OVER, mBttNext); if (mListener) mListener->mediaPlayerHit(this); }
	if (mBttVolume.currentState == BS_DOWN)
		{ setState(BS_OVER, mBttVolume); if (mListener) mListener->mediaPlayerHit(this); }
}

void MediaPlayer::setState(const ButtonState& bs, sMiniButton& button)
{
	if (bs == BS_OVER)
	{
		button.panel->setMaterialName(button.matOver->getName());
	}
	else if (bs == BS_UP)
	{
		button.panel->setMaterialName(button.matUp->getName());
	}
	else
	{
		button.panel->setMaterialName(button.matDown->getName());
		mCurrentAction = button.action;
	}

	button.currentState = bs;
}

void MediaPlayer::setSliderValue(Ogre::Real newVal, Ogre::Real maxValue, bool notifyListener /*= true*/)
{
	mTimeSlider->setValue(newVal, notifyListener);
	 
	int secondsVal = newVal;
	Ogre::String currentSeconds = secondsToString(secondsVal) + ":" + minutesToString(secondsVal);
	secondsVal = maxValue;
	Ogre::String totalSeconds = secondsToString(secondsVal) + ":" + minutesToString(secondsVal);
	
	mTextTime->setCaption(currentSeconds + " | " + totalSeconds);
}

Ogre::String MediaPlayer::secondsToString(int totalSeconds)
{
	Ogre::String strSeconds = Ogre::StringConverter::toString(totalSeconds / 60);
	if (strSeconds.size() == 1)
		strSeconds = "0" + strSeconds;

	return strSeconds;
}

Ogre::String MediaPlayer::minutesToString(int totalSeconds)
{
	Ogre::String strSeconds = Ogre::StringConverter::toString(totalSeconds % 60);
	if (strSeconds.size() == 1)
		strSeconds = "0" + strSeconds;

	return strSeconds;
}

