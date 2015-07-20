/*----------------------------------------------------------------------------------------------------
	created:	2012/10/15
	created:	15:10:2012   10:24
	filename: 	C:\Users\Meham\Documents\Visual Studio 2010\Projects\PCMC_01\PCMC_01\UtilsOgreDshow.cpp
	file path:	C:\Users\Meham\Documents\Visual Studio 2010\Projects\PCMC_01\PCMC_01
	file base:	UtilsOgreDshow
	file ext:	cpp
	author:		Manuel Mellado Haro
	purpose:	Reproduccion de video con Directshow (textura Ogre)
	Copyright(C) 2013 Manuel Mellado Haro. Todos los derechos reservados.
----------------------------------------------------------------------------------------------------*/

#include "Stdafx.h"
#include "UtilsOgreDshow.h"
#include "UtilsOgreDshow_private.h"

DirectShowMovieTexture::DirectShowMovieTexture()
{
    // Create dsdata
    dsdata=new DirectShowData;

	// Initialize all pointers
	dsdata->pGraph=0;
	dsdata->pControl=0;
	dsdata->pEvent=0;
	dsdata->pGrabberF=0;
	dsdata->pAudio=0;
	dsdata->pGrabber=0;
	dsdata->pSeeking=0;
	dsdata->pWindow=0;
	dsdata->videoWidth=0;
	dsdata->videoHeight=0;
	mTexWidth=0;
	mTexHeight=0;
}
 
DirectShowMovieTexture::~DirectShowMovieTexture()
{
    // Deinitialize directshow
    unloadMovie();
    CoUninitialize();
 
    // Destroy texture
	if(!mTexture.isNull())
	{
		Ogre::TextureManager::getSingleton().unload(mTexture->getName());
		Ogre::TextureManager::getSingleton().remove(mTexture->getName());
	}
 
    // Delete dsdata
    delete dsdata;
}
 
