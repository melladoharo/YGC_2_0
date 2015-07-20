// "Depth of Field" demo for Ogre
// Copyright (C) 2006  Christian Lindequist Larsen
//
// This code is in the public domain. You may do whatever you want with it.

#include "Stdafx.h"
#include "DepthOfFieldEffect.h"
#include "Lens.h"

using namespace Ogre;

const int DepthOfFieldEffect::BLUR_DIVISOR = 4;

DepthOfFieldEffect::DepthOfFieldEffect(Ogre::Camera* cam, Ogre::Viewport* vp) 
	: mNearDepth(10.0)
	, mFocalDepth(100.0)
	, mFarDepth(190.0)
	, mFarBlurCutoff(1.0)
	, mCamera(cam)
	, mViewport(vp)
{
	mWidth = mViewport->getActualWidth();
	mHeight = mViewport->getActualHeight();

	mCompositor = NULL;
	mDepthTechnique = NULL;
	mDepthTarget = NULL;
	mDepthViewport = NULL;
	mDepthTexture.setNull();
	mDepthMaterial.setNull();

	createDepthRenderTexture();
	addCompositor();
}

DepthOfFieldEffect::~DepthOfFieldEffect()
{
	removeCompositor();
	destroyDepthRenderTexture();
}

void DepthOfFieldEffect::setFocalDepths(float nearDepth, float focalDepth, float farDepth)
{
	mNearDepth = nearDepth;
	mFocalDepth = focalDepth;
	mFarDepth = farDepth;
}

void DepthOfFieldEffect::setFarBlurCutoff(float cutoff)
{
	mFarBlurCutoff = cutoff;
}

bool DepthOfFieldEffect::getEnabled() const
{
	return mCompositor->getEnabled();
}

void DepthOfFieldEffect::setEnabled(bool value)
{
	mCompositor->setEnabled(value);
}

void DepthOfFieldEffect::createDepthRenderTexture()
{
	// Create the depth render texture
	mDepthTexture = TextureManager::getSingleton().createManual(
		"DoF_Depth",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		TEX_TYPE_2D, mWidth, mHeight,
		0, PF_L8, TU_RENDERTARGET);

	// Get its render target and add a viewport to it
	mDepthTarget = mDepthTexture->getBuffer()->getRenderTarget();
	mDepthViewport = mDepthTarget->addViewport(mCamera);

	// Register 'this' as a render target listener
	mDepthTarget->addListener(this);

	// Get the technique to use when rendering the depth render texture
	mDepthMaterial = MaterialManager::getSingleton().getByName("DoF_Depth");
	mDepthMaterial->load(); // needs to be loaded manually
	mDepthTechnique = mDepthMaterial->getBestTechnique();

	// Create a custom render queue invocation sequence for the depth render texture
    RenderQueueInvocationSequence* invocationSequence =
		Root::getSingleton().createRenderQueueInvocationSequence("DoF_Depth");

	// Add a render queue invocation to the sequence, and disable shadows for it
    RenderQueueInvocation* invocation = invocationSequence->add(RENDER_QUEUE_MAIN, "main");
    invocation->setSuppressShadows(true);

	// Set the render queue invocation sequence for the depth render texture viewport
    mDepthViewport->setRenderQueueInvocationSequenceName("DoF_Depth");

	//re-set texture "DoF_Depth"
	MaterialPtr p = MaterialManager::getSingleton().getByName("DoF_DepthOfField");
	p->load();
	p->getBestTechnique()->getPass(0)->getTextureUnitState("depth")->setTextureName("DoF_Depth");
	p->unload();
}

void DepthOfFieldEffect::destroyDepthRenderTexture()
{
	mDepthViewport->setRenderQueueInvocationSequenceName("");
	Root::getSingleton().destroyRenderQueueInvocationSequence("DoF_Depth");

	mDepthMaterial->unload();

	mDepthTarget->removeAllListeners();
	mDepthTarget->removeAllViewports();
	TextureManager::getSingleton().unload("DoF_Depth");
	TextureManager::getSingleton().remove("DoF_Depth");
}

void DepthOfFieldEffect::addCompositor()
{
	mCompositor = CompositorManager::getSingleton().addCompositor(mViewport, "DoF_Compositor_test");
	mCompositor->addListener(this);

	mCompositor->setEnabled(true);
}

void DepthOfFieldEffect::removeCompositor()
{
	mCompositor->setEnabled(false);

	mCompositor->removeListener(this);
	CompositorManager::getSingleton().removeCompositor(mViewport, "DoF_Compositor_test");
}

void DepthOfFieldEffect::notifyMaterialSetup(uint32 passId, MaterialPtr& material)
{
	switch (passId)
	{
	case BlurPass:
		{
			//float pixelSize[2] = {
			//	1.0f / (mViewport->getActualWidth() / BLUR_DIVISOR),
			//	1.0f / (mViewport->getActualHeight() / BLUR_DIVISOR)};

			// Adjust fragment program parameters
			Ogre::Vector3 ps = Ogre::Vector3(1.0f / (mWidth / BLUR_DIVISOR),1.0f / (mHeight / BLUR_DIVISOR), 1.0f);
			float pixelSize[3] = { ps.x, ps.y, ps.z };
			GpuProgramParametersSharedPtr fragParams = material->getBestTechnique()->getPass(0)->getFragmentProgramParameters();
			if((!fragParams.isNull())&&(fragParams->_findNamedConstantDefinition("pixelSize")))
				fragParams->setNamedConstant("pixelSize", pixelSize, 1, 3);

			break;
		}

	case OutputPass:
		{
			float pixelSizeScene[3] = {
				1.0f / mWidth,
				1.0f / mHeight,
				0};

			float pixelSizeBlur[3] = {
				1.0f / (mWidth / BLUR_DIVISOR),
				1.0f / (mHeight / BLUR_DIVISOR),
				0};

			// Adjust fragment program parameters
			GpuProgramParametersSharedPtr fragParams =
				material->getBestTechnique()->getPass(0)->getFragmentProgramParameters();
			if((!fragParams.isNull())&&(fragParams->_findNamedConstantDefinition("pixelSizeScene")))
				fragParams->setNamedConstant("pixelSizeScene", pixelSizeScene,1,3);
			if((!fragParams.isNull())&&(fragParams->_findNamedConstantDefinition("pixelSizeBlur")))
				fragParams->setNamedConstant("pixelSizeBlur", pixelSizeBlur,1,3);

			break;
		}
	}
}

