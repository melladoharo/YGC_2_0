#ifndef _LINEEDIT_H__
#define _LINEEDIT_H__

#include "Widget.h"
#include "EditString.h"

class LineEdit : public Widget
{
public:
	LineEdit(const Ogre::String& name, const Ogre::String& caption, const Ogre::String& message, 
		Ogre::Real left, Ogre::Real top, Ogre::Real width);
	~LineEdit();

	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _focusLost() { mCursor->hide(); }

	void setText(const Ogre::String& text);
	Ogre::String getText() { return ((mEditString) ? mEditString->getText() : Ogre::StringUtil::BLANK); }
	void injectKeyPress(const OIS::KeyEvent &arg);

private:
	Ogre::OverlayElement* mMessage, *mCursor;
	Ogre::TextAreaOverlayElement* mTextAreaCaption, *mTextAreaMessage;
	EditString* mEditString;
	unsigned int mLeftIndexMessage, mRightIndexMessage,
		mLeftIndexCursor, mRightIndexCursor, mMaxCharacters;
};

#endif // #ifndef _LINEEDIT_H__

