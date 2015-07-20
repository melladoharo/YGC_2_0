#ifndef SDL_MOVIE_AUDIO_FACTORY_H
#define SDL_MOVIE_AUDIO_FACTORY_H

#include "audiofactory.hpp"

class SDLMovieAudioFactory : public Video::MovieAudioFactory
{
    virtual boost::shared_ptr<Video::MovieAudioDecoder> createDecoder(Video::VideoState* videoState);
};

#endif
