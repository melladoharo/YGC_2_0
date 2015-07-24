#ifndef _FILEEXPLORER_H__
#define _FILEEXPLORER_H__

#include "Widget.h"


class FileExplorer : public Widget
{
public:
	FileExplorer(const Ogre::String& name, const Ogre::String& path, Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real height);
	~FileExplorer();

	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _cursorReleased(const Ogre::Vector2& cursorPos);
	void _focusLost() {}

	void setPath(const Ogre::String& path);
	Ogre::String getSelectedPath() { return mSelectedPath.generic_string(); }
	void scrollUp();
	void scrollDown();

private:
	void _getPathItems();
	void _destroyItems();
	void _createTextAreaItems(const Ogre::String& name, const Ogre::String& caption, Ogre::Real top);
	void _setDisplayIndex(unsigned int index);
	void _setPathHome();

	Ogre::OverlayContainer* mHeader, *mBody;
	Ogre::OverlayElement* mNextButton, *mPrevButton, *mHomeButton;
	Ogre::TextAreaOverlayElement* mTextAreaFullPath;
	Ogre::TextAreaOverlayElement* mTextAreaParentPath;
	Ogre::BorderPanelOverlayElement* mTrackFront;
	Ogre::PanelOverlayElement* mHandle;
	Ogre::StringVector mItems;
	std::vector<Ogre::OverlayElement*> mItemElements;
	boost::filesystem::path mPath;
	boost::filesystem::path mSelectedPath;
	unsigned int mItemsShown, mMaxItemsShown, mDisplayIndex;
	int mSelectionIndex;
	bool mDragging;
	Ogre::Real mDragOffset;
};

#endif // #ifndef _FILEEXPLORER_H__

