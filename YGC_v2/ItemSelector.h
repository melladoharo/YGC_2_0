#ifndef _ITEMSELECTOR_H__
#define _ITEMSELECTOR_H__

#include "Widget.h"

class ItemSelector : public Widget
{
public:
	ItemSelector(const Ogre::String& name, const Ogre::DisplayString& caption, const Ogre::StringVector& items, Ogre::Real left, Ogre::Real top, Ogre::Real width);
	~ItemSelector();

	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _cursorReleased(const Ogre::Vector2& cursorPos);
	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void _focusLost() {}

	void setNullOption(const Ogre::String& nullOption);
	void setItems(const Ogre::StringVector& newItems);
	void addItem(const Ogre::String& newOption);
	void removeItem(const Ogre::String& oldOption);
	void selectOption(const Ogre::String& option, bool notifyListener = true);
	Ogre::String getSelectedOption();
	int getSelectedIndex();

private:
	void _spacingElements();

	Ogre::BorderPanelOverlayElement* mBP;
	Ogre::OverlayElement* mPreviousItem, *mNextItem;
	Ogre::TextAreaOverlayElement* mTextArea, *mTextAreaOption;
	Ogre::StringVector mItems;
	unsigned int mCurrentItem;
	bool mItemChanged;
};

#endif // #ifndef _ITEMSELECTOR_H__

