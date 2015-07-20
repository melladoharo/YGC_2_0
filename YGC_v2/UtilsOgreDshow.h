/*----------------------------------------------------------------------------------------------------
	created:	2012/10/15
	created:	15:10:2012   10:23
	filename: 	C:\Users\Meham\Documents\Visual Studio 2010\Projects\PCMC_01\PCMC_01\UtilsOgreDshow.h
	file path:	C:\Users\Meham\Documents\Visual Studio 2010\Projects\PCMC_01\PCMC_01
	file base:	UtilsOgreDshow
	file ext:	h
	author:		Manuel Mellado Haro
	purpose:	Reproduccion de video con Directshow (textura Ogre)
	Copyright(C) 2013 Manuel Mellado Haro. Todos los derechos reservados.
----------------------------------------------------------------------------------------------------*/

#ifndef __FILE_UTILSOGREDSHOW_INCLUDED
#define __FILE_UTILSOGREDSHOW_INCLUDED

#include "Stdafx.h"
 
struct DirectShowData;
 
// A class for playing movies in an ogre texture
class DirectShowMovieTexture
{
public:
    DirectShowMovieTexture();
    virtual ~DirectShowMovieTexture();
 
    // basic movie methods
    bool loadMovie(const Ogre::String& moviePath);	// Loads a given movie
    Ogre::Vector2 getMovieDimensions();	// Obtains the dimensions of the current movie
    void unloadMovie();	// Unloads the current movie
 
    // methods for movie control
    void pauseMovie();	// Pauses the current movie
    void playMovie();	// Starts playing the current movie
    void rewindMovie();	// Makes the current movie rewind
	void forwardRandomMovie(); // Forward random
	void forwardMovie(float pos); // ForwardMovie
    void stopMovie();	// Stops the current movie
	void setVolume(long vol);
	long getVolume(void);
    bool isPlayingMovie(); // Is the latest video put to play, now playing?
	bool isPausingMovie(); // Is the latest video put ti play, now pausing?
	bool isStopingMovie(); // Is the latesy video put to play, now stoping?
	int getDurationMovie(); // Return duration video (seconds)
	int getActualTimeMovie(); // Return actual time video (seconds)

    // methods on movie texture
    Ogre::TexturePtr getMovieTexture();	// Obtain the ogre texture where the movie is rendered
    bool updateMovieTexture();			// Render a movie frame in the ogre texture

protected:
    Ogre::TexturePtr mTexture;	// Texture where to render the movie
    Ogre::Real mTexWidth;		// Real texture width
    Ogre::Real mTexHeight;		// Real texture height
    DirectShowData* dsdata;		// Direct Show specific data

    // Clean the full texture (paint it all black)
    void cleanTextureContents();
};






class DirectShowSound
{
public:
	DirectShowSound(LPSTR szFile);
	~DirectShowSound();

	void Play();
	void Pause();
	void Stop();
	int getDuration(); // Return duration (seconds)
	int getActualTime(); // Return actual time (seconds)
	void forwardTime(long pos); // Forward time (seconds)
	void setVolume(long vol);
	long getVolume(void);
	bool endSound();

	inline bool isReady() {return ready;}
	bool isPlaying() { return mPlaying; }
	bool isPaused() { return mPaused; }
	bool isStopped() { return mStopped; }

private:
	IBaseFilter		*pif;
	IGraphBuilder	*pigb;
	IMediaControl	*pimc;
	IMediaEventEx	*pimex;
	IBasicAudio		*piba;
	IMediaSeeking	*pims;
	bool ready;
	bool mPaused, mPlaying, mStopped;
	LPSTR dirAudio;
};

#endif // __FILE_UTILSOGREDSHOW_INCLUDED



