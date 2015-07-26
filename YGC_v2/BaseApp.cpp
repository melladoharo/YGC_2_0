/* YGC_v2 Main
*/
#include "Stdafx.h"
#include "BaseApp.h"
#include "Ground.h"
#include "Background.h"
#include "ConfigReader.h"
#include "FormSelectorMain.h"
#include "FormOverview.h"
#include "FormReview.h"
#include "FormGames.h"
#include "FormOptions.h"
#include "FormImages.h"
#include "FormModels.h"
#include "FormMusic.h"
#include "FormVideos.h"
#include "FormCollector.h"
#include "GameInfo.h"
#include "Thumbnail3D.h"

//-------------------------------------------------------------------------------------
BaseApp::BaseApp()
	: mRoot(0), mCamera(0), mSceneMgr(0), mWindow(0),
	mResourcesCfg(Ogre::StringUtil::BLANK), mPluginsCfg(Ogre::StringUtil::BLANK),
	mShutDown(false),
	mInputManager(0), mMouse(0), mKeyboard(0), mTrayMgr(0), mGround(0), mBackground(0), mDofEffect(0),
	mFormSelector(0), mFormGames(0), mFormOverview(0), mFormReview(0), mFormImages(0), mFormModels(0),
	mFormMusic(0), mFormVideos(0), mFormCollector(0), mGameInfo(0), mFormOptions(0)
{
	srand(time(NULL));
}

//-------------------------------------------------------------------------------------
BaseApp::~BaseApp()
{
	if (mFormSelector) delete mFormSelector;
	if (mFormGames) delete mFormGames;
	if (mFormOptions) delete mFormOptions;
	if (mFormOverview) delete mFormOverview;
	if (mFormReview) delete mFormReview;
	if (mFormImages) delete mFormImages;
	if (mFormModels) delete mFormModels;
	if (mFormMusic) delete mFormMusic;
	if (mFormVideos) delete mFormVideos;
	if (mFormCollector) delete mFormCollector;
	if (mGameInfo) delete mGameInfo;
	if (mGround) delete mGround;
	if (mDofEffect) delete mDofEffect;
	if (mBackground) delete mBackground;
	if (mCameraMan) delete mCameraMan;
	if (mTrayMgr) delete mTrayMgr;

	//Remove ourself as a Window listener
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
	delete mRoot;
}

