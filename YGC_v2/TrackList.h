#ifndef _TRACKLIST_H__
#define _TRACKLIST_H__

#include "Widget.h"

class TrackList : public Widget
{
public:
	TrackList(const Ogre::String& name, const Ogre::StringVector& tracks, Ogre::Real left, Ogre::Real top, unsigned int maxItemShown);
	~TrackList();

	// mouse managment
	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _cursorReleased(const Ogre::Vector2& cursorPos);
	void _focusLost() {}
	
	void setMaxItemShown(unsigned int maxItems);

	void setTracks(const Ogre::StringVector& tracks);
	Ogre::StringVector getTracks() { return mTracks; }
	bool isEmpty() { return mTracks.empty(); }

	Ogre::String getSelectedTrack();
	void selectNextTrack();
	void selectPrevTrack();
	void selectRandomTrack();
	void selectTrack(unsigned int index);
	void deselectAllTracks();

private:
	void _setDisplayIndex(unsigned int index);
	void _destroyItems();

	const Ogre::String mNameWidget;
	Ogre::TextAreaOverlayElement* mTextCaption;
	Ogre::OverlayElement* mSeparator;
	std::vector<Ogre::OverlayElement*> mItems;
	Ogre::OverlayElement* mDecorVol;
	Ogre::BorderPanelOverlayElement* mTrackFront;
	Ogre::OverlayElement* mHandle;
	Ogre::StringVector mTracks;
	unsigned int mItemsShown, mMaxItemsShown, mDisplayIndex;
	int mItemSelectedShown;
	int mTrackSelected;
	bool mDragging;
	Ogre::Real mDragOffset;
};

#endif // #ifndef _TRACKLIST_H__

