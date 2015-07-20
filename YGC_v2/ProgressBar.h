#ifndef _PROGRESSBAR_H__
#define _PROGRESSBAR_H__

#include "Widget.h"

class ProgressBar : public Widget
{
public:
	ProgressBar(const Ogre::String& name, Ogre::Real width);
	~ProgressBar();

	void setProgress(Ogre::Real progress);
	Ogre::Real getProgress() { return mProgress; }
	void setCaption(const Ogre::DisplayString& caption) { mTextArea->setCaption(caption); }
	const Ogre::DisplayString& getCaption() { return mTextArea->getCaption(); }

private:
	Ogre::TextAreaOverlayElement* mTextArea;
	Ogre::OverlayElement* mMeter;
	Ogre::PanelOverlayElement* mFill;
	Ogre::Real mProgress, nAux;
};

#endif // #ifndef _PROGRESSBAR_H__

