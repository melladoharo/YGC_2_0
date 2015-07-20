#include "stdafx.h"
#include "DialogWindows.h"


DialogWindows::DialogWindows(const Ogre::String& name, const Ogre::DisplayString& caption, Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real height)
{
	mElement = Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("YgcGui/DialogWindow", "BorderPanel", name);
	mBody = (Ogre::BorderPanelOverlayElement*)mElement;
	mHeader = (Ogre::BorderPanelOverlayElement*)mBody->getChild(mBody->getName() + "/DialogWindowHeader");
	mTextArea = (Ogre::TextAreaOverlayElement*)mHeader->getChild(mHeader->getName() + "/HeaderCaption");
	mTextArea->setCaption(caption);
	
	mBody->setTop(top);
	mBody->setLeft(left);
	mBody->setWidth(width);
	mBody->setHeight(height);

	Ogre::Real sizeCaption = sizeInPixels(mTextArea->getCaption(), mTextArea->getFontName(), mTextArea->getCharHeight(), mTextArea->getSpaceWidth());
	mHeader->setWidth(sizeCaption + 50);
}

DialogWindows::~DialogWindows()
{
}



void DialogWindows::setHeaderCaption(const Ogre::DisplayString& caption)
{
	mTextArea->setCaption(caption);
	Ogre::Real sizeCaption = sizeInPixels(mTextArea->getCaption(), mTextArea->getFontName(), mTextArea->getCharHeight(), mTextArea->getSpaceWidth());
	mHeader->setWidth(sizeCaption + 80);
}
