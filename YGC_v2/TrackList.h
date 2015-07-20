#ifndef _TRACKLIST_H__
#define _TRACKLIST_H__

#include "Widget.h"

// Item option
struct sItemTrack
{
	Ogre::OverlayContainer* panel;
	Ogre::TextAreaOverlayElement* textArea;
	ButtonState currentState;
};

class TrackList : public Widget
{
public:
	TrackList(const Ogre::String& name, const Ogre::StringVector& tracks, Ogre::Real left, Ogre::Real top);
	~TrackList();

	// mouse managment
	void _cursorPressed(const Ogre::Vector2& cursorPos);
	void _cursorReleased(const Ogre::Vector2& cursorPos);
	void _cursorMoved(const Ogre::Vector2& cursorPos);
	void _focusLost(){ setStateTrack(BS_UP, mIndexOver); }
	void setStateTrack(const ButtonState& bs, int index);
	void setStateButton(const ButtonState& bs, sMiniButton& button);

	void selectTrack(unsigned int index) { setStateTrack(BS_UP, mIndexSelected); mIndexSelected = index; setStateTrack(BS_SELECTED, mIndexSelected); }
	void deselectTrack(unsigned int index) { setStateTrack(BS_UP, index); }
	unsigned int getTrackIndex() { return mIndexSelected; }

private:
	void _createNewTrack(const Ogre::String& namePanel, const Ogre::String& caption, Ogre::Real leftTrack, Ogre::Real topTrack, Ogre::Real widthTrack);
	void _destroyAllItems();
	void _viewPage(unsigned int index);

	std::vector<sItemTrack> mItems; // Items options
	sMiniButton mBttPrevious, mBttNext;
	Ogre::FontPtr mFont; // Font used for items
	Ogre::ColourValue mColourSelect, mColourDeselect; // Colours when mouse is over an item 
	unsigned int mIndexOver, mIndexSelected; // Current index item over, and current item selected
	unsigned int mMaxRows;
	unsigned int mCurrentPage;
};

#endif // #ifndef _TRACKLIST_H__

