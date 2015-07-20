/* YGC_v2 Main 
*/
#ifndef _BASEAPP_H__
#define _BASEAPP_H__

#include "GuiManager.h"

class Ground;
class Background;
class FormGames;
class FormOverview;
class FormReview;
class FormImages;
class FormModels;
class FormMusic;
class FormVideos;
class FormCollector;
class GameInfo;
class FormSelectorMain;

class BaseApp : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener, public GuiListener
{
public:
    BaseApp();
    ~BaseApp();
    bool go();

private:
    // Ogre::FrameListener
    bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    // OIS::KeyListener
    bool keyPressed( const OIS::KeyEvent &arg );
    bool keyReleased( const OIS::KeyEvent &arg );

    // OIS::MouseListener
    bool mouseMoved( const OIS::MouseEvent &arg );
    bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

    // Ogre::WindowEventListener
    void windowResized(Ogre::RenderWindow* rw);
    void windowClosed(Ogre::RenderWindow* rw);

	// GuiListener
	void buttonHit(Button* button);

	// Ogre 
    Ogre::Root *mRoot;
    Ogre::Camera* mCamera;
    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderWindow* mWindow;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;
	OgreBites::SdkCameraMan* mCameraMan; // basic camera controller
    bool mShutDown;

    // OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*    mMouse;
    OIS::Keyboard* mKeyboard;

	// YGC 
	GuiManager* mTrayMgr;
	FormSelectorMain* mFormSelector;
	FormOverview* mFormOverview;
	FormReview* mFormReview;
	FormImages* mFormImages;
	FormGames* mFormGames;
	FormModels* mFormModels;
	FormMusic* mFormMusic;
	FormVideos* mFormVideos;
	FormCollector* mFormCollector;
	Ground* mGround;
	Background* mBackground;
	DOFManager* mDofEffect;
	Ogre::SceneNode* mNodeCamera;

	Ogre::Entity* entImg;
	Ogre::SceneNode* nodoImg, *nodoLight;
	GameInfo* mGameInfo;
};

#endif // #ifndef _BASEAPP_H__

