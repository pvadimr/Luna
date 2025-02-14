#include "../../config.h"
#ifdef AGAUDIO3
#include "CAudioStream.h"

namespace agEngine
{
    namespace audio
    {
        CAudioStream::CAudioStream(CAudioData* data)
        : attachedData(data)
        {
            if (data)
                data->grab();
        }

        CAudioStream::~CAudioStream()
        {
            detatchAudioData();
        }

        void CAudioStream::attachAudioData(CAudioData* data)
        {
            detatchAudioData();

            attachedData = data;
            data->grab();
        }

        void CAudioStream::detatchAudioData()
        {
            if (attachedData)
                attachedData->drop();

            attachedData = 0;
        }

        void CAudioStream::rewind()
        {
            attachedData->rewind();
        }

        bool CAudioStream::readSamples(CAudioSource* source, ALuint bufferId)
        {
            return attachedData->readSamples(source, bufferId);
        }

        bool CAudioStream::readMemorySamples(CAudioSource* source, ALuint bufferId, char* lpBuffer, long nBytes)
        {
            return attachedData->readMemorySamples(source, bufferId, lpBuffer, nBytes);
        }

        bool CAudioStream::hasFinished() const
        {
            return attachedData->hasFinished();
        }
    }
}
#endif
