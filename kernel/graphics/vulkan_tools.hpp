#pragma once
#ifndef __VULKAN_TOOLS_HPP__
#define __VULKAN_TOOLS_HPP__

#include "core.hpp"

#ifndef NDEBUG
#define vk_assert(expr) assert(expr == VK_SUCCESS)
#else
#define vk_assert(expr) expr
#endif //NDEBUG
#define vk_max(a, b) ((a) > (b) ? (a) : (b))
#define vk_min(a, b) ((a) < (b) ? (a) : (b))

namespace vkt
{
       namespace init
       {
              inline VkPipelineShaderStageCreateInfo pip_shader_stage_info(VkShaderStageFlagBits stage, VkShaderModule module, const char *name)
              {
                     return {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, stage, module, name, nullptr};
              }

              inline VkPipelineVertexInputStateCreateInfo pip_vertex_input_info(
                  uint32_t vertex_binding_descriptionCount,
                  const VkVertexInputBindingDescription *vertex_binding_descriptions,
                  uint32_t vertex_attribute_description_count,
                  const VkVertexInputAttributeDescription *vertex_attribute_descriptions)
              {
                     return {
                         VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, nullptr, 0, vertex_binding_descriptionCount, vertex_binding_descriptions,
                         vertex_attribute_description_count, vertex_attribute_descriptions};
              }

              inline VkPipelineInputAssemblyStateCreateInfo pip_input_assembly_info(VkPrimitiveTopology topology, VkBool32 primitive_restart_enable)
              {
                     return {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, nullptr, 0, topology, primitive_restart_enable};
              }

              inline VkPipelineViewportStateCreateInfo pip_viewport_info(uint32_t viewport_count, const VkViewport *viewports, uint32_t scissor_count, const VkRect2D *scissors)
              {
                     return {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, nullptr, 0, viewport_count, viewports, scissor_count, scissors};
              }

              inline VkPipelineRasterizationStateCreateInfo pip_rasterization_info(
                  VkBool32 depth_clamp_enable, VkBool32 rasterizer_discard_enable, VkPolygonMode polygon_mode, VkCullModeFlags cull_mode, VkFrontFace front_face,
                  VkBool32 depth_bias_enable, float depth_bias_constant_factor, float depth_bias_clamp, float depth_bias_slope_factor, float line_width)
              {
                     return {
                         VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, nullptr, 0, depth_clamp_enable, rasterizer_discard_enable, polygon_mode, cull_mode,
                         front_face, depth_bias_enable, depth_bias_constant_factor, depth_bias_clamp, depth_bias_slope_factor, line_width};
              }

              inline VkPipelineMultisampleStateCreateInfo pip_multisample_info(
                  VkSampleCountFlagBits rasterization_samples, VkBool32 sample_shading_enable, float min_sample_shading, const VkSampleMask *sample_mask,
                  VkBool32 alpha_to_coverage_enable, VkBool32 alpha_to_one_enable)
              {
                     return {
                         VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, nullptr, 0, rasterization_samples, sample_shading_enable, min_sample_shading, sample_mask,
                         alpha_to_coverage_enable, alpha_to_one_enable};
              }

              inline VkPipelineDepthStencilStateCreateInfo pip_depth_stencil_info(
                  VkBool32 depth_test_enable, VkBool32 depth_write_enable, VkCompareOp depth_compare_op, VkBool32 depth_bounds_test_enable, VkBool32 stencil_test_enable,
                  VkStencilOpState front, VkStencilOpState back, float min_depth_bounds, float max_depth_bounds)
              {
                     return {
                         VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, nullptr, 0, depth_test_enable, depth_write_enable, depth_compare_op, depth_bounds_test_enable,
                         stencil_test_enable, front, back, min_depth_bounds, max_depth_bounds};
              }

              inline VkPipelineColorBlendStateCreateInfo pip_color_blend_info(
                  VkBool32 logic_op_enable, VkLogicOp logic_op, uint32_t attachment_count, const VkPipelineColorBlendAttachmentState *attachments)
              {
                     return {
                         VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, nullptr, 0, logic_op_enable, logic_op, attachment_count, attachments, {1.0f, 1.0f, 1.0f, 1.0f}};
              }

              inline VkPipelineDynamicStateCreateInfo pip_dynamic_state_info(uint32_t dynamic_state_count, const VkDynamicState *dynamic_states)
              {
                     return {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, nullptr, 0, dynamic_state_count, dynamic_states};
              }

