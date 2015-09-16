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
	void scrollUp();
	void scrollDown();
	void deselectAll();

	Ogre::String getSelectedPath() { return (mSelectDir) ? _getNormalizedItemSelected() : mCurrentPath.generic_string(); }
	Ogre::String getSelectedFile() { return  (mSelectedPath >= 0 && mSelectedPath <= mPaths.size()) ? mPaths[mSelectedPath].substr(3) : Ogre::StringUtil::BLANK; }
	void enableSelectDir(bool enable) { mSelectDir = enable; }
	
private:
	void _getPathItems();
	void _destroyItems();
	void _createTextAreaItems(const Ogre::String& name, const Ogre::String& caption, Ogre::Real top);
	void _setDisplayIndex(unsigned int index);
	void _setPathHome();
	Ogre::String _getNormalizedItemSelected();

	Ogre::OverlayContainer* mHeader, *mBody;
	Ogre::OverlayElement* mNextButton, *mPrevButton, *mHomeButton;
	Ogre::TextAreaOverlayElement* mTextAreaFullPath;
	Ogre::TextAreaOverlayElement* mTextAreaParentPath;
	Ogre::BorderPanelOverlayElement* mTrackFront;
	Ogre::PanelOverlayElement* mHandle;
	Ogre::StringVector mPaths;
	std::vector<Ogre::OverlayElement*> mItems;
	boost::filesystem::path mCurrentPath;
	unsigned int mItemsShown, mMaxItemsShown, mDisplayIndex;
	int mItemSelectedShown;
	int mSelectedPath;
	bool mDragging;
	bool mSelectDir;
	Ogre::Real mDragOffset;
};

#endif // #ifndef _FILEEXPLORER_H__

