#include "stdafx.h"
#include "CheckBox.h"


CheckBox::CheckBox(const Ogre::String& name, const Ogre::DisplayString& caption, Ogre::Real left, Ogre::Real top, Ogre::Real width) : 
mChecked(false)
{
	mElement = Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("YgcGui/CheckBox", "BorderPanel", name);
	mBP = (Ogre::BorderPanelOverlayElement*)mElement;
	mTextArea = (Ogre::TextAreaOverlayElement*)mBP->getChild(mBP->getName() + "/CheckBoxCaption");
	mSquare = (Ogre::TextAreaOverlayElement*)mBP->getChild(mBP->getName() + "/CheckBoxSquare");
	mElement->setLeft(left);
	mElement->setTop(top);
	mElement->setWidth(width);
	mTextArea->setCaption(caption);
	mTextArea->setLeft(mTextArea->getLeft());
}

CheckBox::~CheckBox()
{
}



void CheckBox::_cursorMoved(const Ogre::Vector2& cursorPos)
{

}

void CheckBox::_cursorPressed(const Ogre::Vector2& cursorPos)
{

}

void CheckBox::_cursorReleased(const Ogre::Vector2& cursorPos)
{
	if (isCursorOver(mSquare, cursorPos) && mListener)
	{
		toggle();
	}
}

void CheckBox::setChecked(bool checked, bool notifyListener /*= true*/)
{
	mChecked = checked;
	if (mChecked) mSquare->setMaterialName("YgcGui/CheckBox/Enable");
	else mSquare->setMaterialName("YgcGui/CheckBox/Disable");
	if (mListener && notifyListener) mListener->checkBoxToggled(this);
}

