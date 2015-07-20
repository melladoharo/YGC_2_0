/* Ground class [Transparent - Not visible]
*/

#ifndef _GROUND_H__
#define _GROUND_H__

class Ground
{
public:
	Ground();
	~Ground();
	
	Ogre::SceneNode* getNodo() const { return mNode; }

private:
	// Ogre scene manager
	Ogre::SceneManager* mSceneMgr;		// Default scene manager 
	
	// Ground (it works like a floor)
	Ogre::Plane* mPlane;				// Plane, Y axis
	const Ogre::String mNamePlane;		// Name plane
	Ogre::Entity* mEntity;				// Entity
	Ogre::MaterialPtr mMaterial;		// Material for plane 
	Ogre::SceneNode* mNode;				// Scene node
};

#endif // ifndef _GROUND_H__

