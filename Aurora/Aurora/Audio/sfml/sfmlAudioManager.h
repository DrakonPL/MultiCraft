#ifndef sfmlAudioManager_H
#define sfmlAudioManager_H

#include <Aurora/Audio/AudioManager.h>

namespace Aurora
{
namespace Audio
{
    class sfmlAudioManager : public AudioManager
    {
    private:


    public:

        sfmlAudioManager();

        bool Init();
        bool SupportedFormat(AudioFormat type);

    };
}
}

#endif
