// "Depth of Field" demo for Ogre
// Copyright (C) 2006  Christian Lindequist Larsen
//
// This code is in the public domain. You may do whatever you want with it.

#ifndef __DepthOfFieldEffect_H__
#define __DepthOfFieldEffect_H__

#include "Stdafx.h"

class Lens;
class DepthOfFieldEffect : public Ogre::CompositorInstance::Listener,
						   public Ogre::RenderTargetListener,
						   public Ogre::RenderQueue::RenderableListener
{
public:
	DepthOfFieldEffect(Ogre::Camera* cam, Ogre::Viewport* vp);
	~DepthOfFieldEffect();

	float getNearDepth() const {return mNearDepth; }
	float getFocalDepth() const {return mFocalDepth; }
	float getFarDepth() const {return mFarDepth; }
	void setFocalDepths(float nearDepth, float focalDepth, float farDepth);
	float getFarBlurCutoff() const {return mFarBlurCutoff; }
	void setFarBlurCutoff(float cutoff);
	bool getEnabled() const;
	void setEnabled(bool value);

private:
	// Implementation of Ogre::CompositorInstance::Listener
	virtual void notifyMaterialSetup(Ogre::uint32 passId, Ogre::MaterialPtr& material);

	// Implementation of Ogre::RenderTargetListener
	virtual void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);
	virtual void postViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);

	// Implementation of Ogre::RenderQueue::RenderableListener
	virtual bool renderableQueued(Ogre::Renderable* rend, Ogre::uint8 groupID, 
				Ogre::ushort priority, Ogre::Technique** ppTech, Ogre::RenderQueue* pQueue);

	int mWidth;
	int mHeight;

	static const int BLUR_DIVISOR;

	enum PassId
	{
		BlurPass,
		OutputPass
	};

	Ogre::Viewport* mDepthViewport;
	Ogre::RenderTexture* mDepthTarget;
	Ogre::TexturePtr mDepthTexture;
	Ogre::MaterialPtr mDepthMaterial;
	Ogre::Technique* mDepthTechnique;
	Ogre::CompositorInstance* mCompositor;
	Ogre::Camera* mCamera;
	Ogre::Viewport* mViewport;
	float mNearDepth;
	float mFocalDepth;
	float mFarDepth;
	float mFarBlurCutoff;

	void createDepthRenderTexture();
	void destroyDepthRenderTexture();
	void addCompositor();
	void removeCompositor();
};

class DOFManager : public Ogre::FrameListener
{
public:
	DOFManager(Ogre::Camera* cam, Ogre::Viewport* vp);
	~DOFManager();

	void setEnabled(bool enabled);
	bool getEnabled();

	// controls
	void zoomView(Ogre::Real delta);
	void Aperture(Ogre::Real delta);
	void moveFocus(Ogre::Real delta);
	void setZoom(Ogre::Real f);
	void setAperture(Ogre::Real f);
	Ogre::Real getAperture(void);
	void setFocus(Ogre::Real f);
	Ogre::Real getFocus(void);

protected:
	virtual bool frameStarted(const Ogre::FrameEvent& evt);

	Ogre::Camera* mCamera;
	Ogre::Viewport* mViewport;
	DepthOfFieldEffect* mDepthOfFieldEffect;
	Lens* mLens;
};

#endif // __DepthOfFieldEffect_H__



