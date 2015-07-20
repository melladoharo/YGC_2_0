#include "Stdafx.h"
#include "DecorWidget.h"


DecorWidget::DecorWidget(const Ogre::String& name, const Ogre::String& templateName)
{
	mElement = Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate(templateName, "", name);
}
