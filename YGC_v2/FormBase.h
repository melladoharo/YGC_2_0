#ifndef _FORMBASE_H__
#define _FORMBASE_H__

#include "GuiManager.h"
#include "CModel.h"

struct sView
{
	Ogre::Vector3 cameraPos;
	Ogre::Quaternion cameraRot;
	Ogre::Vector3 targetPos;
	bool dofEnable;
	Ogre::Real dofFocus;
};

/*=============================================================================
| Listener class for responding to tray events.
=============================================================================*/
class FormBase : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener, public GuiListener
{
public:
	FormBase(GuiManager* tray, GuiListener* oldListener = 0);
	virtual ~FormBase();
	
	// Ogre::FrameListener
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	// OIS::KeyListener
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);

	// OIS::MouseListener
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	
	// Form Base
	virtual void hide();
	virtual void show();
	virtual void showOptions() = 0;
	void enableForm();
	void disableForm();
	void addWidgetToForm(Widget* w);
	void removeWidgetFromForm(Widget* w);
	void createAnimNode(const Ogre::String& nameAnim, Ogre::Real timeAnim, Ogre::SceneNode* node,
		const Ogre::Vector3& posOri, const Ogre::Quaternion& quaOri,
		const Ogre::Vector3& posDest, const Ogre::Quaternion& quaDest);
	void destroyAnim(const Ogre::String& nameAnim);
	void resetCamera();

	// Transform "\n" (size of 2) in '\n' (only one character)
	static void _correctCaption(Ogre::String& caption);
	static void _loadModelStatusFromIni(CSimpleIniA* simpleIni, const Ogre::String& sectionKey, CModel* model);
	static bool _saveModelStatusFromIni(CSimpleIniA* simpleIni, const Ogre::String& sectionKey, CModel* model, const Ogre::String& pathIni);

protected:
	Ogre::SceneManager* mSceneMgr; // default scene manager 
	Ogre::Camera* mCamera; // default scene camera
	Ogre::SceneNode* mNodeCamera;
	GuiManager* mTrayMgr;
	GuiListener* mOldListener;
	std::vector<Widget*> mWidgetsForm;
	DecorWidget* mIconSettings;
	Ogre::RaySceneQuery* mRaySceneQuery;
	Ogre::Vector2 mScreenSize;
	std::vector<Ogre::AnimationState*> mAnimState; // animation state for animations
	Ogre::Real mTimeBase, mTimeDoubleClick; // timers base form
	bool mLBPressed, mRBPressed, mMBPressed, mDoubleClick, mDragging;  // different mouse states
};

#endif // #ifndef _FORMBASE_H__

