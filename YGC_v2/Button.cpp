#include "stdafx.h"
#include "Button.h"

Button::Button(const Ogre::String& name, const Ogre::DisplayString& caption, Ogre::Real left, Ogre::Real top, Ogre::Real width)
{
	mElement = Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("SdkTrays/Button", "BorderPanel", name);
	mBP = (Ogre::BorderPanelOverlayElement*)mElement;
	mTextArea = (Ogre::TextAreaOverlayElement*)mBP->getChild(mBP->getName() + "/ButtonCaption");
	mTextArea->setTop(-(mTextArea->getCharHeight() / 2) + 2);
	mElement->setLeft(left);
	mElement->setTop(top);

	if (width > 0)
	{
		mElement->setWidth(width);
		mFitToContents = false;
	}
	else mFitToContents = true;

	setCaption(caption);
	mState = BS_UP;
}


Button::~Button()
{
}

void Button::_cursorPressed(const Ogre::Vector2& cursorPos)
{
	if (isCursorOver(mElement, cursorPos, 4)) setState(BS_DOWN);
}

void Button::_cursorReleased(const Ogre::Vector2& cursorPos)
{
	if (mState == BS_DOWN)
	{
		setState(BS_OVER);
		if (mListener) mListener->buttonHit(this);
	}
}

void Button::_cursorMoved(const Ogre::Vector2& cursorPos)
{
	if (isCursorOver(mElement, cursorPos, 4))
	{
		if (mState == BS_UP) setState(BS_OVER);
	}
	else
	{
		if (mState != BS_UP) setState(BS_UP);
	}
}

void Button::setState(const ButtonState& bs)
{
	if (bs == BS_OVER)
	{
		mBP->setBorderMaterialName("SdkTrays/Button/Over");
		mBP->setMaterialName("SdkTrays/Button/Over");
	}
	else if (bs == BS_UP)
	{
		mBP->setBorderMaterialName("SdkTrays/Button/Up");
		mBP->setMaterialName("SdkTrays/Button/Up");
	}
	else
	{
		mBP->setBorderMaterialName("SdkTrays/Button/Down");
		mBP->setMaterialName("SdkTrays/Button/Down");
	}

	mState = bs;
}
