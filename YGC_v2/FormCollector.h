#ifndef _FORMCOLLECTOR_H__
#define _FORMCOLLECTOR_H__

#include "FormBase.h"
#include "GameInfo.h"
#include "CModel.h"
#include "ModelController.h"

struct sCollectorObject
{
	Ogre::String meshName;
	Ogre::String textureName;
	CModel* cmodel;
	//std::vector<sView> views;
	//unsigned int currentView;
};

class FormCollector : public FormBase
{
public:
	FormCollector(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener = 0);
	~FormCollector();
	
	// OIS::KeyListener
	bool keyPressed(const OIS::KeyEvent &arg);
	bool keyReleased(const OIS::KeyEvent &arg);

	// OIS::MouseListener
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	// GuiListener
	void buttonHit(Button* button);

	// FormBase
	void hide();
	void show();

	// FormCollector
	void showOptions();
	void hideOptions();
	void showOptionObject();
	void hideOptionObject();
	void showOptionsCamera();
	void hideOptionsCamera();

private:
	void _setCameraCollection();
	void _editModel(unsigned int index);
	void _removeObject(unsigned int index);
	void _resetObject(unsigned int index);
	bool _loadCollection(const Ogre::String& pathIni);
	bool _saveCollection(const Ogre::String& pathIni);

	GameInfo* mGameInfo;
	OgreBites::SdkCameraMan* mCameraMan;
	std::vector<sCollectorObject> mModels;
	sView mSceneView;
	Ogre::SceneNode* mTarget;
	CModelController* mController;
	Ogre::String mPathIni;
	unsigned int mCurrentIndex;
	bool mEditCamera;
};

#endif // #ifndef _FORMCOLLECTOR_H__

