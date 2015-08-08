#ifndef _FORMCOLLECTOR_H__
#define _FORMCOLLECTOR_H__

#include "FormBase.h"
#include "GameInfo.h"
#include "CModel.h"
#include "ModelController.h"

struct sCollectorObject
{
	CModel* cmodel;
	Ogre::String meshName;
	Ogre::StringVector textureName;
	bool isYgcModel;
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
	void labelHit(Label* label);
	void itemChanged(ItemSelector* selector);

	// FormBase
	void hide();
	void show();
	void hideAllOptions();
	void hideOptions();
	void showOptions();
	void hideOptionsItems();
	void showOptionsItems();
	void hideOptionsCamera();
	void showOptionsCamera();
	void hideOptionEditPosition();
	void showOptionEditPosition();

private:
	void _resetCameraCollection();
	void _setCameraCollection();
	void _newItem();
	void _removeItem(unsigned int index);
	void _editItem(unsigned int index);
	void _resetItem(unsigned int index);
	void _loadConfigWindowsItem(unsigned int currentModel, bool newModel = false);
	bool _loadCollection(const Ogre::String& pathIni);
	bool _saveCollection(const Ogre::String& pathIni);
	Ogre::String _getNameResText(const Ogre::String& nameText);

	GameInfo* mGameInfo;
	OgreBites::SdkCameraMan* mCameraMan;
	std::vector<sInfoResource> mTextures;
	std::vector<sInfoResource> mZipModels;
	Ogre::StringVector mYgcModels;
	std::vector<sCollectorObject> mModels;
	sView mSceneView;
	Ogre::SceneNode* mTarget;
	CModelController* mController;
	Ogre::String mPathIni;
	unsigned int mCurrentIndex;
	bool mCtrlPressed, mAltPressed, mShiftPressed;
};

#endif // #ifndef _FORMCOLLECTOR_H__

