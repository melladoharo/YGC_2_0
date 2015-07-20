#include "stdafx.h"
#include "TrackList.h"

TrackList::TrackList(const Ogre::String& name, const Ogre::StringVector& tracks, Ogre::Real left, Ogre::Real top) :
mFont(Ogre::FontManager::getSingletonPtr()->getByName("YgcFont/SemiBold/21")),
mColourSelect(0.96f, 0.95f, 0.97f),
mColourDeselect(0.76f, 0.75f, 0.77f),
mIndexOver(0),
mIndexSelected(0),
mMaxRows(6),
mCurrentPage(0)
{
	// create main element
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	mElement = om.createOverlayElement("Panel", name);
	mElement->setMetricsMode(Ogre::GMM_PIXELS);

	Ogre::Real screenWidth = Ogre::Root::getSingleton().getAutoCreatedWindow()->getViewport(0)->getActualWidth();
	Ogre::Real screenHeight = Ogre::Root::getSingleton().getAutoCreatedWindow()->getViewport(0)->getActualHeight();
	Ogre::Real currentTop = top, currentLeft = left;
	Ogre::Real sepTop = screenHeight / 90, sepLeft = ((screenWidth - currentLeft) / 2) -10;
	unsigned int currentRow = 0, currentCol = 0;
	for (unsigned int i = 0; i < tracks.size(); ++i)
	{
		_createNewTrack(name + "/" + tracks[i], tracks[i], currentLeft, currentTop, sepLeft);
		currentTop += mItems.back().panel->getHeight() + sepTop;
		currentRow++;
		if (currentRow >= mMaxRows)
		{
			currentRow = 0;
			currentTop = top;
			currentLeft += sepLeft;
			currentCol++;
		}
		if (currentCol >= 2)
		{
			currentCol = 0;
			currentLeft = left;
		}
	}
	mIndexSelected = 0;
	mIndexOver = 0;
	_viewPage(mCurrentPage);
	
	// create the previous and next mini button
	Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;
	mBttPrevious.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", name + "/miniButton/Previous");
	mBttPrevious.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttPrevious.panel->setWidth(32);
	mBttPrevious.panel->setHeight(32);
	mBttPrevious.panel->setTop(top + ( (40 + sepTop) * mMaxRows) + 5);
	mBttPrevious.panel->setLeft(left + sepLeft - 32 - 10);
	mBttPrevious.panel->hide();
	mBttPrevious.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPrevious/Up");
	mBttPrevious.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPrevious/Over");
	mBttPrevious.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniPrevious/Down");
	mBttPrevious.panel->setMaterialName(mBttPrevious.matUp->getName());
	mBttPrevious.currentState = BS_UP;
	mBttPrevious.action = "Previous";
	c->addChild(mBttPrevious.panel);

	mBttNext.panel = (Ogre::PanelOverlayElement *)om.createOverlayElement("Panel", name + "/miniButton/Next");
	mBttNext.panel->setMetricsMode(Ogre::GMM_PIXELS);
	mBttNext.panel->setWidth(32);
	mBttNext.panel->setHeight(32);
	mBttNext.panel->setTop(mBttPrevious.panel->getTop());
	mBttNext.panel->setLeft(mBttPrevious.panel->getLeft() + mBttPrevious.panel->getWidth() + 7);
	mBttNext.panel->hide();
	mBttNext.matUp = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniNext/Up");
	mBttNext.matOver = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniNext/Over");
	mBttNext.matDown = Ogre::MaterialManager::getSingleton().getByName("YgcGui/MiniNext/Down");
	mBttNext.panel->setMaterialName(mBttNext.matUp->getName());
	mBttNext.currentState = BS_UP;
	mBttNext.action = "Next";
	c->addChild(mBttNext.panel);

	if (mItems.size() > (mMaxRows * 2))
	{
		mBttPrevious.panel->show();
		mBttNext.panel->show();
	}
}

TrackList::~TrackList()
{
}



void TrackList::setStateTrack(const ButtonState& bs, int index)
{
	if (bs == BS_OVER)
	{
		mItems[index].textArea->setColour(mColourSelect);
	}
	else if (bs == BS_UP)
	{
		mItems[index].textArea->setColour(mColourDeselect);
	}
	else if (bs == BS_DOWN)
	{
		mItems[mIndexSelected].textArea->setColour(mColourDeselect);
		mItems[mIndexSelected].currentState = BS_UP;
		mItems[index].textArea->setColour(mColourSelect);
		mIndexSelected = index;
	}
	else // if (bs==BS_SELECTED)
	{
		mItems[index].textArea->setColour(mColourSelect);
	}

	mItems[index].currentState = bs;
}

void TrackList::setStateButton(const ButtonState& bs, sMiniButton& button)
{
	if (bs == BS_OVER)		
		button.panel->setMaterialName(button.matOver->getName());
	else if (bs == BS_UP)	
		button.panel->setMaterialName(button.matUp->getName());
	else					
		button.panel->setMaterialName(button.matDown->getName());

	button.currentState = bs;
}