              inline VkImageCreateInfo image(
                  VkImageCreateFlags flags, VkFormat format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits sample_count, VkImageTiling tiling,
                  VkImageUsageFlags usage, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, uint32_t queue_family_index_count = 0,
                  const uint32_t *queue_family_indices = nullptr, VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED)
              {
                     return {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, nullptr, flags, VK_IMAGE_TYPE_2D, format, {width, height, depth}, mip_levels, array_layers, sample_count, tiling, usage, sharing_mode, queue_family_index_count, queue_family_indices, initial_layout};
              }

              inline VkImageCreateInfo image_2d(
                  VkFormat format, uint32_t width, uint32_t height, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits sample_count, VkImageTiling tiling,
                  VkImageUsageFlags usage, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, uint32_t queue_family_index_count = 0,
                  const uint32_t *queue_family_indices = nullptr, VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED)
              {
                     return image(
                         0, format, width, height, 1, mip_levels, array_layers, sample_count, tiling, usage, sharing_mode, queue_family_index_count, queue_family_indices, initial_layout);
              }

              inline VkImageCreateInfo image_cube(
                  VkFormat format, uint32_t width, uint32_t height, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits sample_count, VkImageTiling tiling,
                  VkImageUsageFlags usage, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, uint32_t queue_family_index_count = 0,
                  const uint32_t *queue_family_indices = nullptr, VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED)
              {
                     return image(
                         VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, format, width, height, 1, mip_levels, array_layers, sample_count, tiling, usage, sharing_mode,
                         queue_family_index_count, queue_family_indices, initial_layout);
              }

              inline VkImageViewCreateInfo image_view(
                  VkImage image, VkImageViewType view_type, VkFormat format, VkComponentMapping components, VkImageSubresourceRange subresource_range)
              {
                     return {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, nullptr, 0, image, view_type, format, components, subresource_range};
              }

              inline VkImageViewCreateInfo image_view_2d(
                  VkImage image, VkFormat format, VkComponentMapping components, VkImageSubresourceRange subresource_range)
              {
                     return {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, nullptr, 0, image, VK_IMAGE_VIEW_TYPE_2D, format, components, subresource_range};
              }

              inline VkImageViewCreateInfo image_view_2d_array(
                  VkImage image, VkFormat format, VkComponentMapping components, VkImageSubresourceRange subresource_range)
              {
                     return {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, nullptr, 0, image, VK_IMAGE_VIEW_TYPE_2D_ARRAY, format, components, subresource_range};
              }
       }

       struct GPU;
       struct InstanceVkFunc;
       struct DeviceVkFunc;
       struct Swapchain;
       struct BaseRenderer;
       struct Buffer;
       struct Image;
       struct DescriptorSetPoolInfo;
       struct DescriptorSetPool;
       struct PipelineInfo;
       struct VertexInputBinding;
       struct PipelineLayoutInfo;
       struct DescriptorSetInfo;
       struct DescriptorSetBindingInfo;

       struct GPU
       {
              VkPhysicalDevice vk_physical_device;
              VkPhysicalDeviceProperties vk_properties;
              VkPhysicalDeviceFeatures vk_features;
              VkSurfaceFormatKHR vk_surface_format;
              VkPresentModeKHR vk_present_mode;
              VkExtent2D vk_extent;
              VkSurfaceCapabilitiesKHR vk_surface_capabilities;
              uint32_t graphics_queue_index, present_queue_index;
              bool has_same_queue;
       };