//-------------------------------------------------------------------------------------
bool BaseApp::go(void)
{
	// construct the Ogre::Root object
	mRoot = new Ogre::Root("", "", "YGC2.log");

	// list of required plugins
	Ogre::StringVector required_plugins;
	required_plugins.push_back("D3D9 RenderSystem");
	required_plugins.push_back("Cg Program Manager");
	// list of plugins to load
	Ogre::StringVector plugins_toLoad;
	plugins_toLoad.push_back("RenderSystem_Direct3D9");
	plugins_toLoad.push_back("Plugin_CgProgramManager");

	// load the Direct3d RenderSystem and the Octree SceneManager plugins
	for (Ogre::StringVector::iterator j = plugins_toLoad.begin(); j != plugins_toLoad.end(); ++j)
	{
#ifdef _DEBUG
		mRoot->loadPlugin(*j + Ogre::String("_d"));
#else
		mRoot->loadPlugin(*j);
#endif;
	}
	// Check if the required plugins are installed and ready for use [If not, exit the application]
	Ogre::Root::PluginInstanceList ip = mRoot->getInstalledPlugins();
	for (Ogre::StringVector::iterator j = required_plugins.begin(); j != required_plugins.end(); ++j)
	{
		bool found = false;
		// try to find the required plugin in the current installed plugins
		for (Ogre::Root::PluginInstanceList::iterator k = ip.begin(); k != ip.end(); ++k)
		{
			if ((*k)->getName() == *j)
			{
				found = true;
				break;
			}
		}
		if (!found)  // return false because a required plugin is not available
		{
			return false;
		}
	}

	/*--------------------------------------------------------------------------------------------------*/
	// Setup resources
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./media/fonts", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./media/gui", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./media/materials/programs", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./media/materials/scripts", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./media/materials/textures", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./media/models", "FileSystem", "General");

	/*--------------------------------------------------------------------------------------------------*/
	// Initialize render system config
	Ogre::RenderSystem* rs = mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem");
	if (!(rs->getName() == "Direct3D9 Rendering Subsystem"))
	{
		return false; //No RenderSystem found
	}

	// configure our RenderSystem
	rs->setConfigOption("Allow NVPerfHUD", "No");
	//rs->setConfigOption("FSAA", mConfig->fsaa);
	rs->setConfigOption("Fixed Pipeline Enabled", "Yes");
	rs->setConfigOption("Floating-point mode", "Fastest");
	rs->setConfigOption("Full Screen", ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM.GRAPHICS", "Fullscreen", "Yes"));
	rs->setConfigOption("Multi device memory hint", "Use minimun system memory");
	rs->setConfigOption("Resource Creation Policy", "Create on all devices");
	const Ogre::StringVector& rDevice = rs->getConfigOptions()["Rendering Device"].possibleValues;
	if (!rDevice.empty()) rs->setConfigOption("Rendering Device", rDevice.back());
	rs->setConfigOption("Use Multihead", "Auto");
	rs->setConfigOption("VSync", ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM.GRAPHICS", "VSync", "Yes"));
	rs->setConfigOption("VSync Interval", "1");
	rs->setConfigOption("Video Mode", ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM.GRAPHICS", "Resolution", ""));
	rs->setConfigOption("sRGB Gamma Conversion", "No");

	mRoot->setRenderSystem(rs);
	mWindow = mRoot->initialise(true, "YGC 2.0 - Your Games Collection");

	//-------------------------------------------------------------------------------------
	// Get the SceneManager, in this case a generic one
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "YGC2/SceneMgr");

	//-------------------------------------------------------------------------------------
	// create camera
	mCamera = mSceneMgr->createCamera("YGC2/Camera");

	mCamera->lookAt(Ogre::Vector3(0, 0, -1));
	mCamera->setNearClipDistance(1);
	mCamera->setFarClipDistance(1000);
	//mCamera->setPosition(0, 14, 0);
	//mCamera->setPosition(0, 26, -5); // Modo 1
	//mCamera->setPosition(-67, 26, -49); // Modo 2
	//mCamera->yaw(Ogre::Degree(-66)); // Modo 2
	//mCamera->pitch(Ogre::Degree(-16));
	mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller
	mCameraMan->setStyle(OgreBites::CS_ORBIT);
	//mCameraMan->setTopSpeed(8);
	
	/*
	mCameraMan->setStyle(OgreBites::CS_ORBIT);
	Ogre::SceneNode* nodoObjetivo=mSceneMgr->getRootSceneNode()->createChildSceneNode();
	nodoObjetivo->setPosition(0,13,0);
	mCameraMan->setTarget(nodoObjetivo);
	*/

	//-------------------------------------------------------------------------------------
	// create viewports
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	//-------------------------------------------------------------------------------------
	// Set default mipmap level (NB some APIs ignore this)
	//Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);
	//Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(2);
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
	Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(8);

	//-------------------------------------------------------------------------------------
	// Create any resource listeners (for loading screens)
	//createResourceListener();

	//-------------------------------------------------------------------------------------
	// load resources
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	Ogre::FontManager::getSingleton().getByName("YgcFont/SemiBold/21")->load();
	Ogre::FontManager::getSingleton().getByName("YgcFont/SemiBold/16")->load();
	Ogre::FontManager::getSingleton().getByName("YgcFont/Bold/22")->load();
	Ogre::FontManager::getSingleton().getByName("YgcFont/Regular/17")->load();
	Ogre::FontManager::getSingleton().getByName("YgcFont/Italic/16")->load();
	Ogre::FontManager::getSingleton().getByName("YgcFont/SemiboldItalic/16")->load();

	//-------------------------------------------------------------------------------------
	// Create the scene
	mDofEffect = new DOFManager(mCamera, mCamera->getViewport());
	mGround = new Ground;
	mBackground = new Background;
	
	// Set ambient light
	//mSceneMgr->setAmbientLight(Ogre::ColourValue(0.93f, 0.94f, 0.95f));
	//mSceneMgr->setAmbientLight(Ogre::ColourValue(0.85f, 0.85f, 0.87f));
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.73f, 0.74f, 0.75f));
	//mSceneMgr->setAmbientLight(Ogre::ColourValue(0,0,0));
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
	mSceneMgr->setShadowColour(Ogre::ColourValue(0.88f, 0.87f, 0.897f));
	mSceneMgr->setShadowFarDistance(200.0f);
	//mSceneMgr->setShadowTextureCount(1);
	//mSceneMgr->setShadowTextureSize(512);
		
	// Fixed light, dim
	Ogre::Light* mSunLight = mSceneMgr->createLight("SunLight");
	mSunLight->setType(Ogre::Light::LT_SPOTLIGHT);
	mSunLight->setPosition(250, 275, 230);
	mSunLight->setSpotlightRange(Ogre::Degree(30), Ogre::Degree(50));
	mSunLight->setCastShadows(true);
	Ogre::Vector3 dir;
	dir = -mSunLight->getPosition();
	dir.normalise();
	mSunLight->setDirection(dir);
	mSunLight->setDiffuseColour(0.42f, 0.42f, 0.49f);
	mSunLight->setSpecularColour(0.22f, 0.22f, 0.29f);
	Ogre::Viewport* shadowMapViewport = mSceneMgr->getShadowTexture(0)->getBuffer()->getRenderTarget()->getViewport(0);
	Ogre::CompositorManager::getSingleton().addCompositor(shadowMapViewport, "BlurTexture");
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(shadowMapViewport, "BlurTexture", true);

	/*
	// Create a light
	Ogre::Light* l = mSceneMgr->createLight("MainLight");
	l->setType(Ogre::Light::LT_POINT);
	l->setPosition(80, 150, 100);
	l->setCastShadows(true);
	l->setDiffuseColour(Ogre::ColourValue(0.33f, 0.34f, 0.35f));
	l->setSpecularColour(Ogre::ColourValue(0.23f, 0.24f, 0.25f));
	Ogre::Viewport* shadowMapViewport = mSceneMgr->getShadowTexture(0)->getBuffer()->getRenderTarget()->getViewport(0);
	Ogre::CompositorManager::getSingleton().addCompositor(shadowMapViewport, "BlurTexture");
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(shadowMapViewport, "BlurTexture", true);
	*/
	/*
	// Create a second light
	Ogre::Light* l2 = mSceneMgr->createLight("MainLight2");
	l2->setType(Ogre::Light::LT_POINT);
	l2->setPosition(-40, 340, -65);
	l2->setCastShadows(true);
	l2->setDiffuseColour(Ogre::ColourValue(0.33f, 0.34f, 0.35f));
	l2->setSpecularColour(Ogre::ColourValue(0.23f, 0.24f, 0.25f));
	Ogre::Viewport* shadowMapViewport2=mSceneMgr->getShadowTexture(1)->getBuffer()->getRenderTarget()->getViewport(0);
	Ogre::CompositorManager::getSingleton().addCompositor(shadowMapViewport2, "BlurTexture");
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(shadowMapViewport2, "BlurTexture", true);
	*/
	//nodoLight = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	//nodoLight->attachObject(l);
	
	mNodeCamera = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mNodeCamera->attachObject(mCamera);
	//mNodeCamera->setPosition(0, 26, -5);
	//mNodeCamera->pitch(Ogre::Degree(-16));

	//-------------------------------------------------------------------------------------
	//create FrameListener
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem(pl);

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	//Set initial mouse clipping size
	windowResized(mWindow);

	mTrayMgr = new GuiManager("YGC2/GuiManager", mWindow, mMouse, mKeyboard, this);
	mTrayMgr->setDofEffectManager(mDofEffect);
	mTrayMgr->setDofEffectEnable(false);
	mFormSelector = new FormSelectorMain(mTrayMgr, this);
	//mFormGames = new FormGames("D:/Programas/Juegos - Info DEBUG", mTrayMgr, this);
	//mFormOptions = new FormOptions(mTrayMgr, this);
	//mGameInfo = new GameInfo("D:/Programas/Juegos - Info DEBUG/The Witcher 2");
	//mGameInfo = new GameInfo("D:/Programas/Juegos - Info DEBUG/Dragon Age Inquisition");
	//mGameInfo = new GameInfo("D:/Programas/Juegos - Info DEBUG/Pruebas");
	//mGameInfo = new GameInfo("D:/Programas/Juegos - Info DEBUG/Mass Effect 2");
	//mGameInfo->createResourceGroup();
	//mTrayMgr->loadMenuBarGame();
	//mFormOverview = new FormOverview(mGameInfo, mTrayMgr, this);
	//mFormReview = new FormReview(mGameInfo, mTrayMgr, this);
	//mFormImages = new FormImages(mGameInfo, mTrayMgr, this);
	//mFormVideos = new FormVideos(mGameInfo, mTrayMgr);
	//mFormMusic = new FormMusic(mGameInfo, mTrayMgr);
	//mFormModels = new FormModels(mGameInfo, mTrayMgr, this);
	//mFormCollector = new FormCollector(mGameInfo, mTrayMgr, this);

	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	mRoot->addFrameListener(this);

	mRoot->startRendering();

	return true;
}

