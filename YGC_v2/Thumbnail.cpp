#include "stdafx.h"
#include "Thumbnail.h"

Thumbnail::Thumbnail(const Ogre::String& name, const Ogre::String& nameGroupRes, const Ogre::String& caption, const Ogre::String& textName, int size) :
mBorder(6),
mZoom(35),
mState(BS_UP),
mIndex(0)
{
	mElement = Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("YgcGui/Thumbnail", "BorderPanel", name);
	mThumbBackground = (Ogre::BorderPanelOverlayElement*)mElement;
	mThumbImage = (Ogre::BorderPanelOverlayElement*) mThumbBackground->getChild(mThumbBackground->getName() + "/ThumbImage");
	mThumbPanelCaption = (Ogre::BorderPanelOverlayElement*) mThumbImage->getChild(mThumbImage->getName() + "/ThumbPanelCaption");
	mThumbPanelCaption->hide();
	mTextArea = (Ogre::TextAreaOverlayElement*)mThumbPanelCaption->getChild(mThumbPanelCaption->getName() + "/ThumbCaption");
	//mTextArea->setCaption(caption); [fitCaptionToArea]
	mTextArea->hide();

	matThumb = Ogre::MaterialManager::getSingleton().create("Mat/YgcGui/Thumbnail/" + name, nameGroupRes);
	matThumb->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
	matThumb->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	matThumb->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	matThumb->getTechnique(0)->getPass(0)->createTextureUnitState(textName);
	mThumbImage->setMaterialName(matThumb->getName());

	// auto scale aspect ratio
	mElement->setWidth(_getProportionalSize(size));
	mElement->setHeight(size);
	// scale down thumb image size to see the background
	mThumbImage->setWidth(mThumbBackground->getWidth() - mBorder);
	mThumbImage->setHeight(mThumbBackground->getHeight() - mBorder);
	// center thumb image in widget
	mThumbImage->setTop((mThumbBackground->getHeight() - mThumbImage->getHeight()) / 2);
	mThumbImage->setLeft((mThumbBackground->getWidth() - mThumbImage->getWidth()) / 2);
	// caption panel
	mThumbPanelCaption->setWidth(mThumbBackground->getWidth());
	mThumbPanelCaption->setHeight(29);
	mThumbPanelCaption->setLeft(mThumbImage->getLeft() - mBorder);
	mThumbPanelCaption->setVerticalAlignment(Ogre::GVA_BOTTOM);
	mTextArea->setTop(-22);
	fitCaptionToArea(caption, mTextArea, mThumbBackground->getWidth() + 60);
}

Thumbnail::~Thumbnail()
{
	Ogre::MaterialManager::getSingleton().unload(matThumb->getName());
	Ogre::MaterialManager::getSingleton().remove(matThumb->getName());
}

void Thumbnail::_cursorPressed(const Ogre::Vector2& cursorPos)
{
	if (isCursorOver(mThumbBackground, cursorPos, 8))
		mState = BS_DOWN;
}

void Thumbnail::_cursorReleased(const Ogre::Vector2& cursorPos)
{
	if (mState == BS_DOWN)
	{
		mState = BS_OVER;
		if (mListener) mListener->thumbnailHit(this);
	}
}

void Thumbnail::_cursorMoved(const Ogre::Vector2& cursorPos)
{
	if (isCursorOver(mThumbBackground, cursorPos, 10))
	{
		if (mState == BS_UP) { mState = BS_OVER; showCaption(); }
	}
	else
	{
		if (mState != BS_UP) { mState = BS_UP; hideCaption(); }
	}
}

void Thumbnail::_focusLost()
{
	mState = BS_UP;
}

void Thumbnail::showCaption()
{
	if (mTextArea->getCaption() != Ogre::StringUtil::BLANK)
	{
		mThumbPanelCaption->show();
		mTextArea->show();
	}
	
	Ogre::Real newZoomX = _getProportionalSize(mZoom);
	mThumbBackground->setWidth(mThumbBackground->getWidth() + newZoomX);
	mThumbBackground->setHeight(mThumbBackground->getHeight() + mZoom);
	mThumbBackground->setLeft(mThumbBackground->getLeft() - (newZoomX / 2));
	mThumbBackground->setTop(mThumbBackground->getTop() - (mZoom / 2));
	mThumbImage->setWidth(mThumbImage->getWidth() + newZoomX);
	mThumbImage->setHeight(mThumbImage->getHeight() + mZoom);
	mThumbPanelCaption->setWidth(mThumbPanelCaption->getWidth() + newZoomX);
}

void Thumbnail::hideCaption()
{
	if (mTextArea->getCaption() != Ogre::StringUtil::BLANK)
	{
		mThumbPanelCaption->hide();
		mTextArea->hide();
	}

	Ogre::Real newZoomX = _getProportionalSize(mZoom);
	mThumbBackground->setWidth(mThumbBackground->getWidth() - newZoomX);
	mThumbBackground->setHeight(mThumbBackground->getHeight() - mZoom);
	mThumbBackground->setLeft(mThumbBackground->getLeft() + (newZoomX / 2));
	mThumbBackground->setTop(mThumbBackground->getTop() + (mZoom / 2));
	mThumbImage->setWidth(mThumbImage->getWidth() - newZoomX);
	mThumbImage->setHeight(mThumbImage->getHeight() - mZoom);
	mThumbPanelCaption->setWidth(mThumbPanelCaption->getWidth() - newZoomX);
}

void Thumbnail::_setThumbsInGrid(std::vector<Thumbnail*>& thumbs, 
	Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real height, int maxRows)
{
	int leftThumb = left, topThumb = top, row = 0;

	for (unsigned int i = 0; i < thumbs.size(); ++i)
	{
		thumbs[i]->getOverlayElement()->setLeft(leftThumb + (width - thumbs[i]->getWidth()) / 2);
		//thumbs[i]->getOverlayElement()->setTop(topThumb + (height - thumbs[i]->getHeight()) / 2);
		thumbs[i]->getOverlayElement()->setTop(topThumb);

		row++;
		topThumb += height;
		if (row >= maxRows)
		{
			leftThumb += width;
			topThumb = top;
			row = 0;
		}
	}
}

Ogre::Real Thumbnail::_getProportionalSize(Ogre::Real maxSize)
{
	Ogre::Real sizeTexX = matThumb->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureDimensions().first;
	Ogre::Real sizeTexY = matThumb->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureDimensions().second;

	return (sizeTexX * maxSize) / sizeTexY;
}

