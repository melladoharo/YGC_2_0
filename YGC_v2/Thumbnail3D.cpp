#include "stdafx.h"
#include "Thumbnail3D.h"

Thumbnail3D::Thumbnail3D(Ogre::SceneNode* rootNode, const Ogre::String& name, const Ogre::String& nameGroupRes,
	const Ogre::String& caption, const Ogre::String& textName) : 
mSceneMgr(Ogre::Root::getSingletonPtr()->getSceneManager("YGC2/SceneMgr")),
mCamera(mSceneMgr->getCamera("YGC2/Camera")),
mNodeThumb(0), mEntThumb(0), matThumb(0),
mThumbWidget(0), mThumbLayer(0), mElement(0),
mName(name), mCaption(caption), mTextName(textName), mNameGroup(nameGroupRes),
mIndex(0),
mState(BS_UP),
mVisible(true)
{
	// new 3d plane [from mesh_thumb.mesh] for thumbnail
	mEntThumb = mSceneMgr->createEntity("Mesh/Thumbnail3D/" + mName, "mesh_thumb.mesh");
	mEntThumb->setCastShadows(false);
	mEntThumb->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);
	mNodeThumb = rootNode->createChildSceneNode();
	mNodeThumb->attachObject(mEntThumb);
	mNodeThumb->setPosition(0, 0, 0);
	mNodeThumb->pitch(Ogre::Degree(90));

	// new material with texture from images
	matThumb = Ogre::MaterialManager::getSingleton().create("Mat/YgcGui/Thumbnail3D/" + mName, nameGroupRes);
	matThumb->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
	matThumb->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	matThumb->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	matThumb->getTechnique(0)->getPass(0)->createTextureUnitState(mTextName);
	mEntThumb->getSubEntity(1)->setMaterialName(matThumb->getName());

	// resize thumb to correct aspect ratio
	_resizeThumb(Ogre::Vector3(2.7f, 2.7f, 2.7f));
}

Thumbnail3D::~Thumbnail3D()
{
	_removeThumbWidget();
	Ogre::MaterialManager::getSingleton().unload(matThumb->getName());
	Ogre::MaterialManager::getSingleton().remove(matThumb->getName());
	mNodeThumb->detachAllObjects();
	mSceneMgr->destroyEntity(mEntThumb);
	mSceneMgr->destroySceneNode(mNodeThumb);
}



void Thumbnail3D::setMouseOver()
{
	if (mState != BS_OVER)
	{
		mState = BS_OVER;

		// if thumbnail has a caption, then show it
		if (mCaption != Ogre::StringUtil::BLANK)
		{
			// calculate the size of 3d_thumbnail to build a 2d_thumbnail with the correct dimension
			mNodeThumb->_update(true, true);
			Ogre::AxisAlignedBox aaBoxThumb(Ogre::AxisAlignedBox(mNodeThumb->_getWorldAABB()));
			Ogre::Vector3 cornerLeftTop = aaBoxThumb.getCorner(Ogre::AxisAlignedBox::CornerEnum::NEAR_LEFT_TOP);
			Ogre::Vector3 cornerLeftBottom = aaBoxThumb.getCorner(Ogre::AxisAlignedBox::CornerEnum::NEAR_LEFT_BOTTOM);
			Ogre::Vector3 cornerRightTop = aaBoxThumb.getCorner(Ogre::AxisAlignedBox::CornerEnum::NEAR_RIGHT_TOP);
			Ogre::Vector3 cornerRightBottom = aaBoxThumb.getCorner(Ogre::AxisAlignedBox::CornerEnum::NEAR_RIGHT_BOTTOM);
			Ogre::Real textAreaTop = _getScreenPosition(cornerLeftTop).y;
			Ogre::Real textAreaLeft = _getScreenPosition(cornerLeftTop).x + 1;
			Ogre::Real textAreaWidth = _getScreenPosition(cornerRightBottom).x -_getScreenPosition(cornerLeftBottom).x;
			Ogre::Real textAreaHeight = _getScreenPosition(cornerRightBottom).y - _getScreenPosition(cornerRightTop).y;

			// creates a 2d_thumbnail over the 3d_thumbnail
			mThumbWidget = _createThumbWidget(textAreaLeft, textAreaTop, textAreaWidth, textAreaHeight);
			mThumbWidget->showCaption();
		}
	}
}

void Thumbnail3D::setMouseUp()
{
	if (mState != BS_UP)
	{
		_removeThumbWidget();
		mState = BS_UP;
	}
}



