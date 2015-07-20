#include "stdafx.h"
#include "SelectMenu.h"

SelectMenu::SelectMenu(const Ogre::String& name, const Ogre::StringVector& items, Ogre::Real width) :
mCursorOver(false),
mExpanded(false),
mDragging(false),
mSelectionIndex(-1),
mHighlightIndex(0),
mMaxItemsShown(5)
{
	mElement = Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("YgcGui/SelectMenu", "BorderPanel", name);
	mTextArea = (Ogre::TextAreaOverlayElement*)((Ogre::OverlayContainer*)mElement)->getChild(name + "/MenuCaption");
	mExpandedBox = (Ogre::BorderPanelOverlayElement*)((Ogre::OverlayContainer*)mElement)->getChild(name + "/MenuExpandedBox");

	mElement->setWidth(width);
	//mTextArea->setCaption(caption);
	mExpandedBox->setWidth(width);
	mExpandedBox->hide();
	mMaxItemsShown = items.size();
	setItems(items);
}

SelectMenu::~SelectMenu()
{
}



void SelectMenu::setItems(const Ogre::StringVector& items)
{
	mItems = items;
	mSelectionIndex = -1;

	for (unsigned int i = 0; i < mItemElements.size(); i++)   // destroy all the item elements
	{
		nukeOverlayElement(mItemElements[i]);
	}
	mItemElements.clear();

	mItemsShown = std::max<int>(2, std::min<int>(mMaxItemsShown, mItems.size()));

	for (unsigned int i = 0; i < mItemsShown; i++)   // create all the item elements
	{
		Ogre::BorderPanelOverlayElement* e =
			(Ogre::BorderPanelOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate
			("SdkTrays/SelectMenuItem", "BorderPanel",
			mExpandedBox->getName() + "/Item" + Ogre::StringConverter::toString(i + 1));

		e->setTop(1 + i * (40 - 8));
		e->setHeight(40);
		e->setWidth(mExpandedBox->getWidth());
		e->setLeft(mExpandedBox->getLeft());

		mExpandedBox->addChild(e);
		mItemElements.push_back(e);
	}

	if (!items.empty()) selectItem(0, false);
}

void SelectMenu::selectItem(unsigned int index, bool notifyListener /*= true*/)
{
	if (index >= mItems.size())
	{
		Ogre::String desc = "Menu \"" + getName() + "\" contains no item at position " +
			Ogre::StringConverter::toString(index) + ".";
		OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, desc, "SelectMenu::selectItem");
	}

	mSelectionIndex = index;
	fitCaptionToArea(mItems[index], mTextArea, mElement->getWidth() -30);

	if (mListener && notifyListener) mListener->itemSelected(this);
}


void SelectMenu::_cursorPressed(const Ogre::Vector2& cursorPos)
{
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();

	if (mExpanded)
	{
		if (!isCursorOver(mExpandedBox, cursorPos, 3)) retract();
		else
		{
			Ogre::Real l = mItemElements.front()->_getDerivedLeft() * om.getViewportWidth() + 5;
			Ogre::Real t = mItemElements.front()->_getDerivedTop() * om.getViewportHeight() + 5;
			Ogre::Real r = l + mItemElements.back()->getWidth() - 10;
			Ogre::Real b = mItemElements.back()->_getDerivedTop() * om.getViewportHeight() +
				mItemElements.back()->getHeight() - 5;

			if (cursorPos.x >= l && cursorPos.x <= r && cursorPos.y >= t && cursorPos.y <= b)
			{
				if (mHighlightIndex != mSelectionIndex) 
					selectItem(mHighlightIndex);
				retract();
			}
		}
	}
	else
	{
		if (mItems.size() < 2) return;   // don't waste time showing a menu if there's no choice

		if (isCursorOver(mElement, cursorPos, 4))
		{
			mExpandedBox->show();			

			// calculate how much vertical space we need
			Ogre::Real idealHeight = (mItemElements.back()->getTop() - mItemElements.front()->getTop()) + (40 + 3);
			mExpandedBox->setHeight(idealHeight);
			// GuiManager - prioritylayer adds and removes this child (mExpandedBox), so its necessary 
			// set the top and left each time is click shown.
			mExpandedBox->setTop(40);
			mExpandedBox->setLeft(0);

			mExpanded = true;
			mHighlightIndex = mSelectionIndex;
			setDisplayIndex(mHighlightIndex);
		}
	}
}