bool DirectShowMovieTexture::loadMovie(const Ogre::String& moviePath)
{
    HRESULT hr;
 
	// Initialize directshow
	hr=CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if(FAILED(hr)) {Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] Error in co initialize"); return false;}

    // log it!
    Ogre::LogManager::getSingletonPtr()->logMessage(Ogre::String("[DSHOW] Loading movie named '")+moviePath+"'.");
 
    // destroy previous movie objects (if any)
    unloadMovie();
 
    // create filter graph and get interfaces
    hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&dsdata->pGraph);
    if(FAILED(hr)) {Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] Error in creating graph."); return false;}

    hr=dsdata->pGraph->QueryInterface(IID_IMediaControl, (void**)&dsdata->pControl);
    if(FAILED(hr)) {Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] Error in querying media control"); return false;}
 
    hr=dsdata->pGraph->QueryInterface(IID_IMediaEvent, (void**)&dsdata->pEvent);
    if(FAILED(hr)) {Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] Error in querying media event"); return false;}
 
    hr=dsdata->pGraph->QueryInterface(IID_IMediaSeeking, (void**)&dsdata->pSeeking);
    if(FAILED(hr)) {Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] Error in querying seeking interface"); return false;}
 
	hr=dsdata->pGraph->QueryInterface(IID_IBasicAudio, (void**)&dsdata->pAudio);
	if(FAILED(hr)) {Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] Error in querying audio event"); return false;}
	
    // create sample grabber
    hr=CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&dsdata->pGrabberF);
    if(FAILED(hr)) {Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] Error in creating sample grabber"); return false;}
 
    // add sample grabber to the graph
    hr=dsdata->pGraph->AddFilter(dsdata->pGrabberF, L"Sample Grabber");
    if(FAILED(hr)) {Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] Error in adding sample grabber to the graph"); return false;}
 
    // get sample grabber object
    dsdata->pGrabberF->QueryInterface(IID_ISampleGrabber, (void**)&dsdata->pGrabber);
 
    // set sample grabber media type
    AM_MEDIA_TYPE mt;
    ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
    mt.majortype = MEDIATYPE_Video;
    mt.subtype = MEDIASUBTYPE_RGB24;
    mt.formattype = FORMAT_VideoInfo;
    hr=dsdata->pGrabber->SetMediaType(&mt);
    if(FAILED(hr)) {Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] Error in setting sample grabber media type"); return false;}
		
    // open the file
    WCHAR* filepath=util_convertCStringToWString(moviePath.c_str());
    hr=dsdata->pGraph->RenderFile(filepath, NULL);
    if(FAILED(hr)) {Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] Error opening video file!"); return false;}

    // disable auto show
    hr=dsdata->pGraph->QueryInterface(IID_IVideoWindow, (void**) & dsdata->pWindow);
    if(FAILED(hr)) {Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] Error getting video window interface"); return false;}
    dsdata->pWindow->put_AutoShow(OAFALSE);

    // get video information
    AM_MEDIA_TYPE mtt;
    hr=dsdata->pGrabber->GetConnectedMediaType(&mtt);
    if(FAILED(hr)) {Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] Error getting connected media type info"); return false;}
 
    VIDEOINFOHEADER *vih = (VIDEOINFOHEADER*) mtt.pbFormat;
    dsdata->videoWidth=vih->bmiHeader.biWidth;
    dsdata->videoHeight=vih->bmiHeader.biHeight;
    // microsoft's help version of free media type
    if (mtt.cbFormat != 0)
    {
        CoTaskMemFree((PVOID)mtt.pbFormat);
        mtt.cbFormat = 0;
        mtt.pbFormat = NULL;
    }
    if (mtt.pUnk != NULL)
    {
        mtt.pUnk->Release();
        mtt.pUnk = NULL;
    }
 
    // log it
    Ogre::LogManager::getSingletonPtr()->logMessage(
        Ogre::String("[DSHOW] -> This movie has dimensions: ")+
        Ogre::StringConverter::toString(dsdata->videoWidth)+"x"+
        Ogre::StringConverter::toString(dsdata->videoHeight)+".");
 
	mTexWidth=dsdata->videoWidth;
	mTexHeight=dsdata->videoHeight;

	// create the texture we are going to use
	mTexture=Ogre::TextureManager::getSingleton().createManual(
		"DirectShowManualTexture",	// name
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, // group
		Ogre::TEX_TYPE_2D,// texture type
		mTexWidth, // texture width
		mTexHeight,// texture hight
		0, // number of mipmaps
		Ogre::PF_BYTE_BGRA, // pixel format
		Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE // usage
		);

    // set sampling options
    dsdata->pGrabber->SetOneShot(FALSE);
    dsdata->pGrabber->SetBufferSamples(TRUE);
 
    // clean the texture, so that it's ready for rendering this video
    cleanTextureContents();

	return true;
}
 
Ogre::Vector2 DirectShowMovieTexture::getMovieDimensions()
{
    return Ogre::Vector2(dsdata->videoWidth, dsdata->videoHeight);
}
 
void DirectShowMovieTexture::unloadMovie()
{
    if (dsdata->pGraph==0)
        return;
 
    if (dsdata->pGrabber!=0)
    {
        dsdata->pGrabber->Release();
        dsdata->pGrabber=0;
    }
    if (dsdata->pGrabberF!=0)
    {
        dsdata->pGrabberF->Release();
        dsdata->pGrabberF=0;
    }
	if (dsdata->pAudio!=0)
	{
		dsdata->pAudio->Release();
		dsdata->pAudio=0;
	}
    if (dsdata->pWindow!=0)
    {
        dsdata->pWindow->Release();
        dsdata->pWindow=0;
    }
    if (dsdata->pSeeking!=0)
    {
        dsdata->pSeeking->Release();
        dsdata->pSeeking=0;
    }
    if (dsdata->pControl!=0)
    {
        dsdata->pControl->Release();
        dsdata->pControl=0;
    }
    if (dsdata->pEvent!=0)
    {
        dsdata->pEvent->Release();
        dsdata->pEvent=0;
    }
    if (dsdata->pGraph!=0)
    {
        dsdata->pGraph->Release();
        dsdata->pGraph=0;
    }
 
}
 
void DirectShowMovieTexture::pauseMovie()
{
    // pause!
    if (dsdata->pControl)
        dsdata->pControl->Pause();
}
 
void DirectShowMovieTexture::playMovie()
{
    // play!
    if (dsdata->pControl)
        dsdata->pControl->Run();
}
 
