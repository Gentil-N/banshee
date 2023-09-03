#pragma once
#ifndef __SOUND_HPP__
#define __SOUND_HPP__

#include "../common.hpp"

namespace snd
{
       void init();
       void end();

       class Sample;
       class Sound;

       class Sample
       {
              uint32_t m_buffer;
              friend Sound;
       public:
              Sample() = default;
              ~Sample() = default;
              void create(void *buffer, size_t size, int channels, int freq);
              void destroy();
       };

       class Sound
       {
              uint32_t m_source;

       public:
              Sound() = default;
              ~Sound() = default;
              void create();
              void destroy();
              void set_sample(const Sample &sample);
              void play();
              void stop();
              void set_looping(bool looping);
              bool is_playing();
              void set_position(float x, float y, float z);
              void set_velocity(float x, float y, float z);
       };

       namespace ears
       {
              void set_position(float x, float y, float z);
              void set_velocity(float x, float y, float z);
              void set_orientation(float at_x, float at_y, float at_z, float up_x, float up_y, float up_z);
       }
}

#endif //__SOUND_HPP__