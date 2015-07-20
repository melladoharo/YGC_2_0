#include "stdafx.h"
#include "Label.h"

Label::Label(const Ogre::String& name, const Ogre::DisplayString& caption, Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real charHeight)
{
	mElement = Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("YgcGui/Label", "BorderPanel", name);
	mTextArea = (Ogre::TextAreaOverlayElement*)((Ogre::OverlayContainer*)mElement)->getChild(getName() + "/LabelCaption");
	setCaption(caption);

	mElement->setLeft(left);
	mElement->setTop(top);
	mElement->setWidth(width);
	mTextArea->setLeft(12);
	mTextArea->setCharHeight(charHeight);
}

Label::~Label()
{
}



void Label::_cursorMoved(const Ogre::Vector2& cursorPos)
{
	if (isCursorOver(mElement, cursorPos))
	{
		mElement->setParameter("transparent", "false");
		mCursorOver = true;
	}
	else
	{
		mElement->setParameter("transparent", "true");
		mCursorOver = false;
	}
}

void Label::_cursorPressed(const Ogre::Vector2& cursorPos)
{
	if (mListener && mCursorOver)
		mListener->labelHit(this);
}

