#include "stdafx.h"
#include "SimpleText.h"

SimpleText::SimpleText(const Ogre::String& name, const Ogre::String& caption, const Ogre::String& nameFont, int oveCharHeight, Ogre::Real width, Ogre::Real height, Ogre::Real oveLineSep /*= 0*/) :
mText(caption),
mLineSep(oveLineSep),
mCharHeight(oveCharHeight),
mScrollPercentage(0),
mStartingLine(0),
mFont(Ogre::FontManager::getSingletonPtr()->getByName(nameFont)),
mColorText(Ogre::ColourValue(0.97f, 0.99f, 1))
{
	
	// Texbox: area (divided into lines)
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	mElement = static_cast<Ogre::OverlayContainer*>(om.createOverlayElement("Panel", name));
	mElement->setMetricsMode(Ogre::GMM_PIXELS);
	mElement->setWidth(width);
	mElement->setHeight(height);
	mTextArea = static_cast<Ogre::TextAreaOverlayElement*>(om.createOverlayElement("TextArea", "SimpleText/Caption" + name));
	mTextArea->setFontName(mFont->getName());
	mTextArea->setCharHeight(mCharHeight);
	mTextArea->setSpaceWidth(8);
	Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;
	c->addChild(mTextArea);

	setText(mText);
}

SimpleText::~SimpleText()
{
}

void SimpleText::setText(const Ogre::DisplayString& text)
{
	mText = text;
	_removeTextArea(mLines);
	mLines.clear();

	Ogre::Font* font = (Ogre::Font*)Ogre::FontManager::getSingleton().getByName(mTextArea->getFontName()).getPointer();

	Ogre::String current = DISPLAY_STRING_TO_STRING(text);
	bool firstWord = true;
	unsigned int lastSpace = 0;
	unsigned int lineBegin = 0;
	Ogre::Real lineWidth = 0;
	//Ogre::Real rightBoundary = mElement->getWidth() - 2 * mPadding + mScrollTrack->getLeft() + 10;
	Ogre::Real rightBoundary = mElement->getWidth();

	for (unsigned int i = 0; i < current.length(); i++)
	{
		if (current[i] == ' ')
		{
			if (mTextArea->getSpaceWidth() != 0) lineWidth += mTextArea->getSpaceWidth();
			else lineWidth += font->getGlyphAspectRatio(' ') * mTextArea->getCharHeight();
			firstWord = false;
			lastSpace = i;
		}
		else if (current[i] == '\n' || current[i] == '\r')
		{
			firstWord = true;
			lineWidth = 0;
			mLines.push_back(current.substr(lineBegin, i - lineBegin));
			lineBegin = i + 1;
		}
		else
		{
			// use glyph information to calculate line width
			lineWidth += font->getGlyphAspectRatio(current[i]) * (mTextArea->getCharHeight());
			if (lineWidth > rightBoundary)
			{
				if (firstWord)
				{
					current.insert(i, "\n");
					i = i - 1;
				}
				else
				{
					current[lastSpace] = '\n';
					i = lastSpace - 1;
				}
			}
		}
	}

	mLines.push_back(current.substr(lineBegin));

	unsigned int maxLines = _getHeightInLines();

	if (mLines.size() > maxLines)     // if too much text, filter based on scroll percentage
	{
		_filterLines();
	}
	else       // otherwise just show all the text
	{
		_createTextArea(mLines);
		mScrollPercentage = 0;
	}
}

void SimpleText::_filterLines()
{
	Ogre::String shown = "";
	Ogre::StringVector lines;
	unsigned int maxLines = _getHeightInLines();
	unsigned int newStart = (unsigned int)(mScrollPercentage * (mLines.size() - maxLines) + 0.5);

	mStartingLine = newStart;

	for (unsigned int i = 0; i < maxLines; i++)
	{
		shown += mLines[mStartingLine + i] + "\n";
		lines.push_back(mLines[mStartingLine + i] + "\n");
	}

	// show just the filtered lines
	if (vTextArea.empty()) _createTextArea(lines);
	else _copyTextArea(lines);
}

unsigned int SimpleText::_getHeightInLines()
{
	//return (unsigned int)((mElement->getHeight() - 2 * mPadding + 5) / mTextArea->getCharHeight());
	return (unsigned int)((mElement->getHeight()) / mTextArea->getCharHeight());
}

void SimpleText::_createTextArea(Ogre::StringVector lines)
{
	int posY = 0;

	// Creates a new textAre for each line
	for (unsigned int i = 0; i < lines.size(); i++)
	{
		Ogre::String nameText = mElement->getName() + "/LineTextArea/" + Ogre::StringConverter::toString(i) + mElement->getName();
		Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
		vTextArea.push_back(static_cast<Ogre::TextAreaOverlayElement*>(om.createOverlayElement("TextArea", nameText)));
		vTextArea.back()->setMetricsMode(Ogre::GMM_PIXELS);
		vTextArea.back()->setCaption(lines[i]);
		vTextArea.back()->setFontName(mFont->getName());
		vTextArea.back()->setCharHeight(mCharHeight);
		vTextArea.back()->setTop(posY);
		vTextArea.back()->setAlignment(Ogre::TextAreaOverlayElement::Left);
		vTextArea.back()->setColour(mColorText);
		Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;
		c->addChild(vTextArea.back());
		posY += vTextArea.back()->getCharHeight() + mLineSep;
	}
}

void SimpleText::_removeTextArea(Ogre::StringVector lines)
{
	// Creates a new textAre for each line
	for (unsigned int i = 0; i < lines.size(); i++)
	{
		Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;
		c->removeChild(vTextArea[i]->getName());
		Widget::nukeOverlayElement(vTextArea[i]);
		//delete vTextArea[i];
	}
	vTextArea.clear();
}

void SimpleText::_copyTextArea(Ogre::StringVector lines)
{
	for (unsigned int i = 0; i < lines.size(); ++i)
		vTextArea[i]->setCaption(lines[i]);
}

void SimpleText::setTextColor(const Ogre::ColourValue& newCol)
{
	mColorText = newCol;
	for (unsigned int i = 0; i < vTextArea.size(); ++i)
	{
		vTextArea[i]->setColour(mColorText);
	}
}

Ogre::Real SimpleText::getMaxSizeLine()
{
	Ogre::Real maxLine = 0.0f;
	
	for (unsigned int i = 0; i < mLines.size(); ++i)
	{
		Ogre::Real currLine = sizeInPixels(vTextArea[i]->getCaption(), vTextArea[i]->getFontName(), 
			vTextArea[i]->getCharHeight(), vTextArea[i]->getSpaceWidth());
		if (currLine > maxLine)
			maxLine = currLine;
	}

	return maxLine;
}