void DepthOfFieldEffect::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
{

	float dofParams[4] = {mNearDepth, mFocalDepth, mFarDepth, mFarBlurCutoff};

	// Adjust fragment program parameters for depth pass
	GpuProgramParametersSharedPtr fragParams =
		mDepthTechnique->getPass(0)->getFragmentProgramParameters();
	if((!fragParams.isNull())&&(fragParams->_findNamedConstantDefinition("dofParams")))
		fragParams->setNamedConstant("dofParams", dofParams,1,4);		

	// Add 'this' as a RenderableListener to replace the technique for all renderables
	RenderQueue* queue = evt.source->getCamera()->getSceneManager()->getRenderQueue();
	queue->setRenderableListener(this);
}

void DepthOfFieldEffect::postViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
{
	// Reset the RenderableListener
	RenderQueue* queue = evt.source->getCamera()->getSceneManager()->getRenderQueue();
	queue->setRenderableListener(0);
}

bool DepthOfFieldEffect::renderableQueued(Ogre::Renderable* rend, Ogre::uint8 groupID, 
				Ogre::ushort priority, Ogre::Technique** ppTech, Ogre::RenderQueue* pQueue)
{
	// Replace the technique of all renderables
	*ppTech = mDepthTechnique;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
DOFManager::DOFManager(Ogre::Camera* cam, Ogre::Viewport* vp)
	:	mCamera(cam),
		mViewport(vp),
		mDepthOfFieldEffect(new DepthOfFieldEffect(mCamera, mViewport)),
		mLens(new Lens(mCamera->getFOVy(), 1.5f))
{
	setEnabled(true);
	setFocus(60.0f);

	Ogre::Root::getSingleton().addFrameListener(this);
	Ogre::LogManager::getSingleton().logMessage("DOF: Depth of field effect enable.");
}

DOFManager::~DOFManager()
{
	Ogre::Root::getSingleton().removeFrameListener(this);

	if(mLens) delete mLens;
	if(mDepthOfFieldEffect) delete mDepthOfFieldEffect;
	mLens=0;
	mDepthOfFieldEffect=0;

	Ogre::LogManager::getSingleton().logMessage("DOF: Depth of field effect disable.");
}

void DOFManager::setEnabled(bool enabled)
{
	mDepthOfFieldEffect->setEnabled(enabled);
}

bool DOFManager::getEnabled()
{
	return mDepthOfFieldEffect->getEnabled();
}

void DOFManager::zoomView(Ogre::Real delta)
{
	Real fieldOfView = mLens->getFieldOfView().valueRadians();
	fieldOfView += delta;
	fieldOfView = std::max<Real>(0.1f, std::min<Real>(fieldOfView, 2.0f));
	mLens->setFieldOfView(Radian(fieldOfView));
	mCamera->setFOVy(Radian(fieldOfView));
}

void DOFManager::Aperture(Ogre::Real delta)
{
	Real fStop = mLens->getFStop();
	fStop += delta;
	fStop = std::max<Real>(0.5, std::min<Real>(fStop, 12.0));
	mLens->setFStop(fStop);
}

void DOFManager::moveFocus(Ogre::Real delta)
{
	mLens->setFocalDistance(mLens->getFocalDistance() + delta);
}

void  DOFManager::setZoom(Ogre::Real f)
{
	Real fieldOfView = Degree(Real(f)).valueRadians();
	fieldOfView = std::max<Real>(0.1f, std::min<Real>(fieldOfView, 2.0));
	mLens->setFieldOfView(Radian(fieldOfView));
	mCamera->setFOVy(Radian(fieldOfView));
}

void  DOFManager::setAperture(Ogre::Real f)
{
	Real fStop = f;
	fStop = std::max<Real>(0.5, std::min<Real>(fStop, 12.0));
	mLens->setFStop(fStop);
}
Ogre::Real DOFManager::getAperture( void )
{
	if(mLens) mLens->getFStop();
	return 0.0f;
}


void  DOFManager::setFocus(Ogre::Real f)
{
	if(mLens) mLens->setFocalDistance(f);
}
Ogre::Real DOFManager::getFocus( void )
{
	if(mLens)	return mLens->getFocalDistance();
	return 0.0f;
}


bool DOFManager::frameStarted(const FrameEvent& evt)
{
	if(mDepthOfFieldEffect->getEnabled())
	{
		// Calculate and set depth of field using lens
		float nearDepth, focalDepth, farDepth;
		mLens->recalculateDepthOfField(nearDepth, focalDepth, farDepth);
		mDepthOfFieldEffect->setFocalDepths(nearDepth, focalDepth, farDepth);
	}
	
	return true;
}



