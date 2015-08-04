#ifndef _FORMMODELS_H__
#define _FORMMODELS_H__

#include "FormBase.h"
#include "GameInfo.h"
#include "CModel.h"
#include "ModelController.h"

struct sModel
{
	Ogre::String pathIni;
	Ogre::String name;
	CModel* model;
	CSimpleIniA* simpleIni;
	SI_Error iniStatus;
	std::vector<sView> views;
	unsigned int currentView;
};

enum eFMstatus{ FM_OVERVIEW, FM_ZOOM, FM_DETAILS, FM_EDITVIEWS, FM_EDITCAMERA};

class FormModels : public FormBase
{
public:
	FormModels(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener = 0);
	~FormModels();
	
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
	void buttonHit(Button* button);
	void sliderMoved(Slider* slider);
	void sliderOptionsMoved(SliderOptions* slider);
	void labelHit(Label* label);
	void itemChanged(ItemSelector* selector);

	// FormBase
	void hide();
	void show();

	// FormModels
	void hideAllOptions();
	void hideOptions();
	void showOptions();
	void hideOptionEditModel();
	void showOptionEditModel();
	void hideOptionEditPosition();
	void showOptionEditPosition();
	void hideOptionViews();
	void showOptionViews(sModel& model);
	void hideOptionCamera();
	void showOptionCamera();
	void hideOptionTarget();
	void showOptionTarget();

private:
	void _loadModel(const sInfoResource& infoModel);
	void _showModel(unsigned int index);
	void _setOrbitCamera();
	void _setFormToOverview();
	void _setFormToZoom();
	void _setFormToDetails();
	bool _loadCameraView(sModel& model);
	bool _saveCameraView(sModel& model);
	bool _deleteCameraView(sModel& model);
	void _showCameraView(sModel& model);
	void _newCameraView(sModel& model);
	void _loadConfigWindowsViews(sModel& model, bool newView = false);
	void _playAnimation(sModel& model);

	GameInfo* mGameInfo;
	OgreBites::SdkCameraMan* mCameraMan;
	eFMstatus mState;
	std::vector<sModel> mModels;
	Ogre::SceneNode* mTarget;
	CModelController* mController;
	unsigned int mCurrentIndex;
	bool mCtrlPressed, mAltPressed, mShiftPressed;
};

#endif // #ifndef _FORMMODELS_H__