       struct InstanceVkFunc
       {
              PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr;
              PFN_vkCreateInstance vk_create_instance;
              PFN_vkEnumerateInstanceExtensionProperties vk_enumerate_instance_extension_properties;
              PFN_vkEnumerateInstanceLayerProperties vk_enumerate_instance_layer_properties;
#ifdef VK_VERSION_1_1
              PFN_vkEnumerateInstanceVersion vk_enumerate_instance_version;
#endif //VK_VERSION_1_1
              PFN_vkGetDeviceProcAddr vk_get_device_proc_addr;
              PFN_vkDestroyInstance vk_destroy_instance;
              PFN_vkEnumeratePhysicalDevices vk_enumerate_physical_devices;
              PFN_vkGetPhysicalDeviceFeatures vk_get_physical_device_features;
              PFN_vkGetPhysicalDeviceFormatProperties vk_get_physical_device_format_properties;
              PFN_vkGetPhysicalDeviceImageFormatProperties vk_get_physical_device_image_format_properties;
              PFN_vkGetPhysicalDeviceProperties vk_get_physical_device_properties;
              PFN_vkGetPhysicalDeviceQueueFamilyProperties vk_get_physical_device_queue_family_properties;
              PFN_vkGetPhysicalDeviceMemoryProperties vk_get_physical_device_memory_properties;
              PFN_vkCreateDevice vk_create_device;
              PFN_vkEnumerateDeviceExtensionProperties vk_enumerate_device_extension_properties;
              PFN_vkEnumerateDeviceLayerProperties vk_enumerate_device_layer_properties;
              PFN_vkGetPhysicalDeviceSparseImageFormatProperties vk_get_physical_device_sparse_image_format_properties;
              PFN_vkDestroySurfaceKHR vk_destroy_surface_khr;
              PFN_vkGetPhysicalDeviceSurfaceSupportKHR vk_get_physical_device_surface_support_khr;
              PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vk_get_physical_device_surface_capabilities_khr;
              PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vk_get_physical_device_surface_formats_khr;
              PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vk_get_physical_device_surface_present_modes_khr;
#ifdef LINUX
              PFN_vkCreateXlibSurfaceKHR vk_create_surface;
#endif //LINUX
              PFN_vkCreateDebugUtilsMessengerEXT vk_create_debug_utils_messenger_ext;
              PFN_vkDestroyDebugUtilsMessengerEXT vk_destroy_debug_utils_messenger_ext;
       };

