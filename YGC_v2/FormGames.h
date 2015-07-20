#ifndef _FORMGAMES_H__
#define _FORMGAMES_H__

#include "FormBase.h"
#include "GameInfo.h"
#include "FormSelectorGame.h"
#include "Thumbnail3D.h"

class FormGames : public FormBase
{
public:
	FormGames(const Ogre::String& pathGames, GuiManager* tray, GuiListener* oldListener = 0);
	~FormGames();

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

private:
	void _removeThumbs();
	void _adjustThumbs();
	void _filterGamesAll();
	void _filterGamesByInstalled();

	FormSelectorGame* mFormSelector;
	std::vector<GameInfo*> mGameInfo;
	std::vector<sInfoResource> mInfoHeader;
	std::vector<Thumbnail3D*> mThumbs;
	Ogre::SceneNode* mParentThumbs;
	Thumbnail3D* mLastThumbOver;
};

#endif // #ifndef _FORMGAMES_H__

