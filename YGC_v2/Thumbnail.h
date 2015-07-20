#ifndef _THUMBNAIL_H__
#define _THUMBNAIL_H__

#include "Widget.h"

class Thumbnail : public Widget
{
public: 
	Thumbnail(const Ogre::String& name, const Ogre::String& nameGroupRes, const Ogre::String& caption, const Ogre::String& textName, int size);
	~Thumbnail();

	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _cursorReleased(const Ogre::Vector2& cursorPos);
	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void _focusLost();

	bool isOver() { return (mState == BS_OVER); }
	void showCaption();
	void hideCaption();
	void setIndex(unsigned int newPos) { mIndex = newPos; }
	unsigned int getIndex() const { return mIndex; }
	void setMaterial(Ogre::String nameMaterial) { mThumbImage->setMaterialName(nameMaterial); }

	static void _setThumbsInGrid(std::vector<Thumbnail*>& thumbs, Ogre::Real left, Ogre::Real top, 
		Ogre::Real width, Ogre::Real height, int maxRows);

private:
	Ogre::Real _getProportionalSize(Ogre::Real maxSize);

	Ogre::BorderPanelOverlayElement* mThumbBackground, *mThumbImage, *mThumbPanelCaption;
	Ogre::TextAreaOverlayElement* mTextArea;
	Ogre::MaterialPtr matThumb;
	Ogre::Real mBorder, mZoom;
	ButtonState mState;
	int mIndex; // the position on a grid [thumbnails - thumbnailhit]
};

#endif // #define _THUMBNAIL_H__