       struct DeviceVkFunc
       {
              PFN_vkDestroyDevice vk_destroy_device;
              PFN_vkGetDeviceQueue vk_get_device_queue;
              PFN_vkQueueSubmit vk_queue_submit;
              PFN_vkQueueWaitIdle vk_queue_wait_idle;
              PFN_vkDeviceWaitIdle vk_device_wait_idle;
              PFN_vkAllocateMemory vk_allocate_memory;
              PFN_vkFreeMemory vk_free_memory;
              PFN_vkMapMemory vk_map_memory;
              PFN_vkUnmapMemory vk_unmap_memory;
              PFN_vkFlushMappedMemoryRanges vk_flush_mapped_memory_ranges;
              PFN_vkInvalidateMappedMemoryRanges vk_invalidate_mapped_memory_ranges;
              PFN_vkGetDeviceMemoryCommitment vk_get_device_memory_commitment;
              PFN_vkBindBufferMemory vk_bind_buffer_memory;
              PFN_vkBindImageMemory vk_bind_image_memory;
              PFN_vkGetBufferMemoryRequirements vk_get_buffer_memory_requirements;
              PFN_vkGetImageMemoryRequirements vk_get_image_memory_requirements;
              PFN_vkGetImageSparseMemoryRequirements vk_get_image_sparse_memory_requirements;
              PFN_vkQueueBindSparse vk_queue_bind_sparse;
              PFN_vkCreateFence vk_create_fence;
              PFN_vkDestroyFence vk_destroy_fence;
              PFN_vkResetFences vk_reset_fences;
              PFN_vkGetFenceStatus vk_get_fence_status;
              PFN_vkWaitForFences vk_wait_for_fences;
              PFN_vkCreateSemaphore vk_create_semaphore;
              PFN_vkDestroySemaphore vk_destroy_semaphore;
              PFN_vkCreateEvent vk_create_event;
              PFN_vkDestroyEvent vk_destroy_event;
              PFN_vkGetEventStatus vk_get_event_status;
              PFN_vkSetEvent vk_set_event;
              PFN_vkResetEvent vk_reset_event;
              PFN_vkCreateQueryPool vk_create_query_pool;
              PFN_vkDestroyQueryPool vk_destroy_query_pool;
              PFN_vkGetQueryPoolResults vk_get_query_pool_results;
              PFN_vkCreateBuffer vk_create_buffer;
              PFN_vkDestroyBuffer vk_destroy_buffer;
              PFN_vkCreateBufferView vk_create_buffer_view;
              PFN_vkDestroyBufferView vk_destroy_buffer_view;
              PFN_vkCreateImage vk_create_image;
              PFN_vkDestroyImage vk_destroy_image;
              PFN_vkGetImageSubresourceLayout vk_get_image_subresource_layout;
              PFN_vkCreateImageView vk_create_image_view;
              PFN_vkDestroyImageView vk_destroy_image_view;
              PFN_vkCreateShaderModule vk_create_shader_module;
              PFN_vkDestroyShaderModule vk_destroy_shader_module;
              PFN_vkCreatePipelineCache vk_create_pipeline_cache;
              PFN_vkDestroyPipelineCache vk_destroy_pipeline_cache;
              PFN_vkGetPipelineCacheData vk_get_pipeline_cache_data;
              PFN_vkMergePipelineCaches vk_merge_pipeline_caches;
              PFN_vkCreateGraphicsPipelines vk_create_graphics_pipelines;
              PFN_vkCreateComputePipelines vk_create_compute_pipelines;
              PFN_vkDestroyPipeline vk_destroy_pipeline;
              PFN_vkCreatePipelineLayout vk_create_pipeline_layout;
              PFN_vkDestroyPipelineLayout vk_destroy_pipeline_layout;
              PFN_vkCreateSampler vk_create_sampler;
              PFN_vkDestroySampler vk_destroy_sampler;
              PFN_vkCreateDescriptorSetLayout vk_create_descriptor_set_layout;
              PFN_vkDestroyDescriptorSetLayout vk_destroy_descriptor_set_layout;
              PFN_vkCreateDescriptorPool vk_create_descriptor_pool;
              PFN_vkDestroyDescriptorPool vk_destroy_descriptor_pool;
              PFN_vkResetDescriptorPool vk_reset_descriptor_pool;
              PFN_vkAllocateDescriptorSets vk_allocate_descriptor_sets;
              PFN_vkFreeDescriptorSets vk_free_descriptor_sets;
              PFN_vkUpdateDescriptorSets vk_update_descriptor_sets;
              PFN_vkCreateFramebuffer vk_create_framebuffer;
              PFN_vkDestroyFramebuffer vk_destroy_framebuffer;
              PFN_vkCreateRenderPass vk_create_render_pass;
              PFN_vkDestroyRenderPass vk_destroy_render_pass;
              PFN_vkGetRenderAreaGranularity vk_get_render_area_granularity;
              PFN_vkCreateCommandPool vk_create_command_pool;
              PFN_vkDestroyCommandPool vk_destroy_command_pool;
              PFN_vkResetCommandPool vk_reset_command_pool;
              PFN_vkAllocateCommandBuffers vk_allocate_command_buffers;
              PFN_vkFreeCommandBuffers vk_free_command_buffers;
              PFN_vkBeginCommandBuffer vk_begin_command_buffer;
              PFN_vkEndCommandBuffer vk_end_command_buffer;
              PFN_vkResetCommandBuffer vk_reset_command_buffer;
              PFN_vkCmdBindPipeline vk_cmd_bind_pipeline;
              PFN_vkCmdSetViewport vk_cmd_set_viewport;
              PFN_vkCmdSetScissor vk_cmd_set_scissor;
              PFN_vkCmdSetLineWidth vk_cmd_set_line_width;
              PFN_vkCmdSetDepthBias vk_cmd_set_depth_bias;
              PFN_vkCmdSetBlendConstants vk_cmd_set_blend_constants;
              PFN_vkCmdSetDepthBounds vk_cmd_set_depth_bounds;
              PFN_vkCmdSetStencilCompareMask vk_cmd_set_stencil_compare_mask;
              PFN_vkCmdSetStencilWriteMask vk_cmd_set_stencil_write_mask;
              PFN_vkCmdSetStencilReference vk_cmd_set_stencil_reference;
              PFN_vkCmdBindDescriptorSets vk_cmd_bind_descriptor_sets;
              PFN_vkCmdBindIndexBuffer vk_cmd_bind_index_buffer;
              PFN_vkCmdBindVertexBuffers vk_cmd_bind_vertex_buffers;
              PFN_vkCmdDraw vk_cmd_draw;
              PFN_vkCmdDrawIndexed vk_cmd_draw_indexed;
              PFN_vkCmdDrawIndirect vk_cmd_draw_indirect;
              PFN_vkCmdDrawIndexedIndirect vk_cmd_draw_indexed_indirect;
              PFN_vkCmdDispatch vk_cmd_dispatch;
              PFN_vkCmdDispatchIndirect vk_cmd_dispatch_indirect;
              PFN_vkCmdCopyBuffer vk_cmd_copy_buffer;
              PFN_vkCmdCopyImage vk_cmd_copy_image;
              PFN_vkCmdBlitImage vk_cmd_blit_image;
              PFN_vkCmdCopyBufferToImage vk_cmd_copy_buffer_to_image;
              PFN_vkCmdCopyImageToBuffer vk_cmd_copy_image_to_buffer;
              PFN_vkCmdUpdateBuffer vk_cmd_update_buffer;
              PFN_vkCmdFillBuffer vk_cmd_fill_buffer;
              PFN_vkCmdClearColorImage vk_cmd_clear_color_image;
              PFN_vkCmdClearDepthStencilImage vk_cmd_clear_depth_stencil_image;
              PFN_vkCmdClearAttachments vk_cmd_clear_attachments;
              PFN_vkCmdResolveImage vk_cmd_resolve_image;
              PFN_vkCmdSetEvent vk_cmd_set_event;
              PFN_vkCmdResetEvent vk_cmd_reset_event;
              PFN_vkCmdWaitEvents vk_cmd_wait_events;
              PFN_vkCmdPipelineBarrier vk_cmd_pipeline_barrier;
              PFN_vkCmdBeginQuery vk_cmd_begin_query;
              PFN_vkCmdEndQuery vk_cmd_end_query;
              PFN_vkCmdResetQueryPool vk_cmd_reset_query_pool;
              PFN_vkCmdWriteTimestamp vk_cmd_write_timestamp;
              PFN_vkCmdCopyQueryPoolResults vk_cmd_copy_query_pool_results;
              PFN_vkCmdPushConstants vk_cmd_push_constants;
              PFN_vkCmdBeginRenderPass vk_cmd_begin_render_pass;
              PFN_vkCmdNextSubpass vk_cmd_next_subpass;
              PFN_vkCmdEndRenderPass vk_cmd_end_render_pass;
              PFN_vkCmdExecuteCommands vk_cmd_execute_commands;
              PFN_vkCreateSwapchainKHR vk_create_swapchain_khr;
              PFN_vkDestroySwapchainKHR vk_destroy_swapchain_khr;
              PFN_vkGetSwapchainImagesKHR vk_get_swaphain_images_khr;
              PFN_vkAcquireNextImageKHR vk_acquire_next_image_khr;
              PFN_vkQueuePresentKHR vk_queue_present_khr;
       };