void SelectMenu::_cursorReleased(const Ogre::Vector2& cursorPos)
{

}

void SelectMenu::_cursorMoved(const Ogre::Vector2& cursorPos)
{
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();

	if (mExpanded)
	{
		Ogre::Real l = mItemElements.front()->_getDerivedLeft() * om.getViewportWidth() + 5;
		Ogre::Real t = mItemElements.front()->_getDerivedTop() * om.getViewportHeight() + 5;
		Ogre::Real r = l + mItemElements.back()->getWidth() - 10;
		Ogre::Real b = mItemElements.back()->_getDerivedTop() * om.getViewportHeight() +
			mItemElements.back()->getHeight() - 5;

		if (cursorPos.x >= l && cursorPos.x <= r && cursorPos.y >= t && cursorPos.y <= b)
		{
			int newIndex = (int)(mDisplayIndex + (cursorPos.y - t) / (b - t) * mItemElements.size());
			if (mHighlightIndex != newIndex)
			{
				mHighlightIndex = newIndex;
				setDisplayIndex(mDisplayIndex);
			}
		}
	}
	else
	{
		if (isCursorOver(mElement, cursorPos, 4))
		{
			//mSmallBox->setMaterialName("SdkTrays/MiniTextBox/Over");
			//mSmallBox->setBorderMaterialName("SdkTrays/MiniTextBox/Over");
			mCursorOver = true;
		}
		else
		{
			if (mCursorOver)
			{
				//mSmallBox->setMaterialName("SdkTrays/MiniTextBox");
				//mSmallBox->setBorderMaterialName("SdkTrays/MiniTextBox");
				mCursorOver = false;
			}
		}
	}
}

void SelectMenu::_focusLost()
{
	if (mExpandedBox->isVisible()) retract();
}

void SelectMenu::retract()
{
	mDragging = false;
	mExpanded = false;
	mExpandedBox->hide();
	mTextArea->show();
}

void SelectMenu::setDisplayIndex(unsigned int index)
{
	index = std::min<int>(index, mItems.size() - mItemElements.size());
	mDisplayIndex = index;
	Ogre::BorderPanelOverlayElement* ie;
	Ogre::TextAreaOverlayElement* ta;

	for (int i = 0; i < (int)mItemElements.size(); i++)
	{
		ie = mItemElements[i];
		ta = (Ogre::TextAreaOverlayElement*)ie->getChild(ie->getName() + "/MenuItemText");

		fitCaptionToArea(mItems[mDisplayIndex + i], ta, ie->getWidth() - 2 * ta->getLeft());

		if ((mDisplayIndex + i) == mHighlightIndex)
		{
			ie->setMaterialName("YgcGui/SelectMenu/Over");
			ie->setBorderMaterialName("YgcGui/SelectMenu/Over");
			ta->setColour(Ogre::ColourValue(0.96f, 0.95f, 0.97f));
		}
		else
		{
			ie->setMaterialName("YgcGui/SelectMenuExpanded");
			ie->setBorderMaterialName("YgcGui/SelectMenuExpanded");
			ta->setColour(Ogre::ColourValue(0.59f, 0.64f, 0.64f));
		}
	}
}

Ogre::DisplayString SelectMenu::getSelectedItem()
{
	if (mSelectionIndex == -1)
	{
		Ogre::String desc = "Menu \"" + getName() + "\" has no item selected.";
		OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, desc, "SelectMenu::getSelectedItem");
		return "";
	}

	else return mItems[mSelectionIndex];
}


