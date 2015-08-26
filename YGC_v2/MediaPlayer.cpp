#include "stdafx.h"
#include "MediaPlayer.h"

MediaPlayer::MediaPlayer(const Ogre::String& namePlayer) : 
mNameMediaPlayer(namePlayer),
mSelectedAction(MP_STOP),
mRepeatMedia(false), mSilence(false), mRandomMedia(false)
{
	// create background for elements
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	mElement = om.createOverlayElement("Panel", mNameMediaPlayer);
	mElement->setMetricsMode(Ogre::GMM_PIXELS);
	mElement->setMaterialName("YgcGui/Thumbnail/Dark");
	mElement->setHorizontalAlignment(Ogre::GHA_LEFT);
	mElement->setVerticalAlignment(Ogre::GVA_BOTTOM);
	mElement->setWidth(Ogre::Root::getSingleton().getAutoCreatedWindow()->getViewport(0)->getActualWidth());
	mElement->setHeight(64);
	mElement->setLeft(0);
	mElement->setTop(-mElement->getHeight());
	Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;

	Ogre::Real sliderWidth = (mElement->getWidth() - 25) * 2;
	Ogre::Real sliderLeft = (-sliderWidth / 2) + 25;
	mSlider = new SliderOptions(mNameMediaPlayer + "/Slider/Time", "", sliderLeft, 2, sliderWidth, 0, 100, 101);
	mSlider->disableElements();
	c->addChild(mSlider->getOverlayElement());

	mTextTime = (Ogre::TextAreaOverlayElement*)om.createOverlayElement("TextArea", mNameMediaPlayer + "/TextArea/Time");
	mTextTime->setMetricsMode(Ogre::GMM_PIXELS);
	mTextTime->setCaption("00:00/00:00");
	mTextTime->setFontName("YgcFont/SemiBold/21");
	mTextTime->setCharHeight(18);
	mTextTime->setColour(Ogre::ColourValue(0.84f, 0.85f, 0.84f));
	mTextTime->setLeft(mElement->getWidth() - sizeInPixels(mTextTime->getCaption(), mTextTime->getFontName(), mTextTime->getCharHeight(), mTextTime->getSpaceWidth()) - 14);
	mTextTime->setTop(36);
	c->addChild(mTextTime);

	// media buttons
	mBttPrevious.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNameMediaPlayer + "/MiniButton/Previous");
	mBttPrevious.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttPrevious.panel->setWidth(40);
	mBttPrevious.panel->setHeight(33);
	mBttPrevious.panel->setTop(26);
	mBttPrevious.panel->setLeft(6);
	mBttPrevious.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPrevious/Up");
	mBttPrevious.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPrevious/Over");
	mBttPrevious.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPrevious/Down");
	mBttPrevious.panel->setMaterialName(mBttPrevious.matUp->getName());
	mBttPrevious.currentState = BS_UP;
	mBttPrevious.action = MP_PREV;
	c->addChild(mBttPrevious.panel);

	mBttPlay.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNameMediaPlayer + "/MiniButton/Play");
	mBttPlay.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttPlay.panel->setWidth(40);
	mBttPlay.panel->setHeight(33);
	mBttPlay.panel->setTop(mBttPrevious.panel->getTop());
	mBttPlay.panel->setLeft(mBttPrevious.panel->getLeft() + 40 + 1);
	mBttPlay.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPlay/Up");
	mBttPlay.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPlay/Over");
	mBttPlay.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPlay/Down");
	mBttPlay.panel->setMaterialName(mBttPlay.matUp->getName());
	mBttPlay.currentState = BS_UP;
	mBttPlay.action = MP_PLAY;
	c->addChild(mBttPlay.panel);

	mBttStop.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNameMediaPlayer + "/MiniButton/Pause");
	mBttStop.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttStop.panel->setWidth(40);
	mBttStop.panel->setHeight(33);
	mBttStop.panel->setTop(mBttPlay.panel->getTop());
	mBttStop.panel->setLeft(mBttPlay.panel->getLeft() + 40 + 1);
	mBttStop.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniStop/Up");
	mBttStop.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniStop/Over");
	mBttStop.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniStop/Down");
	mBttStop.panel->setMaterialName(mBttStop.matUp->getName());
	mBttStop.currentState = BS_UP;
	mBttStop.action = MP_STOP;
	c->addChild(mBttStop.panel);

	mBttNext.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNameMediaPlayer + "/MiniButton/Next");
	mBttNext.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttNext.panel->setWidth(40);
	mBttNext.panel->setHeight(33);
	mBttNext.panel->setTop(mBttStop.panel->getTop());
	mBttNext.panel->setLeft(mBttStop.panel->getLeft() + 40 + 1);
	mBttNext.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniNext/Up");
	mBttNext.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniNext/Over");
	mBttNext.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniNext/Down");
	mBttNext.panel->setMaterialName(mBttNext.matUp->getName());
	mBttNext.currentState = BS_UP;
	mBttNext.action = MP_NEXT;
	c->addChild(mBttNext.panel);

	mBttVolume.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNameMediaPlayer + "/MiniButton/Volume");
	mBttVolume.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttVolume.panel->setWidth(40);
	mBttVolume.panel->setHeight(33);
	mBttVolume.panel->setTop(mBttNext.panel->getTop());
	mBttVolume.panel->setLeft(mBttNext.panel->getLeft() + 40 + 1);
	mBttVolume.panel->setMaterialName(mSilence ? "YgcGui/MiniVolume/Disable/Up" : "YgcGui/MiniVolume/Enable/Up");
	mBttVolume.currentState = BS_UP;
	mBttVolume.action = MP_VOLUME;
	c->addChild(mBttVolume.panel);
}

