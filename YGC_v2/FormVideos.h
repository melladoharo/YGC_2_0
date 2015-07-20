#ifndef _FORMVIDEOS_H__
#define _FORMVIDEOS_H__

#include "FormBase.h"
#include "GameInfo.h"
#include "videoplayer.hpp"
#include "SDLMovieAudioFactory.h"
#include "Thumbnail3D.h"

class FormVideos : public FormBase
{
public:
	FormVideos(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener = 0);
	~FormVideos();

	// Ogre::FrameListener
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	
	// OIS::KeyListener
	bool keyPressed(const OIS::KeyEvent &arg);
	bool keyReleased(const OIS::KeyEvent &arg);

	// OIS::MouseListener
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	// FormBase
	void hide();
	void show();
	void showOptions();

	// GuiListener
	void sliderMoved(Slider* slider);
	void itemSelected(SelectMenu* menu);
	void medialPlayerHit(MediaPlayer* player);

	// Form video
	void playVideo(unsigned int index, bool fullscreen = true);

private:
	void _filterVideosByIndex(std::pair<unsigned int, unsigned int> indexFilter);
	void _createThumbFromVideo(const Ogre::String& pathVideo, const Ogre::String& pathThumb, unsigned int thumbHeight);

	GameInfo* mGameInfo;
	std::vector<sInfoResource> mVideos;
	std::vector<Thumbnail3D*> mThumbs;
	Ogre::SceneNode* mParentThumbs;
	Thumbnail3D* mLastThumbOver;
	Video::VideoPlayer* mVideoPlayer;
	Ogre::Real mTimeOver;
	std::pair<unsigned int, unsigned int> mIndexTrailer, mIndexGameplay;
	unsigned int mCurrentIndex;
};

#endif // #ifndef _FORMVIDEOS_H__

