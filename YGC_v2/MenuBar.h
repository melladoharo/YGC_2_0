#ifndef _MENUBAR_H__
#define _MENUBAR_H__

#include "Widget.h"

// Item option
struct sItem
{
	Ogre::OverlayContainer* panel;
	Ogre::TextAreaOverlayElement* textArea;
	ButtonState currentState;
};

/*=============================================================================
| MenuBar class - show a top bar showing differents options
=============================================================================*/
class MenuBar : public Widget
{
public:
	MenuBar();
	~MenuBar();

	// mouse managment
	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _cursorReleased(const Ogre::Vector2& cursorPos);
	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void _focusLost(){ setState(BS_UP, mIndexOver); }
	void setState(const ButtonState& bs, int index);
	void _forceCursorReleased();

	// creates the differents menus
	void createMenuMain();
	void createMenuGame();
	void createMenuSaga();

	Ogre::DisplayString getSelectedItem()
	{
		if (mIndexOver >= 0 && mIndexOver < mItems.size())
			return mItems[mIndexOver].panel->getName();
		return Ogre::StringUtil::BLANK;
	}

	int getSelectionIndex()
	{
		return mIndexOver;
	}

	bool isMouseOver() { return mMouseOverBar; }

protected:
	void _createNewOption(const Ogre::String& namePanel, const Ogre::String& textOption, Ogre::Real& sepLeft);
	void _destroyAllItems();

	Ogre::OverlayContainer* mBar; // Main bar
	std::vector<sItem> mItems; // Items options
	Ogre::FontPtr mFont; // Font used for items
	Ogre::ColourValue mColourSelect, mColourDeselect; // Colours when mouse is over an item 
	unsigned int mIndexOver, mIndexSelected; // Current index item over, and current item selected
	bool mMouseOverBar;
};

#endif // #define _MENUBAR_H__

