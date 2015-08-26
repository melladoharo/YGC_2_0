#ifndef _MEDIAPLAYERMINI_H__
#define _MEDIAPLAYERMINI_H__

#include "Widget.h"
#include "SliderOptions.h"

enum eMiniAction {MINI_PLAY, MINI_PAUSE, MINI_STOP, MINI_NEXT, MINI_PREV, MINI_REPEAT, MINI_RANDOM, MINI_VOLUME, MINI_TRACKLIST};

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

	void setCaptionTrack(const Ogre::String& caption) { fitCaptionToArea(caption, mCaptionMedia, mElement->getWidth()); }
	unsigned int getSelectedAction() { return mSelectedAction; }
	Ogre::Real getSliderValue() { return mSlider->getValue(); }
	void setSliderValue(Ogre::Real newVal, Ogre::Real maxValue, bool notifyListener = true);
	void setSliderRange(Ogre::Real minValue, Ogre::Real maxValue, unsigned int snaps);
	void assignSliderListener(GuiListener* listener) { mSlider->_assignListener(listener); }

	bool isRepeatMedia() { return mRepeatMedia; }
	bool isSilence() { return mSilence; }
	bool isRandom() { return mRandomMedia; }
	void setSilence(bool silence) { mSilence = silence; }
	void setRandom(bool random) { mRandomMedia = random; mBttRandom.panel->setMaterialName(mRandomMedia ? "YgcGui/MiniRandom/Enable/Over" : "YgcGui/MiniRandom/Disable/Over"); }
	void setRepeat(bool repeat) { mRepeatMedia = repeat; mBttRepeat.panel->setMaterialName(mRepeatMedia ? "YgcGui/MiniRepeat/Enable/Over" : "YgcGui/MiniRepeat/Disable/Over"); }

private:
	Ogre::String _secondsToString(int totalSeconds);
	Ogre::String _minutesToString(int totalSeconds);

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

