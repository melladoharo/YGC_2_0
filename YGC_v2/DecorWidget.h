#ifndef _DECORWIDGET_H__
#define _DECORWIDGET_H__

#include "Widget.h"

class DecorWidget : public Widget
{
public:

	// Do not instantiate any widgets directly. Use SdkTrayManager.
	DecorWidget(const Ogre::String& name, const Ogre::String& templateName);
};

#endif // #define _DECORWIDGET_H__

