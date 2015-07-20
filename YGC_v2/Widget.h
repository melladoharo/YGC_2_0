#ifndef _WIDGET_H__
#define _WIDGET_H__

#include "GuiListener.h"

class Widget
{
public:
	Widget();
	virtual ~Widget();

	void cleanup()
	{
		if (mElement) nukeOverlayElement(mElement);
		mElement = 0;
	}

	/*-----------------------------------------------------------------------------
	| Static utility method to recursively delete an overlay element plus
	| all of its children from the system.
	-----------------------------------------------------------------------------*/
	static void nukeOverlayElement(Ogre::OverlayElement* element);

	/*-----------------------------------------------------------------------------
	| Static utility method to check if the cursor is over an overlay element.
	-----------------------------------------------------------------------------*/
	static bool isCursorOver(Ogre::OverlayElement* element, const Ogre::Vector2& cursorPos, Ogre::Real voidBorder = 0);
	
	/*-----------------------------------------------------------------------------
	| Static utility method used to get the cursor's offset from the center
	| of an overlay element in pixels.
	-----------------------------------------------------------------------------*/
	static Ogre::Vector2 cursorOffset(Ogre::OverlayElement* element, const Ogre::Vector2& cursorPos);

	/*-----------------------------------------------------------------------------
	| Static utility method used to get the width of a caption in a text area.
	-----------------------------------------------------------------------------*/
	static Ogre::Real getCaptionWidth(const Ogre::DisplayString& caption, Ogre::TextAreaOverlayElement* area);
	
	/*-----------------------------------------------------------------------------
	| Static utility method to cut off a string to fit in a text area.
	-----------------------------------------------------------------------------*/
	static void fitCaptionToArea(const Ogre::DisplayString& caption, Ogre::TextAreaOverlayElement* area, Ogre::Real maxWidth);

	/*-----------------------------------------------------------------------------
	| Static utility method to calculate the size in pixels of a string
	-----------------------------------------------------------------------------*/
	static Ogre::Real sizeInPixels(const Ogre::String& text, const Ogre::String nameFont, Ogre::Real heightFont, Ogre::Real spaceWidth);

	Ogre::OverlayElement* getOverlayElement()
	{
		return mElement;
	}

	const Ogre::String& getName()
	{
		return mElement->getName();
	}

	virtual void hide()
	{
		mElement->hide();
	}

	virtual void show()
	{
		mElement->show();
	}

	bool isVisible()
	{
		return mElement->isVisible();
	}

	void setPosition(Ogre::Real left, Ogre::Real top) 
	{
		mElement->setPosition(left, top); 
	}

	Ogre::Real getLeft() {return mElement->getLeft(); }
	Ogre::Real getTop() {return mElement->getTop(); }
	void setLeft(Ogre::Real left) { mElement->setLeft(left); }
	void setTop(Ogre::Real top) { mElement->setTop(top); }
	Ogre::Real getWidth() const { return mElement->getWidth(); }
	Ogre::Real getHeight() const { return mElement->getHeight(); }

	void _autoScale(unsigned int maxSize, Ogre::MaterialPtr matText);

	// callbacks
	virtual void _cursorPressed(const Ogre::Vector2& cursorPos) {}
	virtual void _cursorReleased(const Ogre::Vector2& cursorPos) {}
	virtual void _cursorMoved(const Ogre::Vector2& cursorPos) {}
	virtual void _focusLost() {}

	// internal methods used by SdkTrayManager. do not call directly.
	void _assignListener(GuiListener* listener) { mListener = listener; }

protected:
	Ogre::OverlayElement* mElement;
	GuiListener* mListener;
};

typedef std::vector<Widget*> WidgetList;
typedef Ogre::VectorIterator<WidgetList> WidgetIterator;

#endif // #define _WIDGET_H__