void DirectShowMovieTexture::rewindMovie()
{
    if (!dsdata->pSeeking) return;
 
    // rewind!
    LONGLONG p1=0;
    LONGLONG p2=0;
 
    dsdata->pSeeking->SetPositions(&p1, AM_SEEKING_AbsolutePositioning, &p2, AM_SEEKING_NoPositioning);
}

void DirectShowMovieTexture::forwardRandomMovie()
{
	if (!dsdata->pSeeking) return;

	// forward
	LONGLONG p1; 
	dsdata->pSeeking->GetDuration(&p1);
	p1/=1000000;
	LONGLONG p2=rand()%p1;
	p2*=1000000;

	dsdata->pSeeking->SetPositions(&p2, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
}

void DirectShowMovieTexture::forwardMovie(float pos)
{
	if (!dsdata->pSeeking) return;

	// forward
	LONGLONG p1=pos;
	p1*=1000000;

	dsdata->pSeeking->SetPositions(&p1, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
}

void DirectShowMovieTexture::stopMovie()
{
    // stop!
    if (dsdata->pControl)
        dsdata->pControl->Stop();
}
 
Ogre::TexturePtr DirectShowMovieTexture::getMovieTexture()
{
    return mTexture;
}

int DirectShowMovieTexture::getDurationMovie()
{
	if (!dsdata->pSeeking) return 0;

	LONGLONG p1; dsdata->pSeeking->GetDuration(&p1);
	p1=p1/1000000;

	return (int)p1;
}

int DirectShowMovieTexture::getActualTimeMovie()
{
	if (!dsdata->pSeeking) return 0;

	LONGLONG p1; dsdata->pSeeking->GetCurrentPosition(&p1);
	p1=p1/1000000;

	return (int)p1;
}

void DirectShowMovieTexture::setVolume( long vol )
{
	if(dsdata->pAudio)
	{
		dsdata->pAudio->put_Volume(vol);
	}
}

long DirectShowMovieTexture::getVolume( void )
{
	if(dsdata->pAudio)
	{
		long vol=0;
		dsdata->pAudio->get_Volume(&vol);
		return vol;
	}
	return 0;
}
 
bool DirectShowMovieTexture::updateMovieTexture()
{
    HRESULT hr;
    BYTE* bmpTmp;
 
    // only do this if there is a graph that has been set up
    if (!dsdata->pGraph)
        return false;
 
    // Find the required buffer size.
    long cbBuffer = 0;
    hr = dsdata->pGrabber->GetCurrentBuffer(&cbBuffer, NULL);
    if (cbBuffer<=0)
    {
        // nothing to do here yet
        return false;
    }
 
    char *pBuffer = new char[cbBuffer];
    if (!pBuffer) 
    {
        // out of memory!
		Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] Out of memory or empty buffer"); 
		return false;
    }
    hr = dsdata->pGrabber->GetCurrentBuffer(&cbBuffer, (long*)pBuffer);
    if (hr==E_INVALIDARG || hr==VFW_E_NOT_CONNECTED || hr==VFW_E_WRONG_STATE)
    {
        // we aren't buffering samples yet, do nothing
        delete[] pBuffer;
        return false;
    }
    if (FAILED(hr)) {Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] Failed at GetCurrentBuffer!"); return false;}
 
	// Texture ogre update
    // get the texture pixel buffer
    Ogre::HardwarePixelBufferSharedPtr pixelBuffer = mTexture->getBuffer();
    bmpTmp=(BYTE*)pBuffer;
 
    // lock the pixel buffer and get a pixel box
    pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
 
    Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);

	// go set all bits...
	int x=0, y=dsdata->videoHeight-1;
	unsigned int i, idx;
	
    for (i=0; i<((unsigned)dsdata->videoWidth*dsdata->videoHeight*3); i+=3)
	{
        idx=(x*4)+y*pixelBox.rowPitch*4;
 
        // paint
        pDest[idx]=bmpTmp[i];//b
        pDest[idx+1]=bmpTmp[i+1];//g
        pDest[idx+2]=bmpTmp[i+2];//r
        pDest[idx+3]=255;//a
 
		x++;
		if (x>=dsdata->videoWidth)
		{
			x=0;
			y--; if(y<0) y=0;
		}
    }
	
    // unlock the pixel buffer
    pixelBuffer->unlock();
	
    delete[] pBuffer;
	return true;
}
 
