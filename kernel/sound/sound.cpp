#include "sound.hpp"

#include "../external/openal.hpp"

namespace snd
{
       ALCdevice *_alc_device;
       ALCcontext *_alc_context;

       ALenum format_of(int channels)
       {
              switch (channels)
              {
              case 1:
                     return AL_FORMAT_MONO16;
              case 2:
                     return AL_FORMAT_STEREO16;
              default:
                     throw std::runtime_error("audio format is not valid");
              }
       }

       void init()
       {
              _alc_device = alcOpenDevice(NULL);
              if (!_alc_device)
              {
                     throw std::runtime_error("failed to open audio device");
              }
              _alc_context = alcCreateContext(_alc_device, NULL);
              if (!_alc_context)
              {
                     throw std::runtime_error("failed to create audio context");
              }
              if (!alcMakeContextCurrent(_alc_context))
              {
                     throw std::runtime_error("failed to make context current");
              }
       }

       void end()
       {
              alcMakeContextCurrent(NULL);
              alcDestroyContext(_alc_context);
              alcCloseDevice(_alc_device);
       }

       void Sample::create(void *buffer, size_t size, int channels, int freq)
       {
              alGenBuffers(1, &m_buffer);
              alBufferData(m_buffer, format_of(channels), buffer, size, freq);
       }

       void Sample::destroy()
       {
              alDeleteBuffers(1, &m_buffer);
       }

       void Sound::create()
       {
              alGenSources(1, &m_source);
       }

       void Sound::destroy()
       {
              alSourcei(m_source, AL_BUFFER, 0);
              alDeleteSources(1, &m_source);
       }

       void Sound::set_sample(const Sample &sample)
       {
              alSourcei(m_source, AL_BUFFER, sample.m_buffer);
       }

       void Sound::play()
       {
              alSourcePlay(m_source);
       }

       void Sound::stop()
       {
              alSourceStop(m_source);
       }

       void Sound::set_looping(bool looping)
       {
              alSourcei(m_source, AL_LOOPING, (ALint)looping);
       }

       bool Sound::is_playing()
       {
              ALint state;
              alGetSourcei(m_source, AL_SOURCE_STATE, &state);
              return state == AL_PLAYING;
       }

       void Sound::set_position(float x, float y, float z)
       {
              alSource3f(m_source, AL_POSITION, x, y, z);
       }

       void Sound::set_velocity(float x, float y, float z)
       {
              alSource3f(m_source, AL_VELOCITY, x, y, z);
       }

       namespace ears
       {
              void set_position(float x, float y, float z)
              {
                     alListener3f(AL_POSITION, x, y, z);
              }

              void set_velocity(float x, float y, float z)
              {
                     alListener3f(AL_VELOCITY, x, y, z);
              }

              void set_orientation(float at_x, float at_y, float at_z, float up_x, float up_y, float up_z)
              {
                     ALfloat orientation[6] = {at_x, at_y, at_z, up_x, up_y, up_z};
                     alListenerfv(AL_ORIENTATION, orientation);
              }
       }
}