void Thumbnail3D::setThumbs3DInGrid(std::vector<Thumbnail3D*>& thumbs, const sGridThumbs& gridThumbs)
{
	/*
	Ogre::Real aspectRatio = 
		(Ogre::Real) Ogre::Root::getSingleton().getAutoCreatedWindow()->getViewport(0)->getActualWidth() /
		(Ogre::Real) Ogre::Root::getSingleton().getAutoCreatedWindow()->getViewport(0)->getActualHeight();
	
	Ogre::Real left = 0;
	if (aspectRatio == 16.0f / 10.0f) left = 19;
	else if (aspectRatio == 16.0f / 9.0f) left = 22;
	else left = 21;
	*/

	Ogre::Vector3 posThumb(-gridThumbs.left, gridThumbs.top, -40);
	unsigned int currentRow = 0;

	for (unsigned int i = 0; i < thumbs.size(); ++i)
	{
		thumbs[i]->setPosition(posThumb);
		posThumb.y -= gridThumbs.verticalSep;
		currentRow++;

		if (currentRow >= gridThumbs.rows)
		{
			posThumb.y = gridThumbs.top;
			posThumb.x += gridThumbs.horizontalSep;
			currentRow = 0;
		}
	}
}

Thumbnail3D* Thumbnail3D::getThumbnail3dOver(std::vector<Thumbnail3D*>& thumbs, const Ogre::String& nameSelected)
{
	Thumbnail3D* lastThumbOver = 0;

	for (unsigned int i = 0; i < thumbs.size(); ++i)
	{
		// change thumbnail over
		if (thumbs[i]->isMouseOver())
		{
			lastThumbOver = thumbs[i];
			if (nameSelected != thumbs[i]->getName())
			{
				thumbs[i]->setMouseUp();
			}
		}
		// new thumbnail over?
		else if (nameSelected == thumbs[i]->getName())
		{
			if (!thumbs[i]->isMouseOver())
			{
				thumbs[i]->setMouseOver();
				lastThumbOver = thumbs[i];
			}
		}
	}

	return lastThumbOver;
}



void Thumbnail3D::_resizeThumb(const Ogre::Vector3& scl)
{
	mNodeThumb->setScale(scl);
	Ogre::Real sizeTexX = matThumb->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureDimensions().first;
	Ogre::Real sizeTexY = matThumb->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureDimensions().second;
	Ogre::Vector3 sclFactor(mNodeThumb->getScale());
	Ogre::Real ratio = sizeTexX / sizeTexY;
	sclFactor.x *= ratio;
	mNodeThumb->setScale(sclFactor);
}

Ogre::Vector2 Thumbnail3D::_getScreenPosition(Ogre::Vector3 position)
{
	Ogre::Real screenX = Ogre::Root::getSingleton().getAutoCreatedWindow()->getViewport(0)->getActualWidth();
	Ogre::Real screenY = Ogre::Root::getSingleton().getAutoCreatedWindow()->getViewport(0)->getActualHeight();
	Ogre::Vector3 hcsPosition = mCamera->getProjectionMatrix() * mCamera->getViewMatrix() * position;
	Ogre::Vector2 screenPos;
	screenPos.x = screenX / 2.0f;
	screenPos.y = screenY / 2.0f;
	screenPos.x = screenPos.x - (screenPos.x * -hcsPosition.x);
	screenPos.y = screenPos.y - (screenPos.y * hcsPosition.y);
	return screenPos;
}

Thumbnail* Thumbnail3D::_createThumbWidget(Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real height)
{
	_removeThumbWidget();

	mThumbLayer = Ogre::OverlayManager::getSingleton().create("/GUImanager/Thumblayer3D/" + mName);
	mElement = (Ogre::OverlayContainer *)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "Thumb/Panel/" + mName);
	mThumbWidget = new Thumbnail("Thumbnail/Widget" + mName, mNameGroup, mCaption, mTextName, height);
	mThumbWidget->setLeft(left);
	mThumbWidget->setTop(top);
	mElement->addChild(mThumbWidget->getOverlayElement());
	mThumbLayer->add2D(mElement);
	mThumbLayer->setZOrder(390);
	mThumbLayer->show();

	return mThumbWidget;
}

void Thumbnail3D::_removeThumbWidget()
{
	if (Ogre::OverlayManager::getSingleton().getByName("/GUImanager/Thumblayer3D/" + mName))
	{
		Widget::nukeOverlayElement(mElement);
		mElement = 0;
		Ogre::OverlayManager::getSingleton().destroy(mThumbLayer);
		mThumbLayer = 0;
		if (mThumbWidget) delete mThumbWidget;
		mThumbWidget = 0;
	}
}

