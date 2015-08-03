#include "stdafx.h"
#include "Widget.h"

Widget::Widget()
{
	mElement = 0;
	mListener = 0;
}

Widget::~Widget()
{
}

void Widget::nukeOverlayElement(Ogre::OverlayElement* element)
{
	Ogre::OverlayContainer* container = dynamic_cast<Ogre::OverlayContainer*>(element);
	if (container)
	{
		std::vector<Ogre::OverlayElement*> toDelete;

		Ogre::OverlayContainer::ChildIterator children = container->getChildIterator();
		while (children.hasMoreElements())
		{
			toDelete.push_back(children.getNext());
		}

		for (unsigned int i = 0; i < toDelete.size(); i++)
		{
			nukeOverlayElement(toDelete[i]);
		}
	}
	if (element)
	{
		Ogre::OverlayContainer* parent = element->getParent();
		if (parent) parent->removeChild(element->getName());
		Ogre::OverlayManager::getSingleton().destroyOverlayElement(element);
	}
}

bool Widget::isCursorOver(Ogre::OverlayElement* element, const Ogre::Vector2& cursorPos, Ogre::Real voidBorder /*= 0*/)
{
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	Ogre::Real l = element->_getDerivedLeft() * om.getViewportWidth();
	Ogre::Real t = element->_getDerivedTop() * om.getViewportHeight();
	Ogre::Real r = l + element->getWidth();
	Ogre::Real b = t + element->getHeight();

	return (cursorPos.x >= l + voidBorder && cursorPos.x <= r - voidBorder &&
		cursorPos.y >= t + voidBorder && cursorPos.y <= b - voidBorder);
}

Ogre::Vector2 Widget::cursorOffset(Ogre::OverlayElement* element, const Ogre::Vector2& cursorPos)
{
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	return Ogre::Vector2(cursorPos.x - (element->_getDerivedLeft() * om.getViewportWidth() + element->getWidth() / 2),
		cursorPos.y - (element->_getDerivedTop() * om.getViewportHeight() + element->getHeight() / 2));
}

Ogre::Real Widget::getCaptionWidth(const Ogre::DisplayString& caption, Ogre::TextAreaOverlayElement* area)
{
	Ogre::Font* font = (Ogre::Font*)Ogre::FontManager::getSingleton().getByName(area->getFontName()).getPointer();
	Ogre::String current = DISPLAY_STRING_TO_STRING(caption);
	Ogre::Real lineWidth = 0;

	for (unsigned int i = 0; i < current.length(); i++)
	{
		// be sure to provide a line width in the text area
		if (current[i] == ' ')
		{
			if (area->getSpaceWidth() != 0) lineWidth += area->getSpaceWidth();
			else lineWidth += font->getGlyphAspectRatio(' ') * area->getCharHeight();
		}
		else if (current[i] == '\n') break;
		// use glyph information to calculate line width
		else lineWidth += font->getGlyphAspectRatio(current[i]) * area->getCharHeight();
	}

	return (unsigned int)lineWidth;
}

Ogre::Real Widget::sizeInPixels(const Ogre::String& text, const Ogre::String nameFont, Ogre::Real heightFont, Ogre::Real spaceWidth)
{
	Ogre::Real textWidth = 0;
	Ogre::FontPtr font = Ogre::FontManager::getSingleton().getByName(nameFont);
	Ogre::Real sp = font->getCharacterSpacer() + font->getGlyphAspectRatio(' ');
	for (unsigned int i = 0; i < text.size(); ++i) 
	{
		textWidth += (text[i] == ' ') ? sp : font->getGlyphAspectRatio(text[i]) * heightFont;
	}
	return textWidth;
}

void Widget::_autoScale(unsigned int maxSize, Ogre::MaterialPtr matText)
{
	Ogre::Real sizeTexX = matText->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureDimensions().first;
	Ogre::Real sizeTexY = matText->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureDimensions().second;
	if (sizeTexX > sizeTexY)
	{
		mElement->setWidth(maxSize);
		mElement->setHeight((sizeTexY * maxSize) / sizeTexX);
	}
	else
	{
		mElement->setWidth((sizeTexX * maxSize) / sizeTexY);
		mElement->setHeight(maxSize);
	}
}

void Widget::fitCaptionToArea(const Ogre::DisplayString& caption, Ogre::TextAreaOverlayElement* area, Ogre::Real maxWidth)
{
	Ogre::Font* f = (Ogre::Font*)Ogre::FontManager::getSingleton().getByName(area->getFontName()).getPointer();
	Ogre::String s = DISPLAY_STRING_TO_STRING(caption);

	int nl = s.find('\n');
	if (nl != -1) s = s.substr(0, nl);

	Ogre::Real width = 0;

	for (unsigned int i = 0; i < s.length(); i++)
	{
		if (s[i] == ' ' && area->getSpaceWidth() != 0) width += area->getSpaceWidth();
		else width += f->getGlyphAspectRatio(s[i]) * area->getCharHeight();
		if (width > maxWidth)
		{
			s = s.substr(0, i);
			break;
		}
	}

	area->setCaption(s);
}

