#include "stdafx.h"
#include "TrackList.h"

TrackList::TrackList(const Ogre::String& name, const Ogre::StringVector& tracks, Ogre::Real left, Ogre::Real top, unsigned int maxItemShown) : 
mNameWidget(name),
mTracks(tracks),
mDisplayIndex(0),
mMaxItemsShown(maxItemShown),
mItemSelectedShown(-1),
mTrackSelected(-1),
mDragging(false),
mDragOffset(0.0f)
{
	// create background for elements
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	mElement = om.createOverlayElement("Panel", mNameWidget);
	mElement->setMetricsMode(Ogre::GMM_PIXELS);
	mElement->setWidth(400);
	mElement->setHeight(100);
	mElement->setHorizontalAlignment(Ogre::GHA_LEFT);
	mElement->setVerticalAlignment(Ogre::GVA_TOP);
	mElement->setTop(top);
	mElement->setLeft(left);
	mElement->setMaterialName("YgcGui/Thumbnail/Dark");
	Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;

	mTextCaption = (Ogre::TextAreaOverlayElement*)om.createOverlayElement("TextArea", mNameWidget + "/TextArea/Caption");
	mTextCaption->setMetricsMode(Ogre::GMM_PIXELS);
	mTextCaption->setCaption("TRACKS");
	mTextCaption->setFontName("YgcFont/SemiBold/21");
	mTextCaption->setCharHeight(20);
	mTextCaption->setColour(Ogre::ColourValue(0.84f, 0.85f, 0.84f));
	mTextCaption->setTop(12);
	mTextCaption->setLeft(10);
	c->addChild(mTextCaption);

	mSeparator = om.createOverlayElement("Panel", mNameWidget + "/Element/LineSeparator");
	mSeparator->setMetricsMode(Ogre::GMM_PIXELS);
	mSeparator->setWidth(mElement->getWidth() - 18);
	mSeparator->setHeight(2);
	mSeparator->setHorizontalAlignment(Ogre::GHA_LEFT);
	mSeparator->setVerticalAlignment(Ogre::GVA_TOP);
	mSeparator->setTop(12 + 18);
	mSeparator->setLeft(9);
	mSeparator->setMaterialName("YgcGui/SliderOption/Track/Back");
	c->addChild(mSeparator);

	mTrackFront = (Ogre::BorderPanelOverlayElement*)om.createOverlayElement("Panel", mNameWidget + "/Slider");
	mTrackFront->setMetricsMode(Ogre::GMM_PIXELS);
	mTrackFront->setWidth(7);
	mTrackFront->setHorizontalAlignment(Ogre::GHA_LEFT);
	mTrackFront->setVerticalAlignment(Ogre::GVA_TOP);
	mTrackFront->setTop(40 + 5);
	mTrackFront->setLeft(mElement->getWidth() - mTrackFront->getWidth() - 14);
	mTrackFront->setMaterialName("YgcGui/SliderOption/Track/Back");
	c->addChild(mTrackFront);

	mHandle = om.createOverlayElement("Panel", mNameWidget + "/Slider/Handle");
	mHandle->setMetricsMode(Ogre::GMM_PIXELS);
	mHandle->setWidth(mTrackFront->getWidth() + 6);
	mHandle->setHeight(20);
	mHandle->setLeft(-3);
	mHandle->setHorizontalAlignment(Ogre::GHA_LEFT);
	mHandle->setVerticalAlignment(Ogre::GVA_TOP);
	mHandle->setMaterialName("YgcGui/SliderOption/Track/Front");
	mTrackFront->addChild(mHandle);

	mDecorVol = om.createOverlayElement("Panel", mNameWidget + "/Element/DecorVol");
	mDecorVol->setMetricsMode(Ogre::GMM_PIXELS);
	mDecorVol->setWidth(24);
	mDecorVol->setHeight(19);
	mDecorVol->setHorizontalAlignment(Ogre::GHA_LEFT);
	mDecorVol->setVerticalAlignment(Ogre::GVA_TOP);
	mDecorVol->setTop(43);
	mDecorVol->setMaterialName("YgcGui/IconVol/Mat");
	mDecorVol->hide();
	c->addChild(mDecorVol);

	setTracks(mTracks);
}

TrackList::~TrackList()
{
}



