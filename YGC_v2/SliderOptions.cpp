#include "stdafx.h"
#include "SliderOptions.h"


SliderOptions::SliderOptions(const Ogre::String& name, const Ogre::String& caption, Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real minValue, Ogre::Real maxValue, unsigned int snaps) :
mDragOffset(0.0f),
mDragging(false),
mValue(0.0f),
mMinValue(0.0f),
mMaxValue(0.0f),
mInterval(0.0f),
mDisableBackground(false)
{
	mElement = Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("YgcGui/SliderOption", "Panel", name);
	Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;
	mTextArea = (Ogre::TextAreaOverlayElement*)c->getChild(c->getName() + "/SliderOptionCaption");
	c = (Ogre::OverlayContainer*)c->getChild(c->getName() + "/SliderOptionContainer");
	mTrackFront = (Ogre::BorderPanelOverlayElement*)c->getChild(c->getName() + "/SliderOptionTrackFront");
	mTrackBack = (Ogre::BorderPanelOverlayElement*)c->getChild(c->getName() + "/SliderOptionTrackBack");
	mHandle = (Ogre::PanelOverlayElement*)c->getChild(c->getName() + "/SliderOptionHandle");

	// spacing elements - caption 
	mElement->setLeft(left);
	mElement->setWidth(width);
	mElement->setTop(top);
	Ogre::Real sizeCaption = sizeInPixels(mTextArea->getCaption(), mTextArea->getFontName(), mTextArea->getCharHeight(), mTextArea->getSpaceWidth());
	mTextArea->setCaption(caption);
	Ogre::Real sizeTracks = (mElement->getWidth() - sizeCaption) / 2;
	c->setWidth(sizeTracks);
	c->setLeft(-c->getWidth() - 12);
	mTrackBack->setWidth(sizeTracks);
	setRange(minValue, maxValue, snaps, false);
}

SliderOptions::~SliderOptions()
{
}



void SliderOptions::_cursorMoved(const Ogre::Vector2& cursorPos)
{
	if (!mDisableBackground)
	{
		if (isCursorOver(mElement, cursorPos)) mElement->setParameter("transparent", "false");
		else mElement->setParameter("transparent", "true");
	}
	
	if (mDragging)
	{
		Ogre::Vector2 co = Widget::cursorOffset(mHandle, cursorPos);
		Ogre::Real newLeft = mHandle->getLeft() + co.x + 6;
		Ogre::Real rightBoundary = mTrackBack->getWidth();

		mHandle->setLeft(Ogre::Math::Clamp<int>((int)newLeft, 0, (int)rightBoundary));
		setValue(_getSnappedValue(newLeft / rightBoundary));
	}
}

void SliderOptions::_cursorPressed(const Ogre::Vector2& cursorPos)
{
	if(isCursorOver(mTrackBack, cursorPos))
	{
		mDragging = true;
		Ogre::Vector2 co = Widget::cursorOffset(mHandle, cursorPos);
		Ogre::Real newLeft = mHandle->getLeft() + co.x + 6;
		Ogre::Real rightBoundary = mTrackBack->getWidth();

		mHandle->setLeft(Ogre::Math::Clamp<int>((int)newLeft, 0, (int)rightBoundary));
		setValue(_getSnappedValue(newLeft / rightBoundary));
	}
}

void SliderOptions::_cursorReleased(const Ogre::Vector2& cursorPos)
{
	mDragging = false;
}



void SliderOptions::setRange(Ogre::Real minValue, Ogre::Real maxValue, unsigned int snaps, bool notifyListener /*= true*/)
{
	mMinValue = minValue;
	mMaxValue = maxValue;

	if (snaps <= 1 || mMinValue >= mMaxValue)
	{
		mInterval = 0;
		mValue = minValue;
	}
	else
	{
		mInterval = (maxValue - minValue) / (snaps - 1);
		setValue(minValue, notifyListener);
	}
}

void SliderOptions::setValue(Ogre::Real value, bool notifyListener /*= true*/)
{
	if (mInterval == 0) return;

	mValue = Ogre::Math::Clamp<Ogre::Real>(value, mMinValue, mMaxValue);

	if (mListener && notifyListener) mListener->sliderOptionsMoved(this);

	mHandle->setLeft((int)((mValue - mMinValue) / (mMaxValue - mMinValue) * (mTrackBack->getWidth())));
	mTrackFront->setWidth(mHandle->getLeft());
	mHandle->setLeft(mHandle->getLeft() - 5);
}

Ogre::Real SliderOptions::_getSnappedValue(Ogre::Real percentage)
{
	percentage = Ogre::Math::Clamp<Ogre::Real>(percentage, 0, 1);
	unsigned int whichMarker = (unsigned int)(percentage * (mMaxValue - mMinValue) / mInterval + 0.5);
	return whichMarker * mInterval + mMinValue;
}
