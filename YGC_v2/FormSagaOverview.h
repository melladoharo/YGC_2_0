#ifndef _FORMSAGAOVERVIEW_H__
#define _FORMSAGAOVERVIEW_H__

#include "FormBase.h"
#include "GameInfo.h"

class FormSagaOverview : public FormBase
{
public:
	FormSagaOverview(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener);
	~FormSagaOverview();

	// OIS::KeyListener
	bool keyPressed(const OIS::KeyEvent &arg);
	bool keyReleased(const OIS::KeyEvent &arg);

	// OIS::MouseListener
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	// Menu options
	void hideAllOptions();
	void hideOptions();
	void showOptions();

private:
	void _showRandomWallpaper();
	Ogre::StringVector _getNameGames(const Ogre::String& pathIni);
	void _addDvdModel(const Ogre::String& pathGame);
	void _addDiscModel(const Ogre::String& pathGame);
	void _setCameraSaga();
	void _setPositionModels();
	Ogre::Vector3 _getCenterSaga();

	GameInfo* mGameInfo;
	OgreBites::SdkCameraMan* mCameraMan;
	Ogre::SceneNode* mTarget;
	std::vector<CModel*> mDvdModel;
	std::vector<CModel*> mDiscModel;
	bool mCtrlPressed, mAltPressed, mShiftPressed;
};

#endif // #ifndef _FORMSAGAOVERVIEW_H__