void DirectShowMovieTexture::cleanTextureContents()
{
    unsigned int idx;
    int x, y;
 
    // OGRE TEXTURE LOCK
    // get the texture pixel buffer
    int texw=mTexture->getWidth();
    int texh=mTexture->getHeight();
    Ogre::HardwarePixelBufferSharedPtr pixelBuffer = mTexture->getBuffer();
 
    // lock the pixel buffer and get a pixel box
    pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
 
    Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);
 
    // FILL!
    for (x=0, y=0; y<texh; ){
        idx=(x*4)+y*pixelBox.rowPitch*4;
 
        // paint
        pDest[idx]=0;//b
        pDest[idx+1]=0;//g
        pDest[idx+2]=0;//r
        pDest[idx+3]=255;//a
 
        x++;
        if (x>=texw)
        {
            x=0;
            y++;
        }
    }
 
    // UNLOCK EVERYTHING!
    // unlock the pixel buffer
    pixelBuffer->unlock();
    // OGRE END
}
 
bool DirectShowMovieTexture::isPlayingMovie()
{
    OAFilterState pfs;
    HRESULT hr;
 
    if (dsdata->pEvent!=NULL){
        long ev, p1, p2;
 
        while (E_ABORT!=dsdata->pEvent->GetEvent(&ev, &p1, &p2, 0)){
            // check for completion
            if (ev==EC_COMPLETE)
            {
                pauseMovie();
                return false;
            }
 
            // release event params
            hr=dsdata->pEvent->FreeEventParams(ev, p1, p2);
            if (FAILED(hr))
            {
                pauseMovie();
                return false;
            }
        }
    }
 
    // get the running state!
    if (dsdata->pControl!=NULL)
    {
        hr=dsdata->pControl->GetState(0, &pfs);
        if (FAILED(hr))
        {
            pauseMovie();
            return false;
        }
 
        return pfs==State_Running;
    }
 
    // it hasn't even been initialized!
    return false;
}

bool DirectShowMovieTexture::isPausingMovie()
{
	OAFilterState pfs;
	HRESULT hr;

	if (dsdata->pEvent!=NULL)
	{
		long ev, p1, p2;

		while (E_ABORT!=dsdata->pEvent->GetEvent(&ev, &p1, &p2, 0))
		{
			// check for completion
			if (ev==EC_COMPLETE)
			{
				pauseMovie();
				return false;
			}

			// release event params
			hr=dsdata->pEvent->FreeEventParams(ev, p1, p2);
			if (FAILED(hr))
			{
				pauseMovie();
				return false;
			}
		}
	}

	// get the running state!
	if (dsdata->pControl!=NULL)
	{
		hr=dsdata->pControl->GetState(0, &pfs);
		if (FAILED(hr))
		{
			pauseMovie();
			return false;
		}

		return pfs==State_Paused;
	}

	// it hasn't even been initialized!
	return false;
}

bool DirectShowMovieTexture::isStopingMovie()
{
	OAFilterState pfs;
	HRESULT hr;

	if (dsdata->pEvent!=NULL)
	{
		long ev, p1, p2;

		while (E_ABORT!=dsdata->pEvent->GetEvent(&ev, &p1, &p2, 0))
		{
			// check for completion
			if (ev==EC_COMPLETE)
			{
				pauseMovie();
				return true;
			}

			// release event params
			hr=dsdata->pEvent->FreeEventParams(ev, p1, p2);
			if (FAILED(hr))
			{
				pauseMovie();
				return true;
			}
		}
	}

	// get the running state!
	if (dsdata->pControl!=NULL)
	{
		hr=dsdata->pControl->GetState(0, &pfs);
		if (FAILED(hr))
		{
			pauseMovie();
			return false;
		}

		return pfs==State_Stopped;
	}

	// it hasn't even been initialized!
	return false;
}


