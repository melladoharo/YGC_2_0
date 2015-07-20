#ifndef _SELECTMENU_H__
#define _SELECTMENU_H__

#include "Widget.h"

class SelectMenu : public Widget
{
public:
	SelectMenu(const Ogre::String& name, const Ogre::StringVector& items, Ogre::Real width);
	~SelectMenu();

	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _cursorReleased(const Ogre::Vector2& cursorPos);
	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void _focusLost();

	bool isExpanded() { return mExpanded; }
	unsigned int getNumItems() { return mItems.size(); }
	void setItems(const Ogre::StringVector& items);
	void selectItem(unsigned int index, bool notifyListener = true);
	Ogre::DisplayString getSelectedItem();
	int getSelectionIndex() { return mSelectionIndex; }
	void retract();
	
private:
	void setDisplayIndex(unsigned int index);

	Ogre::TextAreaOverlayElement* mTextArea;
	Ogre::BorderPanelOverlayElement* mExpandedBox;
	std::vector<Ogre::BorderPanelOverlayElement*> mItemElements;
	Ogre::StringVector mItems;
	bool mCursorOver;
	bool mExpanded;
	bool mFitToContents;
	bool mDragging;
	int mSelectionIndex;
	int mHighlightIndex;
	int mDisplayIndex;
	unsigned int mMaxItemsShown;
	unsigned int mItemsShown;
};

#endif // #ifndef _SELECTMENU_H__
