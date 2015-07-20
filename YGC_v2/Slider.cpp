#include "stdafx.h"
#include "Slider.h"

Slider::Slider(const Ogre::String& name, Ogre::Real width, Ogre::Real minValue, Ogre::Real maxValue, unsigned int snaps) :
mDragOffset(0.0f),
mValue(0.0f),
mMinValue(0.0f),
mMaxValue(0.0f),
mInterval(0.0f)
{
	mDragging = false;
	mElement = Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("YgcGui/Slider", "BorderPanel", name);
	Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;
	mTrack = (Ogre::BorderPanelOverlayElement*) mElement;
	mTrackBack = (Ogre::BorderPanelOverlayElement*)mTrack->getChild(mTrack->getName() + "/SliderBack");
	mHandle = (Ogre::PanelOverlayElement*)mTrack->getChild(mTrack->getName() + "/SliderHandle");
	mTrack->setWidth(width);
	setRange(minValue, maxValue, snaps, false);
}

Slider::~Slider()
{
}

void Slider::_cursorPressed(const Ogre::Vector2& cursorPos)
{
	if (!mHandle->isVisible()) return;

	Ogre::Vector2 co = Widget::cursorOffset(mHandle, cursorPos);

	if (co.squaredLength() <= 81)
	{
		mDragging = true;
		mDragOffset = co.x;
	}
	else if (Widget::isCursorOver(mTrack, cursorPos))
	{
		Ogre::Real newLeft = mHandle->getLeft() + co.x;
		Ogre::Real rightBoundary = mTrack->getWidth() - mHandle->getWidth();

		mHandle->setLeft(Ogre::Math::Clamp<int>((int)newLeft, 0, (int)rightBoundary));
		setValue(_getSnappedValue(newLeft / rightBoundary));
	}
}

void Slider::_cursorReleased(const Ogre::Vector2& cursorPos)
{
	if (mDragging)
	{
		mDragging = false;
		mHandle->setLeft((int)((mValue - mMinValue) / (mMaxValue - mMinValue) *
			(mTrack->getWidth() - mHandle->getWidth())));
		mTrackBack->setWidth(mHandle->getLeft() + 15);
	}
}

void Slider::_cursorMoved(const Ogre::Vector2& cursorPos)
{
	if (mDragging)
	{
		Ogre::Vector2 co = Widget::cursorOffset(mHandle, cursorPos);
		Ogre::Real newLeft = mHandle->getLeft() + co.x - mDragOffset;
		Ogre::Real rightBoundary = mTrack->getWidth() - mHandle->getWidth();

		mHandle->setLeft(Ogre::Math::Clamp<int>((int)newLeft, 0, (int)rightBoundary));
		setValue(_getSnappedValue(newLeft / rightBoundary));
	}
}

void Slider::setRange(Ogre::Real minValue, Ogre::Real maxValue, unsigned int snaps, bool notifyListener /*= true*/)
{
	mMinValue = minValue;
	mMaxValue = maxValue;

	if (snaps <= 1 || mMinValue >= mMaxValue)
	{
		mInterval = 0;
		mHandle->hide();
		mValue = minValue;
	}
	else
	{
		mHandle->show();
		mInterval = (maxValue - minValue) / (snaps - 1);
		setValue(minValue, notifyListener);
	}
}

void Slider::setValue(Ogre::Real value, bool notifyListener /*= true*/)
{
	if (mInterval == 0) return;

	mValue = Ogre::Math::Clamp<Ogre::Real>(value, mMinValue, mMaxValue);

	if (mListener && notifyListener) mListener->sliderMoved(this);

	if (!mDragging) mHandle->setLeft((int)((mValue - mMinValue) / (mMaxValue - mMinValue) *
		(mTrack->getWidth() - mHandle->getWidth())));

	mTrackBack->setWidth(mHandle->getLeft() + 15);
}

Ogre::Real Slider::_getSnappedValue(Ogre::Real percentage)
{
	percentage = Ogre::Math::Clamp<Ogre::Real>(percentage, 0, 1);
	unsigned int whichMarker = (unsigned int)(percentage * (mMaxValue - mMinValue) / mInterval + 0.5);
	return whichMarker * mInterval + mMinValue;
}