WCHAR* util_convertCStringToWString(const char* string)
{
    const int MAX_STRINGZ=500;
    static WCHAR wtext[MAX_STRINGZ+2];
 
    if (strlen(string)>MAX_STRINGZ)
        return 0;
 
    // convert text to wchar
    if (MultiByteToWideChar(
        CP_ACP,// ansi code page
        0,// flags
        string,// orig string
        -1,// calculate len
        wtext,// where to put the string
        MAX_STRINGZ)// maximum allowed path
        ==0)
    {
		Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] convertCStringToWString failed with no extra error info");
    }
 
    return wtext;
}







DirectShowSound::DirectShowSound(LPSTR szFile)
	:	pif(0),
		pigb(0),
		pimc(0),
		pimex(0),
		piba(0),
		pims(0),
		mPaused(false), mPlaying(false), mStopped(false),
		ready(false),
		dirAudio(szFile)
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	WCHAR wFile[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, szFile, -1, wFile, MAX_PATH);

	// log it!
	Ogre::LogManager::getSingletonPtr()->logMessage(Ogre::String("[DSHOW] Loading audio named '")+szFile+"'.");

	if (SUCCEEDED(CoCreateInstance(CLSID_FilterGraph, NULL,CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&this->pigb)))
	{
		pigb->QueryInterface(IID_IMediaControl, (void **)&pimc);
		pigb->QueryInterface(IID_IMediaEventEx, (void **)&pimex);
		pigb->QueryInterface(IID_IBasicAudio,   (void **)&piba);
		pigb->QueryInterface(IID_IMediaSeeking, (void **)&pims);

		if (SUCCEEDED(pigb->RenderFile(wFile, NULL)))
		{
			Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] -> Audio Loaded.");
			ready=true;
		}
	}
	else
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] -> Audio Error.");
	}
}

DirectShowSound::~DirectShowSound()
{
	if(pimc)	{pimc->Stop(); pimc->Release(); pimc=NULL;}
	if(pif)		{pif->Release();  pif=NULL;}
	if(pigb)	{pigb->Release(); pigb=NULL;}
	if(pimc)	{pimc->Release(); pimc=NULL;}
	if(pimex)	{pimex->Release();pimex=NULL;}
	if(piba)	{piba->Release(); piba=NULL;}
	if(pims)	{pims->Release(); pims=NULL;}
	CoUninitialize();

	Ogre::LogManager::getSingletonPtr()->logMessage("[DSHOW] -> Audio Unloaded.");
}


void DirectShowSound::Play()
{
	if(ready && pimc)
	{
		pimc->Run();
		mPlaying = true;
		mPaused = mStopped = false;
	}
}

void DirectShowSound::Pause()
{
	if(ready && pimc)
	{
		pimc->Pause();
		mPaused = true;
		mPlaying = mStopped = false;
	}
}

void DirectShowSound::Stop()
{
	if(ready && pimc)
	{
		pimc->Stop();
		mStopped = true;
		mPlaying = mPaused = false;
	}
}

// The allowed values are -10000 to 0. The loudest value is "0". -10000 is silence.
// This value will be 100 times of dB. For example, -5000 will be -50dB.
void DirectShowSound::setVolume( long vol )
{
	if(ready && piba)
	{
		piba->put_Volume(vol);
	}		
}
long DirectShowSound::getVolume( void )
{
	long vol=0;
	if(ready && piba)
	{
		piba->get_Volume(&vol);
	}
	
	return vol;
}


int DirectShowSound::getDuration()
{
	if(ready && pims)
	{
		LONGLONG p1=0;
		pims->GetDuration(&p1);
		p1=p1/10000000;

		return (int)p1;
	}

	return 0;
}

int DirectShowSound::getActualTime()
{
	if(ready && pims)
	{
		LONGLONG p1=0;
		pims->GetCurrentPosition(&p1);
		p1=p1/10000000;

		return (int)p1;
	}
	return 0;
}

void DirectShowSound::forwardTime( long pos )
{
	if (ready && pims)
	{
		LONGLONG p1=pos;
		p1*=10000000;

		pims->SetPositions(&p1, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
	}
}

bool DirectShowSound::endSound()
{
	if(ready && pimex)
	{
		long evCode = 0;
		LONG_PTR param1 = 0, param2 = 0;
		pimex->GetEvent(&evCode, &param1, &param2, 0);
		if(evCode==EC_COMPLETE) return true; return false;
	}
	return false;
}



