#ifndef _MEDIAPLAYERMINI_H__
#define _MEDIAPLAYERMINI_H__

#include "Widget.h"
#include "SliderOptions.h"

class MediaPlayerMini : public Widget
{
public:
	MediaPlayerMini();
	~MediaPlayerMini();

	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _cursorReleased(const Ogre::Vector2& cursorPos);
	void setState(const ButtonState& bs, sMiniButton& button);

private:
	const Ogre::String mNamePlayer;
	SliderOptions* mSlider;
	Ogre::FontPtr mFont;
	Ogre::TextAreaOverlayElement* mTextTime;
	Ogre::TextAreaOverlayElement* mCaptionMedia;
	Ogre::OverlayElement* mDecorVol;
	sMiniButton mBttPlay, mBttStop, mBttNext, mBttPrevious;
	sMiniButton mBttList, mBttRepeat, mBttRandom, mBttVolume;
	Ogre::String mCurrentAction;
	bool mRepeatMedia, mSilence, mRandomMedia;
};

#endif // #ifndef _MEDIAPLAYERMINI_H__

