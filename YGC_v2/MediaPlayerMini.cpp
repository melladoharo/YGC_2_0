#include "stdafx.h"
#include "MediaPlayerMini.h"

MediaPlayerMini::MediaPlayerMini(const Ogre::String& name, Ogre::Real left, Ogre::Real top) :
mNamePlayer(name),
mSelectedAction(MP_STOP),
mRepeatMedia(false), mSilence(false), mRandomMedia(false)
{
	// create background for elements
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	mElement = om.createOverlayElement("Panel", mNamePlayer);
	mElement->setMetricsMode(Ogre::GMM_PIXELS);
	mElement->setWidth(400);
	mElement->setHeight(70 + 33);
	mElement->setHorizontalAlignment(Ogre::GHA_LEFT);
	mElement->setVerticalAlignment(Ogre::GVA_TOP);
	mElement->setTop(top);
	mElement->setLeft(left);
	mElement->setMaterialName("YgcGui/Thumbnail/Dark");
	Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;

	mDecorVol = om.createOverlayElement("Panel", mNamePlayer + "/Element/DecorVol");
	mDecorVol->setMetricsMode(Ogre::GMM_PIXELS);
	mDecorVol->setWidth(24);
	mDecorVol->setHeight(19);
	mDecorVol->setHorizontalAlignment(Ogre::GHA_LEFT);
	mDecorVol->setVerticalAlignment(Ogre::GVA_TOP);
	mDecorVol->setTop(43);
	mDecorVol->setLeft(10);
	mDecorVol->setMaterialName("YgcGui/IconVol/Mat");
	c->addChild(mDecorVol);

	mCaptionMedia = (Ogre::TextAreaOverlayElement*)om.createOverlayElement("TextArea", mNamePlayer + "/TextArea/CaptionMedia");
	mCaptionMedia->setMetricsMode(Ogre::GMM_PIXELS);
	mCaptionMedia->setCaption("No media");
	mCaptionMedia->setFontName("YgcFont/SemiBold/21");
	mCaptionMedia->setCharHeight(20);
	mCaptionMedia->setColour(Ogre::ColourValue(0.84f, 0.85f, 0.84f));
	mCaptionMedia->setTop(12);
	mCaptionMedia->setLeft(10);
	c->addChild(mCaptionMedia);

	mTextTime = (Ogre::TextAreaOverlayElement*)om.createOverlayElement("TextArea", mNamePlayer + "/TextArea/Time");
	mTextTime->setMetricsMode(Ogre::GMM_PIXELS);
	mTextTime->setCaption("99:99/99:99");
	mTextTime->setFontName("YgcFont/SemiBold/21");
	mTextTime->setCharHeight(18);
	mTextTime->setColour(Ogre::ColourValue(0.84f, 0.85f, 0.84f));
	mTextTime->setTop(46);
	mTextTime->setLeft(mElement->getWidth() - sizeInPixels(mTextTime->getCaption(), mTextTime->getFontName(), mTextTime->getCharHeight(), mTextTime->getSpaceWidth()) - 12);
	c->addChild(mTextTime);

	mSlider = new SliderOptions(mNamePlayer + "/Slider/Time", "", -195, 37, 500, 0, 100, 101);
	mSlider->disableElements();
	c->addChild(mSlider->getOverlayElement());

	// media buttons
	mBttVolume.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNamePlayer + "/MiniButton/Volume");
	mBttVolume.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttVolume.panel->setWidth(40);
	mBttVolume.panel->setHeight(33);
	mBttVolume.panel->setTop(mElement->getHeight() - 33);
	mBttVolume.panel->setLeft(mElement->getWidth() - 38);
	mBttVolume.panel->setMaterialName(mSilence ? "YgcGui/MiniVolume/Disable/Up" : "YgcGui/MiniVolume/Enable/Up");
	mBttVolume.currentState = BS_UP;
	mBttVolume.action = MP_VOLUME;
	c->addChild(mBttVolume.panel);

	mBttRandom.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNamePlayer + "/MiniButton/Random");
	mBttRandom.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttRandom.panel->setWidth(40);
	mBttRandom.panel->setHeight(33);
	mBttRandom.panel->setTop(mBttVolume.panel->getTop());
	mBttRandom.panel->setLeft(mBttVolume.panel->getLeft() - 40 - 0.5f);
	mBttRandom.panel->setMaterialName(mRandomMedia ? "YgcGui/MiniRandom/Enable/Up" : "YgcGui/MiniRandom/Disable/Up");
	mBttRandom.currentState = BS_UP;
	mBttRandom.action = MP_RANDOM;
	c->addChild(mBttRandom.panel);

	mBttRepeat.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNamePlayer + "/MiniButton/Repeat");
	mBttRepeat.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttRepeat.panel->setWidth(40);
	mBttRepeat.panel->setHeight(33);
	mBttRepeat.panel->setTop(mBttRandom.panel->getTop());
	mBttRepeat.panel->setLeft(mBttRandom.panel->getLeft() - 40 - 0.5f);
	mBttRepeat.panel->setMaterialName(mRepeatMedia ? "YgcGui/MiniRepeat/Enable/Up" : "YgcGui/MiniRepeat/Disable/Up");
	mBttRepeat.currentState = BS_UP;
	mBttRepeat.action = MP_REPEAT;
	c->addChild(mBttRepeat.panel);

	mBttList.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNamePlayer + "/MiniList/Next");
	mBttList.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttList.panel->setWidth(40);
	mBttList.panel->setHeight(33);
	mBttList.panel->setTop(mBttRepeat.panel->getTop());
	mBttList.panel->setLeft(mBttRepeat.panel->getLeft() - 40 - 0.5f);
	mBttList.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniList/Up");
	mBttList.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniList/Over");
	mBttList.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniList/Down");
	mBttList.panel->setMaterialName(mBttList.matUp->getName());
	mBttList.currentState = BS_UP;
	mBttList.action = MP_TRACKLIST;
	c->addChild(mBttList.panel);

	mBttNext.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNamePlayer + "/MiniButton/Next");
	mBttNext.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttNext.panel->setWidth(40);
	mBttNext.panel->setHeight(33);
	mBttNext.panel->setTop(mBttList.panel->getTop());
	mBttNext.panel->setLeft(mBttList.panel->getLeft() - 40 - 0.5f);
	mBttNext.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniNext/Up");
	mBttNext.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniNext/Over");
	mBttNext.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniNext/Down");
	mBttNext.panel->setMaterialName(mBttNext.matUp->getName());
	mBttNext.currentState = BS_UP;
	mBttNext.action = MP_NEXT;
	c->addChild(mBttNext.panel);

	mBttStop.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNamePlayer + "/MiniButton/Pause");
	mBttStop.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttStop.panel->setWidth(40);
	mBttStop.panel->setHeight(33);
	mBttStop.panel->setTop(mBttNext.panel->getTop());
	mBttStop.panel->setLeft(mBttNext.panel->getLeft() - 40 - 0.5f);
	mBttStop.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniStop/Up");
	mBttStop.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniStop/Over");
	mBttStop.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniStop/Down");
	mBttStop.panel->setMaterialName(mBttStop.matUp->getName());
	mBttStop.currentState = BS_UP;
	mBttStop.action = MP_STOP;
	c->addChild(mBttStop.panel);

	mBttPlay.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNamePlayer + "/MiniButton/Play");
	mBttPlay.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttPlay.panel->setWidth(40);
	mBttPlay.panel->setHeight(33);
	mBttPlay.panel->setTop(mBttStop.panel->getTop());
	mBttPlay.panel->setLeft(mBttStop.panel->getLeft() - 40 - 0.5f);
	mBttPlay.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPlay/Up");
	mBttPlay.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPlay/Over");
	mBttPlay.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPlay/Down");
	mBttPlay.panel->setMaterialName(mBttPlay.matUp->getName());
	mBttPlay.currentState = BS_UP;
	mBttPlay.action = MP_PLAY;
	c->addChild(mBttPlay.panel);

	mBttPrevious.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", mNamePlayer + "/MiniButton/Previous");
	mBttPrevious.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttPrevious.panel->setWidth(40);
	mBttPrevious.panel->setHeight(33);
	mBttPrevious.panel->setTop(mBttPlay.panel->getTop());
	mBttPrevious.panel->setLeft(mBttPlay.panel->getLeft() - 40 - 0.5f);
	mBttPrevious.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPrevious/Up");
	mBttPrevious.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPrevious/Over");
	mBttPrevious.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPrevious/Down");
	mBttPrevious.panel->setMaterialName(mBttPrevious.matUp->getName());
	mBttPrevious.currentState = BS_UP;
	mBttPrevious.action = MP_PREVIOUS;
	c->addChild(mBttPrevious.panel);
}

