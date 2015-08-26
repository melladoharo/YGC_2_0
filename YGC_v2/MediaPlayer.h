#ifndef _MEDIAPLAYER_H__
#define _MEDIAPLAYER_H__

#include "Widget.h"
#include "SliderOptions.h"

enum ePlayerAction { MP_PLAY, MP_PAUSE, MP_STOP, MP_NEXT, MP_PREV, MP_VOLUME };

struct sPlayerButton
{
	Ogre::PanelOverlayElement* panel;
	Ogre::MaterialPtr matUp, matOver, matDown;
	ButtonState currentState;
	unsigned int action;
};

class MediaPlayer : public Widget
{
public:
	MediaPlayer(const Ogre::String& namePlayer);
	~MediaPlayer();

	// mouse managment
	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _cursorReleased(const Ogre::Vector2& cursorPos);
	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void setState(const ButtonState& bs, sPlayerButton& button);

	unsigned int getSelectedAction() { return mSelectedAction; }
	void setSliderValue(Ogre::Real newVal, Ogre::Real maxValue, bool notifyListener = true);
	Ogre::Real getSliderValue() { return mSlider->getValue(); }
	void setSliderRange(Ogre::Real minValue, Ogre::Real maxValue, unsigned int snaps);
	void _assignSliderListener(GuiListener* listener) { mSlider->_assignListener(listener); }

private:
	Ogre::String secondsToString(int totalSeconds);
	Ogre::String minutesToString(int totalSeconds);

	Ogre::String mNameMediaPlayer;
	SliderOptions* mSlider;
	sPlayerButton  mBttPlay, mBttStop, mBttNext;
	sPlayerButton mBttPrevious, mBttVolume;
	Ogre::TextAreaOverlayElement* mTextTime;
	unsigned int mSelectedAction;
	bool mRepeatMedia, mSilence, mRandomMedia;
};

#endif // #ifndef _MEDIAPLAYER_H__

