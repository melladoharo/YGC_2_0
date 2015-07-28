#ifndef _FORMOVERVIEW_H__
#define _FORMOVERVIEW_H__

#include "FormBase.h"
#include "GameInfo.h"
#include "CModel.h"

enum eFOstatus{ FO_SHORTDESC, FO_ZOOM, FO_OPENVIEW };

class FormOverview : public FormBase
{
public: 
	FormOverview(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener = 0);
	~FormOverview();

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

private:
	void _createOverview();
	void _setCameraDvdClose();
	void _setCameraDvdZoom();
	void _setCameraDvdOpen();
	void _setPositionDisc();

	GameInfo* mGameInfo;
	OgreBites::SdkCameraMan* mCameraMan;
	Ogre::SceneNode* mTarget;
	eFOstatus mCurrentState;
	CModel* mDvdClose; // 3d model dvd case closed
	CModel* mDvdOpen; // 3d model open dvd case
	CModel* mDiscClose; // the first disc game, for the view with the close dvd
	std::vector<CModel*> mDiscOpen; // all the disc game
	bool mCtrlPressed, mAltPressed, mShiftPressed;
};

#endif // #ifndef _FORMOVERVIEW_H__

