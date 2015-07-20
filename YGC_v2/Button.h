#ifndef _BUTTON_H__
#define _BUTTON_H__

#include "Widget.h"

class Button : public Widget
{
public:
	Button(const Ogre::String& name, const Ogre::DisplayString& caption, Ogre::Real left, Ogre::Real top, Ogre::Real width);
	virtual ~Button();

	const Ogre::DisplayString& getCaption()
	{
		return mTextArea->getCaption();
	}

	void setCaption(const Ogre::DisplayString& caption)
	{
		mTextArea->setCaption(caption);
		if (mFitToContents) mElement->setWidth(getCaptionWidth(caption, mTextArea) + mElement->getHeight() - 12);
	}

	const ButtonState& getState() { return mState; }

	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _cursorReleased(const Ogre::Vector2& cursorPos);
	void _cursorMoved(const Ogre::Vector2& cursorPos);

	// fix widget does not change its state when hides
	void show() { Widget::show(); _focusLost(); }

	void _focusLost()
	{
		setState(BS_UP);   // reset button if cursor was lost
	}

protected:
	void setState(const ButtonState& bs);

	ButtonState mState;
	Ogre::BorderPanelOverlayElement* mBP;
	Ogre::TextAreaOverlayElement* mTextArea;
	bool mFitToContents;
};

#endif // #define _BUTTON_H__