MediaPlayerMini::~MediaPlayerMini()
{
}

void MediaPlayerMini::_cursorMoved(const Ogre::Vector2& cursorPos)
{
	if (mElement->isVisible())
	{
		mSlider->_cursorMoved(cursorPos);

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

		if (isCursorOver(mBttList.panel, cursorPos))
		{
			if (mBttList.currentState == BS_UP) setState(BS_OVER, mBttList);
		}
		else
		{
			if (mBttList.currentState != BS_UP) setState(BS_UP, mBttList);
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

		if (isCursorOver(mBttRepeat.panel, cursorPos))
		{
			mBttRepeat.currentState = BS_OVER;
			mBttRepeat.panel->setMaterialName(mRepeatMedia ? "YgcGui/MiniRepeat/Enable/Over" : "YgcGui/MiniRepeat/Disable/Over");
		}
		else
		{
			mBttRepeat.currentState = BS_UP;
			mBttRepeat.panel->setMaterialName(mRepeatMedia ? "YgcGui/MiniRepeat/Enable/Up" : "YgcGui/MiniRepeat/Disable/Up");
		}

		if (isCursorOver(mBttRandom.panel, cursorPos))
		{
			mBttRandom.currentState = BS_OVER;
			mBttRandom.panel->setMaterialName(mRandomMedia ? "YgcGui/MiniRandom/Enable/Over" : "YgcGui/MiniRandom/Disable/Over");
		}
		else
		{
			mBttRandom.currentState = BS_UP;
			mBttRandom.panel->setMaterialName(mRandomMedia ? "YgcGui/MiniRandom/Enable/Up" : "YgcGui/MiniRandom/Disable/Up");
		}
	}

}
void MediaPlayerMini::_cursorPressed(const Ogre::Vector2& cursorPos)
{
	mSlider->_cursorPressed(cursorPos);
	if (isCursorOver(mBttPlay.panel, cursorPos)) setState(BS_DOWN, mBttPlay);
	if (isCursorOver(mBttStop.panel, cursorPos)) setState(BS_DOWN, mBttStop);
	if (isCursorOver(mBttPrevious.panel, cursorPos)) setState(BS_DOWN, mBttPrevious);
	if (isCursorOver(mBttNext.panel, cursorPos)) setState(BS_DOWN, mBttNext);
	if (isCursorOver(mBttList.panel, cursorPos)) setState(BS_DOWN, mBttList);
	if (isCursorOver(mBttVolume.panel, cursorPos))
	{
		mBttVolume.currentState = BS_DOWN;
		mSilence = !mSilence;
		mBttVolume.panel->setMaterialName(mSilence ? "YgcGui/MiniVolume/Disable/Over" : "YgcGui/MiniVolume/Enable/Over");
	}
	if (isCursorOver(mBttRepeat.panel, cursorPos))
	{
		mBttRepeat.currentState = BS_DOWN;
		mRepeatMedia = !mRepeatMedia;
		mBttRepeat.panel->setMaterialName(mRepeatMedia ? "YgcGui/MiniRepeat/Enable/Over" : "YgcGui/MiniRepeat/Disable/Over");
	}
	if (isCursorOver(mBttRandom.panel, cursorPos))
	{
		mBttRandom.currentState = BS_DOWN;
		mRandomMedia = !mRandomMedia;
		mBttRandom.panel->setMaterialName(mRandomMedia ? "YgcGui/MiniRandom/Enable/Over" : "YgcGui/MiniRandom/Disable/Over");
	}
}

void MediaPlayerMini::_cursorReleased(const Ogre::Vector2& cursorPos)
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
		if (mListener) mListener->mediaPlayerMiniHit(this);
	}
	if (mBttStop.currentState == BS_DOWN)
	{
		setState(BS_OVER, mBttStop); if (mListener) mListener->mediaPlayerMiniHit(this);
	}
	if (mBttPrevious.currentState == BS_DOWN)
	{
		setState(BS_OVER, mBttPrevious); if (mListener) mListener->mediaPlayerMiniHit(this);
	}
	if (mBttNext.currentState == BS_DOWN)
	{
		setState(BS_OVER, mBttNext); if (mListener) mListener->mediaPlayerMiniHit(this);
	}
	if (mBttList.currentState == BS_DOWN)
	{
		setState(BS_OVER, mBttList); if (mListener) mListener->mediaPlayerMiniHit(this);
	}
}


void MediaPlayerMini::setState(const ButtonState& bs, sMiniPlayerButton& button)
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


