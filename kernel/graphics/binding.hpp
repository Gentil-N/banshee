#pragma once
#ifndef __BINDING_HPP__
#define __BINDING_HPP__

#include "core.hpp"
#include "vulkan_tools.hpp"

namespace grx
{
       struct Context
       {
              void *vk_dl_handler;
              uint32_t api_version;
              VkInstance vk_instance;
              vkt::InstanceVkFunc ifunc;
              VkDebugUtilsMessengerEXT vk_debug_utils_messenger = VK_NULL_HANDLE;
              VkSurfaceKHR vk_surface;
              vkt::GPU gpu;
              VkSampleCountFlagBits sample_count;
              VkDevice vk_device;
              vkt::DeviceVkFunc dfunc;
              VkQueue vk_graphics_queue, vk_present_queue;
              VkCommandPool vk_command_pool;
              VkCommandBuffer vk_cmd;
              VkFence vk_fence;
              VmaAllocator vma_allocator;
              struct
              {
                     VkSwapchainKHR vkh;
                     std::vector<VkImage> vk_images;
                     std::vector<VkImageView> vk_image_views;
                     std::vector<VkSemaphore> vk_image_available_sems, vk_render_complete_sems;
                     std::vector<VkFence> vk_fences;
                     uint32_t curr_image_id;
              } swpch;
       };
       extern Context _c;

       namespace engine_3d
       {
              struct Mesh
              {
                     vkt::Buffer buffer;
                     size_t vertices_size, index_count;
              };

              struct Texture
              {
                     vkt::Image image;
                     VkSampler vk_sampler;
              };

              struct Material
              {
                     VkDescriptorSet vk_desc_set;
              };

              struct Batch
              {
                     Mesh *mesh;
                     Material *material;
                     std::vector<ObjectData *> objects;
              };

              struct ObjectData
              {
                     VkDescriptorSet vk_desc_set;
                     vkt::Buffer buffer;
                     size_t index_alloc;
              };

              struct ParticleSystem
              {
                     VkDescriptorSet vk_desc_set;
                     vkt::Buffer material_buf;
                     vkt::Buffer inst_buf;
                     uint32_t particle_count;
              };

              struct Engine
              {
                     bool init = false;
                     vkt::Image depth_res;
                     vkt::Image color_res;
                     std::vector<VkCommandBuffer> vk_command_buffers;
                     std::vector<std::function<void()>> operations;
                     vkt::Buffer scene_buffer;
                     struct
                     {
                            VkRenderPass vk_render_pass;
                            std::vector<VkFramebuffer> vk_framebuffers;
                            VkDescriptorSetLayout vk_desc_lay_scene, vk_desc_lay_object, vk_desc_lay_material;
                            VkPipelineLayout vk_pipeline_layout;
                            VkPipeline vk_pipeline;
                            VkDescriptorPool vk_desc_pool;
                            VkDescriptorSet vk_desc_set_scene;
                            std::vector<Batch *> batches;
                            std::vector<std::pair<bool /*true if allocated*/, ObjectData *>> objects;
                     } base;
                     struct
                     {
                            VkDescriptorSetLayout vk_desc_lay_scene, vk_desc_lay_material;
                            VkPipelineLayout vk_pipeline_layout;
                            VkPipeline vk_pipeline;
                            VkDescriptorPool vk_desc_pool;
                            VkDescriptorSet vk_desc_set_scene;
                            vkt::Buffer quad_buf;
                            std::vector<ParticleSystem *> particle_systems;
                     } psys;
                     bool updated;
              };
              extern Engine _e;

              extern float _quad_vertices[16];
              extern uint32_t _quad_indices[6];
              extern size_t _quad_vertex_size;
              extern size_t _quad_index_size;
              extern uint32_t _quad_index_count;

              void engine_update();
              ObjectData *engine_alloc_object_data();
              void engine_free_object_data(ObjectData *object_data);
       }
}

#endif //__BINDING_HPP__