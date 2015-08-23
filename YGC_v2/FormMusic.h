#ifndef _FORMMUSIC_H__
#define _FORMMUSIC_H__

#include "FormBase.h"
#include "GameInfo.h"
#include "TrackList.h"
#include "UtilsOgreDshow.h"

class FormMusic : public FormBase
{
public:
	FormMusic(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener = 0);
	~FormMusic();

	// Ogre::FrameListener
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	// OIS::KeyListener
	bool keyPressed(const OIS::KeyEvent &arg);
	bool keyReleased(const OIS::KeyEvent &arg);

	// OIS::MouseListener
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	// GuiListener
	void sliderMoved(Slider* slider);
	void trackListHit(TrackList* track);
	void mediaPlayerHit(MediaPlayer* player);

	// FormBase
	void hide();
	void show();
	void showOptions();

	// Music Controls
	void playTrack(unsigned int index);

private:
	GameInfo* mGameInfo;
	CModel* mDiscClose;
	std::vector<sInfoResource> mInfoTracks;
	TrackList* mTrackList;
	DirectShowSound* mAudioPlayer;
	unsigned int mCurrentIndex;
};

#endif // #ifndef _FORMMUSIC_H__
