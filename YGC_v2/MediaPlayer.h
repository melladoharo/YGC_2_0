#ifndef _MEDIAPLAYER_H__
#define _MEDIAPLAYER_H__

#include "Widget.h"
#include "Slider.h"

class MediaPlayer : public Widget
{
public:
	MediaPlayer(const Ogre::String& namePlayer);
	~MediaPlayer();

	// mouse managment
	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _cursorReleased(const Ogre::Vector2& cursorPos);
	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void setState(const ButtonState& bs, sMiniButton& button);

	void setSliderValue(Ogre::Real newVal, Ogre::Real maxValue, bool notifyListener = true);
	Ogre::Real getSliderValue() { return mTimeSlider->getValue(); }
	void setSliderRange(Ogre::Real minValue, Ogre::Real maxValue, unsigned int snaps)
	{
		mTimeSlider->setRange(minValue, maxValue, snaps); 
		mBttPlay.action = "Pause";
		mBttPlay.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPause/Up");
		mBttPlay.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPause/Over");
		mBttPlay.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPause/Down");
		setState(BS_UP, mBttPlay);
	}
	Ogre::String getCurrentAction() { return mCurrentAction; }
	void hide() { mElement->hide(); this->_assignListener(NULL); mTimeSlider->_assignListener(NULL); }
	void show(GuiListener* listener) { mElement->show(); this->_assignListener(listener); mTimeSlider->_assignListener(listener); }

private:
	Ogre::String secondsToString(int totalSeconds);
	Ogre::String minutesToString(int totalSeconds);

	Ogre::String mNameMediaPlayer;
	Slider* mTimeSlider;
	sMiniButton  mBttPlay, mBttStop, mBttNext, 
		mBttPrevious, mBttVolume;
	Ogre::FontPtr mFont; // Font used for items
	Ogre::TextAreaOverlayElement* mTextTime;
	Ogre::String mCurrentAction;
};

#endif // #ifndef _MEDIAPLAYER_H__

