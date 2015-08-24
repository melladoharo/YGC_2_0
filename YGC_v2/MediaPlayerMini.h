#ifndef _MEDIAPLAYERMINI_H__
#define _MEDIAPLAYERMINI_H__

#include "Widget.h"
#include "SliderOptions.h"

enum ePlayerAction {MP_PLAY, MP_PAUSE, MP_STOP, MP_NEXT, MP_PREVIOUS, MP_REPEAT, MP_RANDOM, MP_VOLUME, MP_TRACKLIST};

struct sMiniPlayerButton
{
	Ogre::PanelOverlayElement* panel;
	Ogre::MaterialPtr matUp, matOver, matDown;
	ButtonState currentState;
	unsigned int action;
};

class MediaPlayerMini : public Widget
{
public:
	MediaPlayerMini(const Ogre::String& name, Ogre::Real left, Ogre::Real top);
	~MediaPlayerMini();

	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _cursorReleased(const Ogre::Vector2& cursorPos);
	void setState(const ButtonState& bs, sMiniPlayerButton& button);

	unsigned int getSelectedAction() { return mSelectedAction; }

private:
	const Ogre::String mNamePlayer;
	SliderOptions* mSlider;
	Ogre::TextAreaOverlayElement* mTextTime;
	Ogre::TextAreaOverlayElement* mCaptionMedia;
	Ogre::OverlayElement* mDecorVol;
	sMiniPlayerButton mBttPlay, mBttStop, mBttNext, mBttPrevious;
	sMiniPlayerButton mBttList, mBttRepeat, mBttRandom, mBttVolume;
	unsigned int mSelectedAction;
	bool mRepeatMedia, mSilence, mRandomMedia;
};

#endif // #ifndef _MEDIAPLAYERMINI_H__

