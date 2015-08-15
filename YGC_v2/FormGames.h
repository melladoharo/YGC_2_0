#ifndef _FORMGAMES_H__
#define _FORMGAMES_H__

#include "FormBase.h"
#include "GameInfo.h"
#include "FormSelectorGame.h"
#include "FormNewGame.h"
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
	void hideAllOptions();
	void hideOptions();
	void showOptions();
	void hideOptionsThumbs();
	void showOptionsThumbs();
	void hideOptionsDelete();
	void showOptionsDelete();

	// GuiListener
	void buttonHit(Button* button);
	void sliderMoved(Slider* slider);
	void itemSelected(SelectMenu* menu);
	void labelHit(Label* label);
	void itemChanged(ItemSelector* selector);
	void sliderOptionsMoved(SliderOptions* slider);
	void yesNoDialogClosed(const Ogre::DisplayString& question, bool yesHit);

private:
	void _addNewGame(const Ogre::String& newGame);
	void _removeGame(unsigned int index);
	void _removeThumbs();
	void _adjustThumbs();
	void _filterGamesAll();
	void _filterGamesByInstalled();

	FormSelectorGame* mFormSelector;
	FormNewGame* mFormNewGame;
	Ogre::String mPathGames;
	std::vector<GameInfo*> mGameInfo;
	std::vector<sInfoResource> mInfoHeader;
	std::vector<Thumbnail3D*> mThumbs;
	Ogre::SceneNode* mParentThumbs;
	Thumbnail3D* mLastThumbOver;
	sGridThumbs mGridThumbs;
	Ogre::DisplayString mQuestionDelete;
};

#endif // #ifndef _FORMGAMES_H__

