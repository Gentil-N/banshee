#include "asset.hpp"

#include "../external/assimp.hpp"
#include "../external/spdlog.hpp"
#include "stb_image.h"
#include "minimp3_ex.h"

namespace ast
{
       void load_model(Model &model, const char *file_name, size_t opt_count, const ModelOpt *opts)
       {
              size_t vertex_size = 3;
              bool has_colors = false, has_color_channel = false, has_normals = false, has_texcoords = false;
              const struct aiScene *scene = aiImportFile(file_name, aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
              if (scene == NULL || scene->mNumMeshes == 0)
              {
                     throw std::runtime_error("failed to load model");
              }
              aiMesh *mesh = scene->mMeshes[0];
              for (size_t i = 0; i < opt_count; ++i)
              {
                     switch (opts[i])
                     {
                     case MDL_OPT_NORMALS:
                     {
                            if (mesh->mNormals == NULL)
                            {
                                   spdlog::error("normals are not available");
                                   break;
                            }
                            vertex_size += 3;
                            break;
                     }
                     case MDL_OPT_COLORS:
                     {
                            if (mesh->mColors[0] == NULL)
                            {
                                   spdlog::error("colors are not available");
                                   break;
                            }
                            vertex_size += 3;
                            break;
                     }
                     case MDL_OPT_COLORS_WITH_ALPHA:
                     {
                            if (mesh->mColors[0] == NULL)
                            {
                                   spdlog::error("colors are not available");
                                   break;
                            }
                            vertex_size += 4;
                            break;
                     }
                     case MDL_OPT_TEXCOORDS:
                     {
                            if (mesh->mTextureCoords[0] == NULL)
                            {
                                   spdlog::error("texcoords are not available");
                                   break;
                            }
                            vertex_size += 2;
                            break;
                     }
                     default:
                     {
                            throw std::runtime_error("model's option is not valid");
                            break;
                     }
                     }
              }
              if (mesh->mNumFaces == 0)
              {
                     throw std::runtime_error("faces are not available");
              }
              model.vertices.resize(vertex_size * mesh->mNumVertices);
              model.indices.resize(mesh->mNumFaces * 3);
              for (size_t i = 0; i < mesh->mNumVertices; ++i)
              {
                     model.vertices[i * vertex_size] = mesh->mVertices[i].x;
                     model.vertices[i * vertex_size + 1] = mesh->mVertices[i].y;
                     model.vertices[i * vertex_size + 2] = mesh->mVertices[i].z;
                     size_t vertex_pos = 3;
                     for (size_t j = 0; j < opt_count; ++j)
                     {
                            switch (opts[j])
                            {
                            case MDL_OPT_NORMALS:
                            {
                                   model.vertices[i * vertex_size + vertex_pos] = mesh->mNormals[i].x;
                                   ++vertex_pos;
                                   model.vertices[i * vertex_size + vertex_pos] = mesh->mNormals[i].y;
                                   ++vertex_pos;
                                   model.vertices[i * vertex_size + vertex_pos] = mesh->mNormals[i].z;
                                   ++vertex_pos;
                                   break;
                            }
                            case MDL_OPT_COLORS:
                            {
                                   model.vertices[i * vertex_size + vertex_pos] = mesh->mColors[0][i].r;
                                   ++vertex_pos;
                                   model.vertices[i * vertex_size + vertex_pos] = mesh->mColors[0][i].g;
                                   ++vertex_pos;
                                   model.vertices[i * vertex_size + vertex_pos] = mesh->mColors[0][i].b;
                                   ++vertex_pos;
                                   break;
                            }
                            case MDL_OPT_COLORS_WITH_ALPHA:
                            {
                                   model.vertices[i * vertex_size + vertex_pos] = mesh->mColors[0][i].r;
                                   ++vertex_pos;
                                   model.vertices[i * vertex_size + vertex_pos] = mesh->mColors[0][i].g;
                                   ++vertex_pos;
                                   model.vertices[i * vertex_size + vertex_pos] = mesh->mColors[0][i].b;
                                   ++vertex_pos;
                                   model.vertices[i * vertex_size + vertex_pos] = mesh->mColors[0][i].a;
                                   ++vertex_pos;
                                   break;
                            }
                            case MDL_OPT_TEXCOORDS:
                            {
                                   model.vertices[i * vertex_size + vertex_pos] = mesh->mTextureCoords[0][i].x;
                                   ++vertex_pos;
                                   model.vertices[i * vertex_size + vertex_pos] = mesh->mTextureCoords[0][i].y;
                                   ++vertex_pos;
                                   break;
                            }
                            }
                     }
              }
              for (size_t i = 0; i < mesh->mNumFaces; ++i)
              {
                     model.indices[i * 3] = mesh->mFaces[i].mIndices[0];
                     model.indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
                     model.indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
              }
       }

       std::vector<char> read_file(const char *file_name)
       {
              std::ifstream file(file_name, std::ios::binary | std::ios::in | std::ios::ate);
              if (!file.is_open())
              {
                     throw std::runtime_error("failed to open file");
              }
              size_t size = file.tellg();
              file.seekg(0, std::ios::beg);
              std::vector<char> code(size);
              file.read(code.data(), size);
              file.close();
              return code;
       }

       Image load_image(const char *file_name, ImageChannel required_channels)
       {
              Image image;
              int channels;
              image.pixels = stbi_load(file_name, &image.width, &image.height, &channels, (int)required_channels);
              if (!image.pixels)
              {
                     throw std::runtime_error("failed to load image");
              }
              return image;
       }

       void free_image(Image &image)
       {
              stbi_image_free(image.pixels);
       }

       Mp3Sound load_mp3_sound(const char *file_name)
       {
              mp3dec_t mp3d;
              mp3dec_file_info_t info;
              if (mp3dec_load(&mp3d, file_name, &info, NULL, NULL))
              {
                     throw std::runtime_error("failed to load mp3 sound");
              }
              return Mp3Sound{info.buffer, info.samples * sizeof(mp3d_sample_t), info.channels, info.hz};
       }

       void free_mp3_sound(Mp3Sound &sound)
       {
              free(sound.buffer);
       }
}