void TrackList::_cursorMoved(const Ogre::Vector2& cursorPos)
{
	if (mDragging)
	{
		Ogre::Vector2 co = Widget::cursorOffset(mHandle, cursorPos);
		Ogre::Real newTop = mHandle->getTop() + co.y - mDragOffset;
		Ogre::Real lowerBoundary = mTrackFront->getHeight() - mHandle->getHeight();
		mHandle->setTop(Ogre::Math::Clamp<int>((int)newTop, 0, (int)lowerBoundary));

		Ogre::Real scrollPercentage = Ogre::Math::Clamp<Ogre::Real>(newTop / lowerBoundary, 0, 1);
		int newIndex = (int)(scrollPercentage * (mTracks.size() - mItems.size()) + 0.5);
		if (newIndex != mDisplayIndex) _setDisplayIndex(newIndex);
		
	}

	for (unsigned int i = 0; i < mItemsShown; ++i)
	{
		if (i != mItemSelectedShown)
		{
			if (isCursorOver(mItems[i], cursorPos))
				mItems[i]->setParameter("transparent", "false");
			else
				mItems[i]->setParameter("transparent", "true");
		}
	}
}

void TrackList::_cursorPressed(const Ogre::Vector2& cursorPos)
{
	Ogre::Vector2 co = Widget::cursorOffset(mHandle, cursorPos);

	if (co.squaredLength() <= 81)
	{
		mDragging = true;
		mDragOffset = co.y;
		return;
	}
	else if (Widget::isCursorOver(mTrackFront, cursorPos, -10))
	{
		Ogre::Real newTop = mHandle->getTop() + co.y;
		Ogre::Real lowerBoundary = mTrackFront->getHeight() - mHandle->getHeight();
		mHandle->setTop(Ogre::Math::Clamp<int>((int)newTop, 0, (int)lowerBoundary));

		Ogre::Real scrollPercentage = Ogre::Math::Clamp<Ogre::Real>(newTop / lowerBoundary, 0, 1);
		_setDisplayIndex((unsigned int)(scrollPercentage * (mTracks.size() - mItems.size()) + 0.5));
		return;
	}
}

void TrackList::_cursorReleased(const Ogre::Vector2& cursorPos)
{
	mDragging = false;

	if (mElement->isVisible())
	{
		for (unsigned int i = 0; i < mItemsShown; ++i)
		{
			if (mItems[i]->getParameter("transparent") == "false" && i != mItemSelectedShown)
			{
				// deselect previous item selected
				if (mItemSelectedShown >= 0 && mItemSelectedShown < mItems.size())
					mItems[mItemSelectedShown]->setParameter("transparent", "true");

				mItemSelectedShown = i;
				mTrackSelected = mDisplayIndex + i;
				mDecorVol->show();
				mDecorVol->setTop(mItems[mItemSelectedShown]->getTop() + 5);
				mListener->trackListHit(this);
				return;
			}
		}
	}
}



void TrackList::setMaxItemShown(unsigned int maxItems)
{
	mMaxItemsShown = maxItems;
	setTracks(mTracks);
}



void TrackList::setTracks(const Ogre::StringVector& tracks)
{
	mTracks = tracks;
	mItemSelectedShown = -1;
	mTrackSelected = -1;
	mDecorVol->hide();
	_destroyItems();

	// height widget
	//Ogre::Real screenHeight = Ogre::Root::getSingleton().getAutoCreatedWindow()->getViewport(0)->getActualHeight();
	//unsigned int maxItems = (screenHeight - mElement->getTop() - 40 - 60)/30;
	//mElement->setHeight(std::min<int>((40 + (mTracks.size() * 30) + 5), (40 + (maxItems * 30) + 5)));
	//mMaxItemsShown = (mElement->getHeight() - 40) / 30;

	mElement->setHeight(40 + (mMaxItemsShown * 30) + 5);
	mItemsShown = std::max<int>(0, std::min<int>(mMaxItemsShown, mTracks.size()));

	// show or hide slider
	if (mMaxItemsShown == mTracks.size())
	{
		mTrackFront->hide();
		mHandle->hide();
		mDecorVol->setLeft(mElement->getWidth() - 24 - 12 - 10);
	}
	else // spacing elements
	{
		mTrackFront->show();
		mHandle->show();
		mTrackFront->setTop(mSeparator->getTop() + 10);
		mTrackFront->setHeight(mElement->getHeight() - 40 - 10);
		mHandle->setTop(0);
		mDecorVol->setLeft(mElement->getWidth() - 24 - 12 - 10 - 18);
	}

	// create all the item elements
	unsigned int topItems = mSeparator->getTop() + 10;
	Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;
	for (unsigned int i = 0; i < mItemsShown; i++)   
	{
		Ogre::String nameItem = mNameWidget + "/TextArea/TrackItem_" + Ogre::StringConverter::toString(i);
		mItems.push_back(Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("ItemFileExplorer", "Panel", nameItem));
		mItems.back()->setTop(topItems);
		mItems.back()->setWidth(mElement->getWidth() - (mTrackFront->isVisible() ? (mTrackFront->getWidth() + 12 + 14) : 12));
		Ogre::OverlayContainer* ic = (Ogre::OverlayContainer*)mItems.back();
		Ogre::TextAreaOverlayElement* textArea = (Ogre::TextAreaOverlayElement*)ic->getChild(ic->getName() + "/ItemFileExplorerCaption");
		textArea->setLeft(20);
		Ogre::String trackCaption = boost::filesystem::path(tracks[i]).filename().generic_string();
		fitCaptionToArea(trackCaption, textArea, mItems.back()->getWidth() - 50);
		c->addChild(mItems.back());

		topItems += 30;
	}
}



