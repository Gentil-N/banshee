#pragma once
#ifndef __ASSET_HPP__
#define __ASSET_HPP__

#include "../common.hpp"

namespace ast
{

       enum ModelOpt
       {
              MDL_OPT_NORMALS = 0x1,
              MDL_OPT_COLORS = 0x2,
              MDL_OPT_COLORS_WITH_ALPHA = 0x4,
              MDL_OPT_TEXCOORDS = 0x8
       };

       struct Model
       {
              std::vector<float> vertices;
              std::vector<uint32_t> indices;
       };

       void load_model(Model &model, const char *file_name, size_t opt_count, const ModelOpt *opts);

       std::vector<char> read_file(const char *file_name);

       enum ImageChannel
       {
              IMG_CHL_GREY = 1,
              IMG_CHL_GREY_ALPHA = 2,
              IMG_CHL_RGB = 3,
              IMG_CHL_RGB_ALPHA = 4
       };

       struct Image
       {
              void *pixels;
              int width, height;
       };

       Image load_image(const char *file_name, ImageChannel required_channels);
       void free_image(Image &image);

       struct Mp3Sound
       {
              void *buffer;
              size_t size;
              int channels, freq;
       };

       Mp3Sound load_mp3_sound(const char *file_name);
       void free_mp3_sound(Mp3Sound &sound);
}

#endif //__ASSET_H__