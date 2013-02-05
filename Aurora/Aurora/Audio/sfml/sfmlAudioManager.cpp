#include <Aurora/Audio/sfml/sfmlAudioManager.h>

namespace Aurora
{
namespace Audio
{
    sfmlAudioManager::sfmlAudioManager()
    {

    }

    bool sfmlAudioManager::Init()
    {

        return true;
    }

    bool sfmlAudioManager::SupportedFormat(AudioFormat type)
    {
        switch(type)
        {
            case MP3:
                return false;
            break;

            case WAV:
                return true;
            break;

            case OGG:
                return true;
            break;
        }

        return false;
    }

}
}