       struct Swapchain
       {
              VkSwapchainKHR vkh;
              std::vector<VkImage> vk_images;
              std::vector<VkImageView> vk_image_views;
              std::vector<VkSemaphore> vk_image_available_sems, vk_render_complete_sems;
              std::vector<VkFence> vk_fences;
              uint32_t curr_image_id;
       };

       struct Buffer
       {
              VkBuffer vkh;
              VmaAllocation allocation;
              void *mapped = nullptr;
              void create(VkDevice device, const DeviceVkFunc &func, VmaAllocator vma_allocator, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage vma_usage);
              void *create_mapped(VkDevice device, const DeviceVkFunc &func, VmaAllocator vma_allocator, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage vma_usage);
              void destroy(VkDevice device, const DeviceVkFunc &func, VmaAllocator vma_allocator);
              void *map(VmaAllocator vma_allocator);
              void unmap(VmaAllocator vma_allocator);
       };

       struct Image
       {
              VkImage vkh;
              VmaAllocation allocation;
              VkImageView vk_view;

              void create(
                  VkDevice device, const DeviceVkFunc &func, VmaAllocator vma_allocator, VkImageCreateFlags flags, VkImageViewType image_view_type, VkFormat format, uint32_t width,
                  uint32_t height, uint32_t depth, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits sample_count, VkImageTiling tiling, VkImageUsageFlags usage,
                  VkComponentMapping components, VkImageSubresourceRange subresource_range, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
                  uint32_t queue_family_index_count = 0, const uint32_t *queue_family_indices = nullptr, VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED);
              void destroy(VkDevice device, const DeviceVkFunc &func, VmaAllocator vma_allocator);

