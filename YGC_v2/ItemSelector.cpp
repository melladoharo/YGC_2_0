#include "stdafx.h"
#include "ItemSelector.h"

ItemSelector::ItemSelector(const Ogre::String& name, const Ogre::DisplayString& caption, const Ogre::StringVector& items, Ogre::Real left, Ogre::Real top, Ogre::Real width) :
mItems(items),
mCurrentItem(0),
mItemChanged(false)
{
	mElement = Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("YgcGui/ItemSelector", "Panel", name);
	mBP = (Ogre::BorderPanelOverlayElement*)mElement;
	mTextArea = (Ogre::TextAreaOverlayElement*)mBP->getChild(mBP->getName() + "/ItemSelectorCaption");
	mPreviousItem = (Ogre::TextAreaOverlayElement*)mBP->getChild(mBP->getName() + "/ItemSelectorPrevious");
	mTextAreaOption = (Ogre::TextAreaOverlayElement*)mBP->getChild(mBP->getName() + "/ItemSelectorOption");
	mNextItem = (Ogre::TextAreaOverlayElement*)mBP->getChild(mBP->getName() + "/ItemSelectorNext");
	mElement->setLeft(left);
	mElement->setTop(top);
	mElement->setWidth(width);
	mTextArea->setCaption(caption);
	mTextArea->setLeft(mTextArea->getLeft() + 2);

	// spacing elements 
	if (!mItems.empty())
	{
		mTextAreaOption->setCaption(mItems[mCurrentItem]);
		_spacingElements();
	}
}

ItemSelector::~ItemSelector()
{
}



void ItemSelector::_cursorMoved(const Ogre::Vector2& cursorPos)
{
	if (isCursorOver(mElement, cursorPos)) mElement->setParameter("transparent", "false");
	else mElement->setParameter("transparent", "true");
}

void ItemSelector::_cursorPressed(const Ogre::Vector2& cursorPos)
{
	if (isCursorOver(mPreviousItem, cursorPos))
	{
		unsigned int lastItem = mCurrentItem;
		mCurrentItem = (mCurrentItem == 0) ? mItems.size() - 1 : mCurrentItem - 1;
		mItemChanged = (lastItem != mCurrentItem) ? true : false;

	}
	else if (isCursorOver(mNextItem, cursorPos))
	{
		unsigned int lastItem = mCurrentItem;
		mCurrentItem = (mCurrentItem == mItems.size() - 1) ? 0 : mCurrentItem + 1;
		mItemChanged = (lastItem != mCurrentItem) ? true : false;
	}
}

void ItemSelector::_cursorReleased(const Ogre::Vector2& cursorPos)
{
	if (!mItems.empty() && mItemChanged)
	{
		mTextAreaOption->setCaption(mItems[mCurrentItem]);
		_spacingElements();
		if (mListener) mListener->itemChanged(this);
	}
	mItemChanged = false;
}




void ItemSelector::setNullOption(const Ogre::String& nullOption)
{
	mTextAreaOption->setCaption(nullOption);
	_spacingElements();
}

void ItemSelector::setItems(const Ogre::StringVector& newItems)
{
	mItems = newItems;
	mCurrentItem = 0;
	if (!mItems.empty())
	{
		mTextAreaOption->setCaption(mItems[mCurrentItem]);
		_spacingElements();
	}
	else
	{
		mTextAreaOption->setCaption("  ");
		_spacingElements();
	}
}

void ItemSelector::addItem(const Ogre::String& newOption)
{
	mItems.push_back(newOption);
}

void ItemSelector::removeItem(const Ogre::String& oldOption)
{
	for (unsigned int i = 0; i < mItems.size(); ++i)
		if (mItems[i] == oldOption)
			mItems.erase(mItems.begin() + i);
}

void ItemSelector::selectOption(const Ogre::String& option, bool notifyListener /*= true*/)
{
	for (unsigned int i = 0; i < mItems.size(); ++i)
	{
		if (mItems[i] == option)
		{
			mCurrentItem = i;
			mTextAreaOption->setCaption(mItems[mCurrentItem]);
			_spacingElements();
			if (mListener && notifyListener) mListener->itemChanged(this);
		}
	}
}

Ogre::String ItemSelector::getSelectedOption()
{
	if (mCurrentItem >= 0 && mCurrentItem < mItems.size())
		return mItems[mCurrentItem];

	return Ogre::StringUtil::BLANK;
}



void ItemSelector::_spacingElements()
{	
	Ogre::Real sizeCaption = sizeInPixels(mTextArea->getCaption(), mTextArea->getFontName(), mTextArea->getCharHeight(), mTextArea->getSpaceWidth());
	Ogre::Real sizeOption = sizeInPixels(mTextAreaOption->getCaption(), mTextAreaOption->getFontName(), mTextAreaOption->getCharHeight(), mTextAreaOption->getSpaceWidth());
	Ogre::Real sizePanel = 19;
	mNextItem->setWidth((sizeOption / 2) + sizePanel);
	mPreviousItem->setWidth(mElement->getWidth() - sizeCaption - sizeOption - sizePanel);

	mNextItem->setLeft(-mNextItem->getWidth() - 12);
	mTextAreaOption->setLeft(-sizeOption - sizePanel - 7 - 12);
	mPreviousItem->setLeft(mTextAreaOption->getLeft() - mPreviousItem->getWidth() - 9);
}