void TrackList::_cursorMoved(const Ogre::Vector2& cursorPos)
{
	for (unsigned int i = 0; i < mItems.size(); ++i) // look for the item cursor is over it
	{
		if (mItems[i].currentState != BS_SELECTED && mItems[i].panel->isVisible())
		{
			if (isCursorOver(mItems[i].panel, cursorPos))
			{
				if (mItems[i].currentState == BS_UP)
				{
					mIndexOver = i;
					setStateTrack(BS_OVER, i);
				}
			}
			else
			{
				if (mItems[i].currentState != BS_UP) setStateTrack(BS_UP, i);
			}
		}
	}

	if (isCursorOver(mBttPrevious.panel, cursorPos))
	{
		if (mBttPrevious.currentState == BS_UP) 
			setStateButton(BS_OVER, mBttPrevious);
	}
	else
	{
		if (mBttPrevious.currentState != BS_UP) 
			setStateButton(BS_UP, mBttPrevious);
	}

	if (isCursorOver(mBttNext.panel, cursorPos))
	{
		if (mBttNext.currentState == BS_UP) 
			setStateButton(BS_OVER, mBttNext);
	}
	else
	{
		if (mBttNext.currentState != BS_UP) 
			setStateButton(BS_UP, mBttNext);
	}
}
void TrackList::_cursorPressed(const Ogre::Vector2& cursorPos)
{
	if (isCursorOver(mItems[mIndexOver].panel, cursorPos)) setStateTrack(BS_DOWN, mIndexOver);
	if (isCursorOver(mBttPrevious.panel, cursorPos)) setStateButton(BS_DOWN, mBttPrevious);
	if (isCursorOver(mBttNext.panel, cursorPos)) setStateButton(BS_DOWN, mBttNext);
}

void TrackList::_cursorReleased(const Ogre::Vector2& cursorPos)
{
	if (mItems[mIndexOver].currentState == BS_DOWN)
	{
		setStateTrack(BS_SELECTED, mIndexOver);
		if (mListener)
			mListener->trackListHit(this);
	}
	if (mBttPrevious.currentState == BS_DOWN)
	{
		setStateButton(BS_OVER, mBttPrevious);
		mCurrentPage = (mCurrentPage > 0) ? mCurrentPage - 1 : mCurrentPage;
		_viewPage(mCurrentPage);
	}
	if (mBttNext.currentState == BS_DOWN)
	{
		setStateButton(BS_OVER, mBttNext);
		mCurrentPage = (((mCurrentPage + 1) * mMaxRows * 2) < mItems.size()) ? mCurrentPage + 1 : mCurrentPage;
		_viewPage(mCurrentPage);
	}
}



void TrackList::_createNewTrack(const Ogre::String& namePanel, const Ogre::String& caption, Ogre::Real leftTrack, Ogre::Real topTrack, Ogre::Real widthTrack)
{
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();

	mItems.push_back(sItemTrack());
	mItems.back().panel = (Ogre::PanelOverlayElement*)om.createOverlayElement("Panel", namePanel);
	mItems.back().textArea = static_cast<Ogre::TextAreaOverlayElement*>(om.createOverlayElement("TextArea", "TrackList/Caption" + namePanel));
	mItems.back().currentState = BS_UP;
	mItems.back().panel->addChild(mItems.back().textArea);
	Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;
	c->addChild(mItems.back().panel);
	
	//mItems.back().panel->setMaterialName("YgcGui/MenuBar");
	mItems.back().panel->setMetricsMode(Ogre::GMM_PIXELS);
	mItems.back().panel->setWidth(widthTrack);
	mItems.back().panel->setHeight(40);
	mItems.back().panel->setLeft(leftTrack);
	mItems.back().panel->setTop(topTrack);
	mItems.back().textArea->setMetricsMode(Ogre::GMM_PIXELS);
	mItems.back().textArea->setCaption(caption);
	mItems.back().textArea->setFontName(mFont->getName());
	mItems.back().textArea->setCharHeight(21);
	mItems.back().textArea->setTop(-8);
	mItems.back().textArea->setVerticalAlignment(Ogre::GVA_CENTER);
	mItems.back().textArea->setHorizontalAlignment(Ogre::GHA_LEFT);
	mItems.back().textArea->setAlignment(Ogre::TextAreaOverlayElement::Left);
	mItems.back().textArea->setColour(Ogre::ColourValue(mColourDeselect));
	Widget::fitCaptionToArea(caption, mItems.back().textArea, widthTrack + 10);
}

void TrackList::_destroyAllItems()
{
	for (unsigned int i = 0; i < mItems.size(); ++i)
		Widget::nukeOverlayElement(mItems[i].panel);

	mItems.clear();
}

void TrackList::_viewPage(unsigned int index)
{
	unsigned int minItem = index * mMaxRows * 2;
	unsigned int maxItem = (index+1) * mMaxRows * 2;

	for (unsigned int i = 0; i < mItems.size(); ++i)
	{
		if (i >= minItem && i < maxItem)
		{
			mItems[i].panel->show();
		}
		else
		{
			mItems[i].panel->hide();
		}
	}
}