              inline void create_2d(
                  VkDevice device, const DeviceVkFunc &func, VmaAllocator vma_allocator, VkFormat format, uint32_t width, uint32_t height,
                  uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits sample_count, VkImageTiling tiling, VkImageUsageFlags usage,
                  VkComponentMapping components, VkImageSubresourceRange subresource_range, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
                  uint32_t queue_family_index_count = 0, const uint32_t *queue_family_indices = nullptr, VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED)
              {
                     create(
                         device, func, vma_allocator, 0, VK_IMAGE_VIEW_TYPE_2D, format, width, height, 1, mip_levels, array_layers, sample_count, tiling, usage, components,
                         subresource_range, sharing_mode, queue_family_index_count, queue_family_indices, initial_layout);
              }

              inline void create_cube(
                  VkDevice device, const DeviceVkFunc &func, VmaAllocator vma_allocator, VkFormat format, uint32_t width, uint32_t height,
                  uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits sample_count, VkImageTiling tiling, VkImageUsageFlags usage,
                  VkComponentMapping components, VkImageSubresourceRange subresource_range, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
                  uint32_t queue_family_index_count = 0, const uint32_t *queue_family_indices = nullptr, VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED)
              {
                     create(
                         device, func, vma_allocator, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, VK_IMAGE_VIEW_TYPE_CUBE, format, width, height, 1, mip_levels, array_layers, sample_count,
                         tiling, usage, components, subresource_range, sharing_mode, queue_family_index_count, queue_family_indices, initial_layout);
              }
       };

       struct DescriptorSetPoolInfo
       {
              uint32_t set_count;                   /*sets to be allocated*/
              uint32_t uniform_buffer_count;        /*count by set*/
              uint32_t combined_image_sample_count; /*same*/
       };

       struct DescriptorSetPool
       {
              VkDescriptorPool vk_desc_pool;
              utl::node<VkDescriptorSet> head;

              void create(VkDevice device, const DeviceVkFunc &func, const DescriptorSetPoolInfo & desc_pool_set_info, VkDescriptorSetLayout desc_set_layout);
              void destroy(VkDevice device, const DeviceVkFunc &func);

              utl::node<VkDescriptorSet> alloc();
              void free(utl::node<VkDescriptorSet> desc_set);
       };

       enum GlslType : size_t
       {
              GLSL_INT,
              GLSL_FLOAT,
              GLSL_VEC2,
              GLSL_VEC3,
              GLSL_VEC4,
              GLSL_IVEC2,
              GLSL_IVEC3,
              GLSL_IVEC4,
              GLSL_MAT3,
              GLSL_MAT4,
       };

       struct PipelineInfo
       {
              uint32_t vertex_input_binding_count;
              const VertexInputBinding *vertex_input_bindings;
              VkShaderModule vertex_module, fragment_module;
              VkPrimitiveTopology topology;
              VkViewport viewport;
              VkRect2D scissor;
              VkPolygonMode polygon_mode;
              VkCullModeFlagBits cull_mode;
              VkFrontFace front_face;
              VkSampleCountFlagBits rasterization_samples;
              bool enable_depth_test;
              VkPipelineLayout pipeline_layout;
              VkRenderPass render_pass;
       };

       struct VertexInputBinding
       {
              VkVertexInputRate input_rate;
              uint32_t attribute_count;
              const GlslType *attributes;
       };

       struct PipelineLayoutInfo
       {
              uint32_t desc_set_info_count;
              const DescriptorSetInfo *desc_set_infos;
       };

       struct DescriptorSetInfo
       {
              uint32_t desc_set_binding_info_count;
              const DescriptorSetBindingInfo *desc_set_binding_infos;
       };

       struct DescriptorSetBindingInfo
       {
              VkDescriptorType desc_type;
              uint32_t desc_count;
              VkShaderStageFlags shader_stages;
       };

       void *load_vulkan_dl(InstanceVkFunc &func);

       void unload_vulkan_dl(void *dl_handler);

       void load_instance_func(VkInstance instance, InstanceVkFunc &func);

       void load_device_func(VkInstance instance, const InstanceVkFunc &ifunc, VkDevice device, DeviceVkFunc &dfunc);

       bool check_instance_layers(const InstanceVkFunc &func, const char **required_layers, uint32_t layer_count);

       bool check_instance_extensions(const InstanceVkFunc &func, const char **required_extensions, uint32_t extensions_count);

       bool check_physical_device_extensions(VkPhysicalDevice physical_device, const InstanceVkFunc &func, const char **required_extensions, uint32_t extension_count);

       VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR> &surface_formats);

       VkExtent2D choose_extent(VkSurfaceCapabilitiesKHR surface_capabilities, uint32_t width, uint32_t height);

       VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR> &present_modes);

       std::vector<GPU> enumerate_gpus(VkInstance instance, const InstanceVkFunc &func, VkSurfaceKHR surface, uint32_t width, uint32_t height);

       VkShaderModule create_shader_module(VkDevice device, const DeviceVkFunc &func, size_t size, const void *code);

       VkImageView create_image_view(
           VkDevice device, const DeviceVkFunc &func, VkImage image, VkImageViewType image_view_type, VkFormat format, VkComponentMapping components,
           VkImageSubresourceRange subresource_range);

       inline VkImageView create_image_view_2d(
           VkDevice device, const DeviceVkFunc &func, VkImage image, VkFormat format, VkComponentMapping components, VkImageSubresourceRange subresource_range)
       {
              return create_image_view(device, func, image, VK_IMAGE_VIEW_TYPE_2D, format, components, subresource_range);
       }

       inline VkImageView create_image_view_cube(
           VkDevice device, const DeviceVkFunc &func, VkImage image, VkFormat format, VkComponentMapping components, VkImageSubresourceRange subresource_range)
       {
              return create_image_view(device, func, image, VK_IMAGE_VIEW_TYPE_CUBE, format, components, subresource_range);
       }

       VkImage create_image(
           VkDevice device, const DeviceVkFunc &func, VkImageCreateFlags flags, VkFormat format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mip_levels, uint32_t array_layers,
           VkSampleCountFlagBits sample_count, VkImageTiling tiling, VkImageUsageFlags usage, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, uint32_t queue_family_index_count = 0,
           const uint32_t *queue_family_indices = nullptr, VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED);

       inline VkImage create_image_2d(
           VkDevice device, const DeviceVkFunc &func, VkFormat format, uint32_t width, uint32_t height, uint32_t mip_levels, uint32_t array_layers,
           VkSampleCountFlagBits sample_count, VkImageTiling tiling, VkImageUsageFlags usage, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, uint32_t queue_family_index_count = 0,
           const uint32_t *queue_family_indices = nullptr, VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED)
       {
              return create_image(
                  device, func, 0, format, width, height, 1, mip_levels, array_layers, sample_count, tiling, usage, sharing_mode, queue_family_index_count, queue_family_indices,
                  initial_layout);
       }

       inline VkImage create_image_cube(
           VkDevice device, const DeviceVkFunc &func, VkFormat format, uint32_t width, uint32_t height, uint32_t mip_levels, uint32_t array_layers,
           VkSampleCountFlagBits sample_count, VkImageTiling tiling, VkImageUsageFlags usage, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, uint32_t queue_family_index_count = 0,
           const uint32_t *queue_family_indices = nullptr, VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED)
       {
              return create_image(
                  device, func, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, format, width, height, 1, mip_levels, array_layers, sample_count, tiling, usage, sharing_mode,
                  queue_family_index_count, queue_family_indices, initial_layout);
       }

       VkDescriptorSetLayout create_desc_set_layout(VkDevice device, const DeviceVkFunc &func, uint32_t binding_count, const VkDescriptorSetLayoutBinding *bindings);

       VkDescriptorPool create_desc_pool(VkDevice device, const DeviceVkFunc &func, uint32_t max_sets, uint32_t pool_size_count, const VkDescriptorPoolSize *pool_sizes);

       void alloc_command_buffers(VkDevice device, const DeviceVkFunc &func, VkCommandPool command_pool, VkCommandBufferLevel level, uint32_t count, VkCommandBuffer *command_buffers);

       void begin_command_buffer(VkDevice device, const DeviceVkFunc &func, VkCommandBufferUsageFlags usage, VkCommandBuffer command_buffer);

       void end_launch_command_buffer(VkDevice device, const DeviceVkFunc &func, VkQueue queue, VkFence fence, VkCommandBuffer command_buffer);

       size_t sizeof_glsl(GlslType type);

       VkFormat format_glsl(GlslType type);

       VkPipeline create_pipeline(VkDevice device, const DeviceVkFunc &func, const PipelineInfo &pip_info);

       VkPipelineLayout create_pipeline_layout(VkDevice device, const DeviceVkFunc &func, const PipelineLayoutInfo &pip_lay_info, VkDescriptorSetLayout *desc_set_layouts /*size : desc_set_info_count*/);
}

#endif //__VULKAN_TOOLS_HPP__