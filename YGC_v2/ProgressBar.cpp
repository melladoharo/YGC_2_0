#include "stdafx.h"
#include "ProgressBar.h"

ProgressBar::ProgressBar(const Ogre::String& name, Ogre::Real width)
{
	mElement = Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("YgcGui/ProgressBar", "BorderPanel", name);
	mElement->setWidth(width);
	mElement->setLeft(-width / 2);
	mElement->setTop(-mElement->getHeight() - 15);
	Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;
	mTextArea = (Ogre::TextAreaOverlayElement*)c->getChild(getName() + "/ProgressCaption");
	mMeter = c->getChild(getName() + "/ProgressMeter");
	mMeter->setWidth(width - 10);
	mFill = (Ogre::PanelOverlayElement*)((Ogre::OverlayContainer*)mMeter)->getChild(mMeter->getName() + "/ProgressFill");
	nAux = (width / 20.0f) / 8.0f;
	// nAux = (width / max_width_interval) / num_intervals [0, 0.1, 0.2, 0.3, ..., 1] = 10
	// independientemente del tamaño del widget, los incrementos tienen siempre el mismo tamaño
}

ProgressBar::~ProgressBar()
{
}

void ProgressBar::setProgress(Ogre::Real progress)
{
	mProgress = Ogre::Math::Clamp<Ogre::Real>(progress, 0, 1);
	mFill->setWidth(std::max<int>((int)mFill->getHeight(), (int)(mProgress * (mMeter->getWidth() - 2 * mFill->getLeft()))));
	mFill->setTiling((mProgress * 10.0f) * nAux, 1);
}

