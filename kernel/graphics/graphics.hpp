#pragma once
#ifndef __GRAPHICS_HPP__
#define __GRAPHICS_HPP__

#include "../common.hpp"

namespace grx
{
       struct InitInfo
       {
              const char *app_name;
              uint32_t app_version_major, app_version_minor, app_version_patch;
              uint32_t window_width, window_height;
#ifdef LINUX
              Display *display;
              Window window;
#endif //LINUX
       };

       bool init(const InitInfo &init_info, bool debug_mode = false);
       void end();
       void render_next_frame();

       namespace engine_3d
       {
              struct Mesh;
              struct Texture;
              struct Material;
              struct Batch;
              struct ObjectData;
              struct ParticleSystem;

              struct InitInfo
              {
                     size_t max_object_data, max_materials, max_pmaterials;
                     uint32_t shadvert_size;
                     const void *shadvert_code;
                     uint32_t shadfrag_size;
                     const void *shadfrag_code;
                     uint32_t pshadvert_size;
                     const void *pshadvert_code;
                     uint32_t pshadfrag_size;
                     const void *pshadfrag_code;
              };

              void init(const engine_3d::InitInfo &init_info);
              void end();

              Mesh *create_mesh(void *vertices_data, void *indices_data, size_t vertices_size, size_t indices_size, size_t index_count);
              void destroy_mesh(Mesh *mesh);

              Texture *create_texture(uint32_t width, uint32_t height, void *pixels);
              void destroy_texture(Texture *texture);

              Material *create_material(Texture *texture);
              void destroy_material(Material *material);

              Batch *create_batch(Mesh *mesh, Material *texture);
              void destroy_batch(Batch *batch);

              ObjectData *alloc_object_data(Batch *batch);
              void free_object_data(Batch *batch, ObjectData *object_data);
              void update_object_data(ObjectData *object_data, const void *data);

              ParticleSystem *create_particle_system(Texture *texture, size_t particle_count, float grid_x, float grid_y);
              void destroy_particle_system(ParticleSystem *particle_system);
              /**
               * vec3 position;
               * vec2 texture_index;
               * float scale;
               */
              void update_particle(ParticleSystem *particle_system, size_t index, const void *data);

              /**
               * mat4 view;
               * mat4 projection;
               * vec4 sun_dir;
               * vec4 sun_color;
               * vec4 ambient;
               */
              void update_scene_data(size_t offset, size_t size, const void *data);

              inline void update_scene_projection(void *data)
              {
                     update_scene_data(0, sizeof(float) * 16, data);
              }
              inline void update_scene_view(void *data)
              {
                     update_scene_data(sizeof(float) * 16, sizeof(float) * 16, data);
              }
              inline void update_scene_sun_direction(void *data)
              {
                     update_scene_data(sizeof(float) * 16 * 2, sizeof(float) * 4, data);
              }
              inline void update_scene_sun_color(void *data)
              {
                     update_scene_data(sizeof(float) * 16 * 2 + sizeof(float) * 4, sizeof(float) * 4, data);
              }
              inline void update_scene_ambient(void *data)
              {
                     update_scene_data(sizeof(float) * 16 * 2 + sizeof(float) * 4 * 2, sizeof(float) * 4, data);
              }
       }
}

#endif //__GRAPHICS_HPP__