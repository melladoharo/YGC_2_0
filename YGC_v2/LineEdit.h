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

	// Escape codes are invalid in LineEdit widget, so replace them with a ' ' character.
	static void removeEscapeCodes(Ogre::String& str);
	static void removeSpecialCodes(Ogre::String& str);

	// simple ini can�t save '\n' character, so replace character '\n' to string "\n".
	static void replaceNewLineEscapeToINI(Ogre::String& str);
	static void replaceNewLineEscapeFromINI(Ogre::String& str);

private:

	Ogre::OverlayElement* mMessage, *mCursor;
	Ogre::TextAreaOverlayElement* mTextAreaCaption, *mTextAreaMessage;
	EditString* mEditString;
	unsigned int mLeftIndexMessage, mRightIndexMessage,
		mLeftIndexCursor, mRightIndexCursor, mMaxCharacters;
};

#endif // #ifndef _LINEEDIT_H__

