#ifndef OGG_H
#define OGG_H
#include "../../config.h"
#ifdef AGAUDIO3
#ifdef OGG
#ifdef __APPLE__
#include <OpenAL/al.h>
#else
#ifdef WIN32
#include <al.h>
#else
#include <AL/al.h>
#endif
#endif

#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <iostream>
#include <cstring>

namespace agEngine
{
    namespace Audio
    {
        struct SOggFile
        {
            char *dataPtr;
            unsigned int dataSize;
            unsigned int dataRead;
        };

        class OggFile
        {
        public:
            int open(const std::string&, const bool &loadToMemory);
            void close();
            long samples();
            long getPosition();
            void setPosition(long);
            double getLength();
            double curTime();
            void rewind();
            int read(char*, int, int, int,int, int*);
            void bufferthis(ALuint*, char*, int);
        private:

            FILE* file;
            SOggFile oggData;
            OggVorbis_File stream;
            ov_callbacks vorbisCallbacks;
            vorbis_info* info;

            ALuint buffers[2];
            ALenum format;

            bool loadedInMemory;

            unsigned int bufferSize;
        };
    }
}

#endif
#endif
#endif
