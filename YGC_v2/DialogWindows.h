#ifndef _DIALOGWINDOW_H__
#define _DIALOGWINDOW_H__

#include "Widget.h"

class DialogWindows : public Widget
{
public:
	DialogWindows(const Ogre::String& name, const Ogre::DisplayString& caption, Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real height);
	~DialogWindows();


	void setHeaderCaption(const Ogre::DisplayString& caption);

private:
	Ogre::BorderPanelOverlayElement* mHeader, *mBody;
	Ogre::TextAreaOverlayElement* mTextArea;
};

#endif // #ifndef _DIALOGWINDOW_H__

