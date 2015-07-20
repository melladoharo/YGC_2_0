#ifndef _CHECKBOX_H__
#define _CHECKBOX_H__

#include "Widget.h"

class CheckBox : public Widget
{
public:
	CheckBox(const Ogre::String& name, const Ogre::DisplayString& caption, Ogre::Real left, Ogre::Real top, Ogre::Real width);
	~CheckBox();

	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _cursorReleased(const Ogre::Vector2& cursorPos);
	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void _focusLost() { mChecked = false; mSquare->setMaterialName("YgcGui/CheckBox/Disable"); }

	void setChecked(bool checked, bool notifyListener = true);
	bool isChecked() { return mChecked; }
	void toggle(bool notifyListener = true){ setChecked(!isChecked(), notifyListener); }

private:
	Ogre::BorderPanelOverlayElement* mBP;
	Ogre::OverlayElement* mSquare;
	Ogre::TextAreaOverlayElement* mTextArea;
	bool mChecked;
};

#endif // #ifndef _CHECKBOX_H__

