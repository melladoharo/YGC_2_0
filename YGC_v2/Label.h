#ifndef _LABEL_H__
#define _LABEL_H__

#include "Widget.h"

class Label : public Widget
{
public:
	Label(const Ogre::String& name, const Ogre::DisplayString& caption, Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real charHeight);
	~Label();

	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void _cursorPressed(const Ogre::Vector2& cursorPos);

	const Ogre::DisplayString& getCaption()
	{
		return mTextArea->getCaption();
	}

	void setCaption(const Ogre::DisplayString& caption)
	{
		mTextArea->setCaption(caption);
	}

	void setCharHeight(Ogre::Real charHeight){ mTextArea->setCharHeight(charHeight); }

private:
	Ogre::TextAreaOverlayElement* mTextArea;
	bool mCursorOver;
};

#endif // #ifndef _LABEL_H__

