/* Background class
*/

#ifndef _BACKGROUND_H__
#define _BACKGROUND_H__

class Background
{
public:
	Background();
	~Background();

private:
	// Ogre scene manager
	Ogre::SceneManager* mSceneMgr;		// Default scene manager 

	// 2d rectangle (it works like 2d background)
	Ogre::MaterialPtr mMaterial;		// Material
	Ogre::Rectangle2D* mRect;			// 2d rectangle
	Ogre::SceneNode* mNode;				// Scene node for rectangle
};

#endif // ifndef _BACKGROUND_H__