MediaPlayer::~MediaPlayer()
{
}



void MediaPlayer::_cursorMoved(const Ogre::Vector2& cursorPos)
{
	mSlider->_cursorMoved(cursorPos);

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
			mBttVolume.currentState = BS_OVER;
			mBttVolume.panel->setMaterialName(mSilence ? "YgcGui/MiniVolume/Disable/Over" : "YgcGui/MiniVolume/Enable/Over");
		}
		else
		{
			mBttVolume.currentState = BS_UP;
			mBttVolume.panel->setMaterialName(mSilence ? "YgcGui/MiniVolume/Disable/Up" : "YgcGui/MiniVolume/Enable/Up");
		}
	}
}

void MediaPlayer::_cursorPressed(const Ogre::Vector2& cursorPos)
{
	mSlider->_cursorPressed(cursorPos);

	if (isCursorOver(mBttPlay.panel, cursorPos)) setState(BS_DOWN, mBttPlay);
	if (isCursorOver(mBttStop.panel, cursorPos)) setState(BS_DOWN, mBttStop);
	if (isCursorOver(mBttPrevious.panel, cursorPos)) setState(BS_DOWN, mBttPrevious);
	if (isCursorOver(mBttNext.panel, cursorPos)) setState(BS_DOWN, mBttNext);
	if (isCursorOver(mBttVolume.panel, cursorPos)) { mBttVolume.currentState = BS_DOWN; }
}

void MediaPlayer::_cursorReleased(const Ogre::Vector2& cursorPos)
{
	mSlider->_cursorReleased(cursorPos);

	if (mBttPlay.currentState == BS_DOWN)
	{ 
		if (mBttPlay.matDown->getName() == "YgcGui/MiniPlay/Down")
		{
			mBttPlay.action = MP_PAUSE;
			mBttPlay.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPause/Up");
			mBttPlay.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPause/Over");
			mBttPlay.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPause/Down");
		}
		else
		{
			mBttPlay.action = MP_PLAY;
			mBttPlay.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPlay/Up");
			mBttPlay.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPlay/Over");
			mBttPlay.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPlay/Down");
		}
		setState(BS_OVER, mBttPlay);
		if (mListener) mListener->mediaPlayerHit(this); 
	}
	if (mBttVolume.currentState == BS_DOWN)
	{
		mBttVolume.currentState = BS_OVER;
		mSilence = !mSilence;
		mBttVolume.panel->setMaterialName(mSilence ? "YgcGui/MiniVolume/Disable/Over" : "YgcGui/MiniVolume/Enable/Over");
		mSelectedAction = MP_VOLUME;
		if (mListener) mListener->mediaPlayerHit(this);
	}
	if (mBttStop.currentState == BS_DOWN)
		{ setState(BS_OVER, mBttStop); if (mListener) mListener->mediaPlayerHit(this); }
	if (mBttPrevious.currentState == BS_DOWN)
		{ setState(BS_OVER, mBttPrevious); if (mListener) mListener->mediaPlayerHit(this); }
	if (mBttNext.currentState == BS_DOWN)
		{ setState(BS_OVER, mBttNext); if (mListener) mListener->mediaPlayerHit(this); }
}

void MediaPlayer::setState(const ButtonState& bs, sPlayerButton& button)
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
		mSelectedAction = button.action;
	}

	button.currentState = bs;
}


void MediaPlayer::setSliderRange(Ogre::Real minValue, Ogre::Real maxValue, unsigned int snaps)
{
	mSlider->setRange(minValue, maxValue, snaps);
	mBttPlay.action = MP_PAUSE;
	mBttPlay.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPause/Up");
	mBttPlay.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPause/Over");
	mBttPlay.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPause/Down");
	setState(BS_UP, mBttPlay);
}


void MediaPlayer::setSliderValue(Ogre::Real newVal, Ogre::Real maxValue, bool notifyListener /*= true*/)
{
	mSlider->setValue(newVal, notifyListener);
	 
	int secondsVal = newVal;
	Ogre::String currentSeconds = secondsToString(secondsVal) + ":" + minutesToString(secondsVal);
	secondsVal = maxValue;
	Ogre::String totalSeconds = secondsToString(secondsVal) + ":" + minutesToString(secondsVal);
	
	mTextTime->setCaption(currentSeconds + "/" + totalSeconds);
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

