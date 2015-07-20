#ifndef _SIMPLETEXT_H__
#define _SIMPLETEXT_H__

#include "Widget.h"

class SimpleText : public Widget
{
public:
	SimpleText(const Ogre::String& name, const Ogre::String& caption, const Ogre::String& nameFont,
		int oveCharHeight, Ogre::Real width, Ogre::Real height, Ogre::Real oveLineSep = 0);
	~SimpleText();

	unsigned int getNumLines() { return vTextArea.size(); }
	Ogre::Real getMaxSizeLine();
	void setTextColor(const Ogre::ColourValue& newCol);
	void setText(const Ogre::DisplayString& text);
	Ogre::String getText() { return mText; }
	void setCaptionHeight(Ogre::Real captionHeight) 
	{ 
		mCharHeight = captionHeight;
		mTextArea->setCharHeight(captionHeight);
		setText(mText);
	}
	Ogre::FontPtr getFont() { return mFont; }

private:
	void _filterLines();
	unsigned int _getHeightInLines();
	void _createTextArea(Ogre::StringVector lines);
	void _removeTextArea(Ogre::StringVector lines);
	void _copyTextArea(Ogre::StringVector lines);

	Ogre::TextAreaOverlayElement* mTextArea;
	std::vector<Ogre::TextAreaOverlayElement*> vTextArea;
	Ogre::FontPtr mFont;

	Ogre::DisplayString mText;
	Ogre::StringVector mLines;
	Ogre::Real mLineSep;
	Ogre::Real mCharHeight;
	unsigned int mStartingLine;
	Ogre::Real mScrollPercentage;
	Ogre::ColourValue mColorText;
};

#endif // #ifndef _SIMPLETEXT_H__

