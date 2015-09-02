#ifndef _THUMBNAIL3D_H__
#define _THUMBNAIL3D_H__

#include "Thumbnail.h"

struct sGridThumbs
{
	unsigned int rows;
	Ogre::Real size;
	Ogre::Real left;
	Ogre::Real top;
	Ogre::Real horizontalSep;
	Ogre::Real verticalSep;
};

class Thumbnail3D
{
public:
	Thumbnail3D(Ogre::SceneNode* rootNode, const Ogre::String& name, const Ogre::String& nameGroupRes, 
		const Ogre::String& caption, const Ogre::String& textName);
	~Thumbnail3D();

	bool isVisible() { return mVisible; }
	void show() { mNodeThumb->setVisible(true); mNodeThumbBack->setVisible(true); mVisible = true; }
	void hide() { mNodeThumb->setVisible(false); mNodeThumbBack->setVisible(false); mVisible = false; }
	bool isMouseOver() { return (mState == BS_OVER); }
	void setMouseOver();
	void setMouseUp();
	void setIndex(unsigned int newIndex) { mIndex = newIndex; }
	unsigned int getIndex() { return mIndex; }
	void setPosition(const Ogre::Vector3& newPos) { mNodeThumb->setPosition(newPos); mNodeThumbBack->setPosition(newPos); }
	Ogre::Vector3 getPosition() { return mNodeThumb->getPosition(); }
	void setScale(Ogre::Vector3 newScl) { _resizeThumb(newScl); }
	void setMaterialWidget(Ogre::String nameMaterial) { mThumbWidget->setMaterial(nameMaterial); }
	Ogre::String getName() { return (mEntThumb) ? mEntThumb->getName() : Ogre::StringUtil::BLANK; }

	static Thumbnail3D* getThumbnail3dOver(std::vector<Thumbnail3D*>& thumbs, const Ogre::String& nameSelected);
	static void setThumbs3DInGrid(std::vector<Thumbnail3D*>& thumbs, const sGridThumbs& gridThumbs);

private:
	void _resizeThumb(const Ogre::Vector3& scl);
	Ogre::Vector2 _getScreenPosition(Ogre::Vector3 position);
	Thumbnail* _createThumbWidget(Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real height);
	void _removeThumbWidget();

	Ogre::SceneManager* mSceneMgr;				// default scene manager 
	Ogre::Camera* mCamera;						// default scene camera
	Ogre::SceneNode* mNodeThumb;				// scene node for 3d rectangle model
	Ogre::SceneNode* mNodeThumbBack;
	Ogre::Entity* mEntThumb;					// entity for thumbnail
	Ogre::Entity* mEntThumbBack;
	Ogre::MaterialPtr matThumb;					// mat fot thumbnail
	Thumbnail* mThumbWidget;					// thumbnail mouse over 
	Ogre::Overlay* mThumbLayer;					// overlay for thumbnail widget [mthumbwidget]
	Ogre::OverlayContainer* mElement;			// overlay container for thumbnailwidget
	Ogre::String mName, mCaption, 
		mTextName, mNameGroup;					// properties of thumbnail
	ButtonState mState;							// state of thumbnail, over, down, ...
	unsigned int mIndex;						// in std::vector<Thumbnail3D> indicates the current thumb selected
	bool mVisible;								// visibility flag
};

#endif

