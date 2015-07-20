#ifndef _CMODELCONTROLLER_H__
#define _CMODELCONTROLLER_H__

class CModelController
{
public:
	CModelController(Ogre::SceneNode* node, Ogre::Real acceleration = 1.0f);
	~CModelController();

	void injectKeyDown(const OIS::KeyEvent &arg);
	void injectKeyUp(const OIS::KeyEvent &arg);
	void injectMouseMove(const OIS::MouseEvent &arg);
	void injectMouseDown(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	void injectMouseUp(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

private:
	void _createLine(const Ogre::Vector3& position, const Ogre::Vector3 axis, const Ogre::ColourValue& color);
	void _removeLine();

	Ogre::SceneManager* mSceneMgr; // default scene manager 
	Ogre::SceneNode* mNode;
	Ogre::Entity* mEntSphere;
	Ogre::Vector3 mActionAxis;
	Ogre::Real mAccel;
	bool mTranslate, mRotate, mScale, mDragging;
};

#endif // #ifndef _CMODELCONTROLLER_H__