//-------------------------------------------------------------------------------------
bool BaseApp::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (mWindow->isClosed())
		return false;

	if (mShutDown || mTrayMgr->isShutDownEnabled())
		return false;

	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();

	mTrayMgr->frameRenderingQueued(evt);
	if( mCameraMan) mCameraMan->frameRenderingQueued(evt);

	// finish ygc 2.0
	if (mFormSelector && mFormSelector->isFinished())
	{
		delete mFormSelector;
		mFormSelector = 0;
		mShutDown = true;
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool BaseApp::keyPressed(const OIS::KeyEvent &arg)
{
	if (mCameraMan) mCameraMan->injectKeyDown(arg);
	if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
	{
		mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
	}
	else if (arg.key == OIS::KC_ESCAPE)
	{
		mShutDown = true;
	}
	
	return true;
}

//-------------------------------------------------------------------------------------
bool BaseApp::keyReleased(const OIS::KeyEvent &arg)
{
	if (mCameraMan) mCameraMan->injectKeyUp(arg);
	return true;
}

//-------------------------------------------------------------------------------------
bool BaseApp::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;
	if (mCameraMan) mCameraMan->injectMouseMove(arg);
	return true;
}

//-------------------------------------------------------------------------------------
bool BaseApp::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;
	if (mCameraMan) mCameraMan->injectMouseDown(arg, id);
	return true;
}

//-------------------------------------------------------------------------------------
bool BaseApp::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;
	if (mCameraMan) mCameraMan->injectMouseUp(arg, id);
	return true;
}

//-------------------------------------------------------------------------------------
// Adjust mouse clipping area
void BaseApp::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

//-------------------------------------------------------------------------------------
// Unattach OIS before window shutdown (very important under Linux)
void BaseApp::windowClosed(Ogre::RenderWindow* rw)
{
	//Only close for window that created OIS (the main window in these demos)
	if (rw == mWindow)
	{
		if (mInputManager)
		{
			mInputManager->destroyInputObject(mMouse);
			mInputManager->destroyInputObject(mKeyboard);

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}

void BaseApp::buttonHit(Button* button)
{
	if (button->getName() == "bttExit")
	{
		mShutDown = true;
	}
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
{
	BaseApp YGC_v2_0;

	try {
		YGC_v2_0.go();
	}
	catch (Ogre::Exception& e)
	{
		MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!",
			MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}
}

