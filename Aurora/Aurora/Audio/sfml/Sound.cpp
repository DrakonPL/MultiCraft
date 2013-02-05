#include <Aurora/Audio/Sound.h>

#include <SFML/Audio.hpp>

sf::Music _music;
sf::SoundBuffer _buffer;
sf::Sound _sound;

namespace Aurora
{
namespace Audio
{
    Sound::Sound(std::string name,std::string filePath,bool streaming)
    {
        _name = name;
        _filePath = filePath;
        _stream = streaming;

        _looped = false;
        _paused = false;
        _loaded = false;
        
        if (_stream)
        {
            if(_music.OpenFromFile(filePath))
            {
                _loaded = true;
            } 
        }else
        {
            if(_buffer.LoadFromFile(filePath))
            {
                _sound.SetBuffer(_buffer);
                _loaded = true;
            }
        }
    }

    bool Sound::IsPlaying()
    {
        if (_stream)
        {
            return _music.GetStatus() == sf::Music::Playing;
        }else
        {
            return _sound.GetStatus() == sf::Sound::Playing;
        }
        
        return false;
    }

    void Sound::SetLoop(bool state)
    {
        _looped = state;
    }

    void Sound::Play()
    {
        if (_stream)
        {
            _music.Play();
        }else
        {
            _sound.Play();
        }
    }

    void Sound::Stop()
    {
        if (_stream)
        {
            _music.Stop();
        }else
        {
            _sound.Stop();
        }
    }

    void Sound::Pause()
    {
        if (_stream)
        {
            _music.Pause();
        }else
        {
             _sound.Pause();
        }
    }
}
}