void TrackList::_setDisplayIndex(unsigned int index)
{
	mDisplayIndex = std::min<int>(index, mTracks.size() - mItems.size());

	Ogre::OverlayContainer* ie;
	Ogre::TextAreaOverlayElement* ta;

	for (int i = 0; i < (int)mItems.size(); i++)
	{
		ie = (Ogre::OverlayContainer*)mItems[i];
		ta = (Ogre::TextAreaOverlayElement*)ie->getChild(ie->getName() + "/ItemFileExplorerCaption");
		Ogre::String trackCaption = boost::filesystem::path(mTracks[mDisplayIndex + i]).filename().generic_string();
		fitCaptionToArea(trackCaption, ta, mItems.back()->getWidth() - 50);

		// deselect previous item selected
		if (mItemSelectedShown >= 0 && mItemSelectedShown < mItems.size())
		{
			mItems[mItemSelectedShown]->setParameter("transparent", "true");
			mDecorVol->hide();
		}

		// new selected index
		mItemSelectedShown = (index - mDisplayIndex <= 0)
			? mTrackSelected - mDisplayIndex 
			: mTrackSelected + mDisplayIndex;
		
		// select the new index if it is possible
		if (mItemSelectedShown >= 0 && mItemSelectedShown < mItems.size())
		{
			mItems[mItemSelectedShown]->setParameter("transparent", "false");
			mDecorVol->show();
			mDecorVol->setTop(mItems[mItemSelectedShown]->getTop() + 5);
		}
	}
}



void TrackList::_destroyItems()
{
	Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;
	for (unsigned int i = 0; i < mItems.size(); ++i)
	{
		c->removeChild(mItems[i]->getName());
		Widget::nukeOverlayElement(mItems[i]);
	}
	mItems.clear();
}



Ogre::String TrackList::getSelectedTrack()
{
	return (mTrackSelected >= 0 && mTrackSelected <= mTracks.size())
		? mTracks[mTrackSelected]
		: Ogre::StringUtil::BLANK;
}

void TrackList::selectNextTrack()
{
	if (mTracks.size() > 1)
	{
		mTrackSelected = (mTrackSelected == mTracks.size() - 1) ? 0 : mTrackSelected + 1;
		_setDisplayIndex(mDisplayIndex);
	}
}

void TrackList::selectPrevTrack()
{
	if (mTracks.size() > 1)
	{
		mTrackSelected = (mTrackSelected > 0) ? mTrackSelected - 1 : mTracks.size() - 1;
		_setDisplayIndex(mDisplayIndex);
	}
}

void TrackList::selectRandomTrack()
{
	if (mTracks.size() > 1)
	{
		mTrackSelected = Ogre::Math::RangeRandom(0, mTracks.size() - 1);
		_setDisplayIndex(mDisplayIndex);
	}
}

void TrackList::selectTrack(unsigned int index)
{
	if (index >= 0 && index < mTracks.size())
	{
		mTrackSelected = index;
		_setDisplayIndex(mDisplayIndex);
	}
}

void TrackList::deselectAllTracks()
{
	// deselect previous item selected
	if (mItemSelectedShown >= 0 && mItemSelectedShown < mItems.size())
		mItems[mItemSelectedShown]->setParameter("transparent", "true");
	mDecorVol->hide();

	mItemSelectedShown = -1;
	mTrackSelected = -1;
}

