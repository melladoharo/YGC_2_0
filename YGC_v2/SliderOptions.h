#ifndef _SLIDEROPTIONS_H__
#define _SLIDEROPTIONS_H__

#include "Widget.h"

class SliderOptions : public Widget
{
public:
	SliderOptions(const Ogre::String& name, const Ogre::String& caption, Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real minValue, Ogre::Real maxValue, unsigned int snaps);
	~SliderOptions();

	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _cursorReleased(const Ogre::Vector2& cursorPos);
	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void _focusLost() { mDragging = false; }

	void setRange(Ogre::Real minValue, Ogre::Real maxValue, unsigned int snaps, bool notifyListener = true);
	void setValue(Ogre::Real value, bool notifyListener = true);
	Ogre::Real getValue() { return mValue; }

private:
	Ogre::Real _getSnappedValue(Ogre::Real percentage);

	Ogre::BorderPanelOverlayElement* mTrackFront, *mTrackBack;
	Ogre::PanelOverlayElement* mHandle;
	Ogre::TextAreaOverlayElement* mTextArea;
	bool mDragging;
	Ogre::Real mDragOffset;
	Ogre::Real mValue;
	Ogre::Real mMinValue;
	Ogre::Real mMaxValue;
	Ogre::Real mInterval;
};

#endif // #ifndef _SLIDEROPTIONS_H__

