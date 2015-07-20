#ifndef _SLIDER_H__
#define _SLIDER_H__

#include "Widget.h"

class Slider : public Widget
{
public:
	Slider(const Ogre::String& name, Ogre::Real width, Ogre::Real minValue, Ogre::Real maxValue, unsigned int snaps);
	~Slider();

	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _cursorReleased(const Ogre::Vector2& cursorPos);
	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void _focusLost() {mDragging = false;}

	void setRange(Ogre::Real minValue, Ogre::Real maxValue, unsigned int snaps, bool notifyListener = true);
	void setValue(Ogre::Real value, bool notifyListener = true);
	Ogre::Real getValue() { return mValue; }

private:
	Ogre::Real _getSnappedValue(Ogre::Real percentage);

	Ogre::BorderPanelOverlayElement* mTrack, *mTrackBack;
	Ogre::PanelOverlayElement* mHandle;
	bool mDragging;
	Ogre::Real mDragOffset;
	Ogre::Real mValue;
	Ogre::Real mMinValue;
	Ogre::Real mMaxValue;
	Ogre::Real mInterval;
};

#endif // #define _SLIDER_H__

