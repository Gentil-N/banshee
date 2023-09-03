#include "binding.hpp"

namespace grx
{
       VKAPI_ATTR VkBool32 VKAPI_CALL vk_dbg_callback(
           VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
       {
              if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
              {
                     spdlog::warn(pCallbackData->pMessage);
              }
              return VK_FALSE;
       }

       bool init(const InitInfo &init_info, bool debug_mode)
       {
              _c.vk_dl_handler = vkt::load_vulkan_dl(_c.ifunc);
              if (!_c.vk_dl_handler)
              {
                     return false;
              }
              std::vector<const char *> layers;
              std::vector<const char *> extensions = {
#ifdef LINUX
                  VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#endif
              };
              if (debug_mode)
              {
                     layers.push_back("VK_LAYER_KHRONOS_validation");
                     extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
              }

              {
                     if (!vkt::check_instance_layers(_c.ifunc, layers.data(), (uint32_t)layers.size()) || !vkt::check_instance_extensions(_c.ifunc, extensions.data(), (uint32_t)extensions.size()))
                     {
                            return false;
                     }
                     vk_assert(_c.ifunc.vk_enumerate_instance_version(&_c.api_version));
                     VkApplicationInfo app_info =
                         {VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, init_info.app_name, VK_MAKE_VERSION(init_info.app_version_major, init_info.app_version_minor, init_info.app_version_patch),
                          ORION, VK_MAKE_VERSION(ORN_VERS_MAJ, ORN_VERS_MIN, 0), _c.api_version};
                     VkInstanceCreateInfo instance_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, &app_info, (uint32_t)layers.size(), layers.data(), (uint32_t)extensions.size(), extensions.data()};
                     vk_assert(_c.ifunc.vk_create_instance(&instance_info, nullptr, &_c.vk_instance));
                     vkt::load_instance_func(_c.vk_instance, _c.ifunc);

                     if (debug_mode)
                     {
                            VkDebugUtilsMessengerCreateInfoEXT debug_info =
                                {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT, nullptr, 0,
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                                 vk_dbg_callback, nullptr};
                            vk_assert(_c.ifunc.vk_create_debug_utils_messenger_ext(_c.vk_instance, &debug_info, nullptr, &_c.vk_debug_utils_messenger));
                     }

#ifdef LINUX
                     VkXlibSurfaceCreateInfoKHR surface_info = {VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR, nullptr, 0, init_info.display, init_info.window};
                     vk_assert(_c.ifunc.vk_create_surface(_c.vk_instance, &surface_info, nullptr, &_c.vk_surface));
#endif //LINUX
              }

              {
                     auto gpus = vkt::enumerate_gpus(_c.vk_instance, _c.ifunc, _c.vk_surface, init_info.window_height, init_info.window_height);
                     if (gpus.empty())
                     {
                            return false;
                     }
                     _c.gpu = gpus[0];
                     for (vkt::GPU &gpu : gpus)
                     {
                            if (gpu.vk_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                            {
                                   _c.gpu = gpu;
                                   break;
                            }
                     }
                     _c.sample_count = VK_SAMPLE_COUNT_1_BIT;
                     VkSampleCountFlags sample_enum = _c.gpu.vk_properties.limits.framebufferColorSampleCounts & _c.gpu.vk_properties.limits.framebufferDepthSampleCounts;
                     for (uint32_t i = 1; i < VK_SAMPLE_COUNT_16_BIT; i = (i << 1))
                     {
                            if (sample_enum & i)
                            {
                                   _c.sample_count = (VkSampleCountFlagBits)i;
                            }
                     }
              }

              {
                     const uint32_t extension_count = 1;
                     const char *extensions[1] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
                     VkDeviceQueueCreateInfo queue_infos[2];
                     VkPhysicalDeviceFeatures features = {0};
                     //features.logicOp = VK_TRUE;
                     VkDeviceCreateInfo device_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, nullptr, 0, 1, queue_infos, 0, nullptr, extension_count, extensions, &features};
                     float priority = 1.0f;
                     queue_infos[0] = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, nullptr, 0, _c.gpu.graphics_queue_index, 1, &priority};
                     if (!_c.gpu.has_same_queue)
                     {
                            queue_infos[1] = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, nullptr, 0, _c.gpu.present_queue_index, 1, &priority};
                            device_info.queueCreateInfoCount = 2;
                     }
                     vk_assert(_c.ifunc.vk_create_device(_c.gpu.vk_physical_device, &device_info, nullptr, &_c.vk_device));
                     vkt::load_device_func(_c.vk_instance, _c.ifunc, _c.vk_device, _c.dfunc);
                     _c.dfunc.vk_get_device_queue(_c.vk_device, _c.gpu.graphics_queue_index, 0, &_c.vk_graphics_queue);
                     _c.dfunc.vk_get_device_queue(_c.vk_device, _c.gpu.present_queue_index, 0, &_c.vk_present_queue);

                     VkCommandPoolCreateInfo command_pool_info =
                         {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, _c.gpu.graphics_queue_index};
                     vk_assert(_c.dfunc.vk_create_command_pool(_c.vk_device, &command_pool_info, nullptr, &_c.vk_command_pool));

                     vkt::alloc_command_buffers(_c.vk_device, _c.dfunc, _c.vk_command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &_c.vk_cmd);
                     VkFenceCreateInfo fence_info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, 0};
                     vk_assert(_c.dfunc.vk_create_fence(_c.vk_device, &fence_info, nullptr, &_c.vk_fence));

                     VmaVulkanFunctions vma_func = {
                         _c.ifunc.vk_get_physical_device_properties, _c.ifunc.vk_get_physical_device_memory_properties,
                         _c.dfunc.vk_allocate_memory, _c.dfunc.vk_free_memory, _c.dfunc.vk_map_memory, _c.dfunc.vk_unmap_memory,
                         _c.dfunc.vk_flush_mapped_memory_ranges, _c.dfunc.vk_invalidate_mapped_memory_ranges, _c.dfunc.vk_bind_buffer_memory,
                         _c.dfunc.vk_bind_image_memory, _c.dfunc.vk_get_buffer_memory_requirements, _c.dfunc.vk_get_image_memory_requirements,
                         _c.dfunc.vk_create_buffer, _c.dfunc.vk_destroy_buffer, _c.dfunc.vk_create_image, _c.dfunc.vk_destroy_image,
                         _c.dfunc.vk_cmd_copy_buffer, nullptr, nullptr, nullptr, nullptr, nullptr};
                     VmaAllocatorCreateInfo vma_info{0};
                     vma_info.instance = _c.vk_instance;
                     vma_info.physicalDevice = _c.gpu.vk_physical_device;
                     vma_info.device = _c.vk_device;
                     vma_info.vulkanApiVersion = VK_API_VERSION_1_0;
                     vma_info.pVulkanFunctions = &vma_func;
                     vk_assert(vmaCreateAllocator(&vma_info, &_c.vma_allocator));
              }

              {
                     const uint32_t swapchain_image_count_required = 2;
                     uint32_t image_count = vk_max(_c.gpu.vk_surface_capabilities.minImageCount, vk_min(_c.gpu.vk_surface_capabilities.maxImageCount, swapchain_image_count_required));

                     uint32_t queue_indices[2];
                     VkSwapchainCreateInfoKHR swapchain_info =
                         {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR, nullptr, 0,
                          _c.vk_surface, image_count, _c.gpu.vk_surface_format.format, _c.gpu.vk_surface_format.colorSpace, _c.gpu.vk_extent, 1,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, 1, queue_indices, _c.gpu.vk_surface_capabilities.currentTransform,
                          VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, _c.gpu.vk_present_mode, VK_TRUE, VK_NULL_HANDLE};
                     if (_c.gpu.has_same_queue)
                     {
                            queue_indices[0] = _c.gpu.graphics_queue_index;
                     }
                     else
                     {
                            queue_indices[0] = _c.gpu.graphics_queue_index;
                            queue_indices[1] = _c.gpu.present_queue_index;
                            swapchain_info.queueFamilyIndexCount = 2;
                     }
                     vk_assert(_c.dfunc.vk_create_swapchain_khr(_c.vk_device, &swapchain_info, nullptr, &_c.swpch.vkh));
                     {
                            uint32_t count = 0;
                            vk_assert(_c.dfunc.vk_get_swaphain_images_khr(_c.vk_device, _c.swpch.vkh, &count, nullptr));
                            _c.swpch.vk_images.resize(count);
                            _c.swpch.vk_image_views.resize(count);
                            vk_assert(_c.dfunc.vk_get_swaphain_images_khr(_c.vk_device, _c.swpch.vkh, &count, _c.swpch.vk_images.data()));
                            for (uint32_t i = 0; i < count; ++i)
                            {
                                   VkComponentMapping components =
                                       {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
                                   VkImageSubresourceRange subresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
                                   VkImageViewCreateInfo image_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, nullptr, 0,
                                                                       _c.swpch.vk_images[i], VK_IMAGE_VIEW_TYPE_2D, _c.gpu.vk_surface_format.format, components, subresource};
                                   vk_assert(_c.dfunc.vk_create_image_view(_c.vk_device, &image_info, nullptr, &_c.swpch.vk_image_views[i]));
                            }
                     }
                     {
                            VkSemaphoreCreateInfo semaphore_info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, nullptr, 0};
                            VkFenceCreateInfo fence_info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, VK_FENCE_CREATE_SIGNALED_BIT};
                            _c.swpch.vk_image_available_sems.resize(_c.swpch.vk_images.size());
                            _c.swpch.vk_render_complete_sems.resize(_c.swpch.vk_images.size());
                            _c.swpch.vk_fences.resize(_c.swpch.vk_images.size());
                            for (uint32_t i = 0; i < _c.swpch.vk_images.size(); ++i)
                            {
                                   vk_assert(_c.dfunc.vk_create_semaphore(_c.vk_device, &semaphore_info, nullptr, &_c.swpch.vk_image_available_sems[i]));
                                   vk_assert(_c.dfunc.vk_create_semaphore(_c.vk_device, &semaphore_info, nullptr, &_c.swpch.vk_render_complete_sems[i]));
                                   vk_assert(_c.dfunc.vk_create_fence(_c.vk_device, &fence_info, nullptr, &_c.swpch.vk_fences[i]));
                                   //vk_assert(_c.dfunc.vk_reset_fences(_c.vk_device, 1, &_c.swpch.vk_fences[i]));
                            }
                     }
                     _c.swpch.curr_image_id = -1;
              }

              return true;
       }

       void end()
       {
              for (size_t i = 0; i < _c.swpch.vk_image_available_sems.size(); ++i)
              {
                     _c.dfunc.vk_destroy_fence(_c.vk_device, _c.swpch.vk_fences[i], nullptr);
                     _c.dfunc.vk_destroy_semaphore(_c.vk_device, _c.swpch.vk_render_complete_sems[i], nullptr);
                     _c.dfunc.vk_destroy_semaphore(_c.vk_device, _c.swpch.vk_image_available_sems[i], nullptr);
              }
              for (VkImageView image_view : _c.swpch.vk_image_views)
              {
                     _c.dfunc.vk_destroy_image_view(_c.vk_device, image_view, nullptr);
              }
              _c.dfunc.vk_destroy_swapchain_khr(_c.vk_device, _c.swpch.vkh, nullptr);
              vmaDestroyAllocator(_c.vma_allocator);
              _c.dfunc.vk_destroy_fence(_c.vk_device, _c.vk_fence, nullptr);
              _c.dfunc.vk_free_command_buffers(_c.vk_device, _c.vk_command_pool, 1, &_c.vk_cmd);
              _c.dfunc.vk_destroy_command_pool(_c.vk_device, _c.vk_command_pool, nullptr);
              _c.dfunc.vk_destroy_device(_c.vk_device, nullptr);
              _c.ifunc.vk_destroy_surface_khr(_c.vk_instance, _c.vk_surface, nullptr);
              if (_c.vk_debug_utils_messenger != VK_NULL_HANDLE)
              {
                     _c.ifunc.vk_destroy_debug_utils_messenger_ext(_c.vk_instance, _c.vk_debug_utils_messenger, nullptr);
              }
              _c.ifunc.vk_destroy_instance(_c.vk_instance, nullptr);
              vkt::unload_vulkan_dl(_c.vk_dl_handler);
       }

       void render_next_frame()
       {
              if (engine_3d::_e.init)
              {
                     engine_3d::engine_update();

                     uint32_t next_index = (_c.swpch.curr_image_id + 1) % (uint32_t)(_c.swpch.vk_image_views.size());

                     vk_assert(_c.dfunc.vk_wait_for_fences(_c.vk_device, 1, &_c.swpch.vk_fences[next_index], VK_TRUE, UINT64_MAX));
                     vk_assert(_c.dfunc.vk_reset_fences(_c.vk_device, 1, &_c.swpch.vk_fences[next_index]));

                     vk_assert(_c.dfunc.vk_acquire_next_image_khr(
                         _c.vk_device, _c.swpch.vkh, UINT64_MAX, _c.swpch.vk_image_available_sems[next_index], VK_NULL_HANDLE, &_c.swpch.curr_image_id));
                     
                     if(next_index != _c.swpch.curr_image_id)
                     {
                            //vk_assert(_c.dfunc.vk_wait_for_fences(_c.vk_device, 1, &_c.swpch.vk_fences[next_index], VK_TRUE, UINT64_MAX));
                            //vk_assert(_c.dfunc.vk_reset_fences(_c.vk_device, 1, &_c.swpch.vk_fences[next_index]));
                            _c.swpch.curr_image_id = next_index;
                     }

                     VkPipelineStageFlags wait_dst_stage[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
                     VkSubmitInfo submit_info = {
                         VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr,
                         1, &_c.swpch.vk_image_available_sems[_c.swpch.curr_image_id],
                         wait_dst_stage,
                         1, &engine_3d::_e.vk_command_buffers[_c.swpch.curr_image_id],
                         1, &_c.swpch.vk_render_complete_sems[_c.swpch.curr_image_id]};
                     vk_assert(_c.dfunc.vk_queue_submit(_c.vk_graphics_queue, 1, &submit_info, _c.swpch.vk_fences[_c.swpch.curr_image_id]));
                     VkPresentInfoKHR present_info = {
                         VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, nullptr,
                         1, &_c.swpch.vk_render_complete_sems[_c.swpch.curr_image_id], 1, &_c.swpch.vkh, &_c.swpch.curr_image_id, nullptr};
                     vk_assert(_c.dfunc.vk_queue_present_khr(_c.vk_present_queue, &present_info));
              }
       }

       namespace engine_3d
       {
              void init(const engine_3d::InitInfo &init_info)
              {
                     _e.updated = true;
                     {
                            _e.depth_res.create_2d(
                                _c.vk_device, _c.dfunc, _c.vma_allocator, VK_FORMAT_D32_SFLOAT, _c.gpu.vk_extent.width, _c.gpu.vk_extent.height, 1, 1, _c.sample_count,
                                VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, {}, {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1});
                            _e.color_res.create_2d(
                                _c.vk_device, _c.dfunc, _c.vma_allocator, _c.gpu.vk_surface_format.format, _c.gpu.vk_extent.width, _c.gpu.vk_extent.height, 1, 1, _c.sample_count,
                                VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, {}, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
                     }
                     {
                            _e.vk_command_buffers.resize(_c.swpch.vk_images.size());
                            vkt::alloc_command_buffers(_c.vk_device, _c.dfunc, _c.vk_command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, (uint32_t)_e.vk_command_buffers.size(), _e.vk_command_buffers.data());
                     }
                     /*BASE*/
                     {
                            VkAttachmentDescription color_attachment = {
                                0, _c.gpu.vk_surface_format.format, _c.sample_count, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
                                VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
                            VkAttachmentReference color_attachment_ref = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
                            VkAttachmentDescription depth_attachment = {
                                0, VK_FORMAT_D32_SFLOAT, _c.sample_count, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
                            VkAttachmentReference depth_attachment_ref = {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
                            VkAttachmentDescription color_resolve_attachment = {
                                0, _c.gpu.vk_surface_format.format, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE,
                                VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};
                            VkAttachmentReference color_resolve_attachment_ref = {2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
                            VkSubpassDescription subpass = {0, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, nullptr, 1, &color_attachment_ref, &color_resolve_attachment_ref, &depth_attachment_ref, 0, nullptr};
                            VkSubpassDependency dependency = {
                                VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0,
                                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, 0};
                            std::array<VkAttachmentDescription, 3> attachments = {color_attachment, depth_attachment, color_resolve_attachment};
                            VkRenderPassCreateInfo render_pass_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, nullptr, 0, (uint32_t)attachments.size(), attachments.data(), 1, &subpass, 1, &dependency};
                            vk_assert(_c.dfunc.vk_create_render_pass(_c.vk_device, &render_pass_info, nullptr, &_e.base.vk_render_pass));
                     }
                     {
                            _e.base.vk_framebuffers.resize(_c.swpch.vk_images.size());
                            for (size_t i = 0; i < _e.base.vk_framebuffers.size(); ++i)
                            {
                                   std::array<VkImageView, 3> attachments = {_e.color_res.vk_view, _e.depth_res.vk_view, _c.swpch.vk_image_views[i]};
                                   VkFramebufferCreateInfo framebuffer_info = {
                                       VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, nullptr, 0, _e.base.vk_render_pass, (uint32_t)attachments.size(), attachments.data(), _c.gpu.vk_extent.width, _c.gpu.vk_extent.height, 1};
                                   vk_assert(_c.dfunc.vk_create_framebuffer(_c.vk_device, &framebuffer_info, nullptr, &_e.base.vk_framebuffers[i]));
                            }
                     }
                     {
                            std::array<vkt::DescriptorSetBindingInfo, 3> bindings;
                            bindings[0] = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT};
                            bindings[1] = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT};
                            bindings[2] = {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT};
                            std::array<vkt::DescriptorSetInfo, 3> sets;
                            sets[0] = {1, &bindings[0]};
                            sets[1] = {1, &bindings[1]};
                            sets[2] = {1, &bindings[2]};
                            vkt::PipelineLayoutInfo pip_lay_info = {(uint32_t)sets.size(), sets.data()};
                            VkDescriptorSetLayout layouts[3];
                            _e.base.vk_pipeline_layout = vkt::create_pipeline_layout(_c.vk_device, _c.dfunc, pip_lay_info, layouts);
                            _e.base.vk_desc_lay_scene = layouts[0];
                            _e.base.vk_desc_lay_object = layouts[1];
                            _e.base.vk_desc_lay_material = layouts[2];

                            const uint32_t max_unif_buf = init_info.max_object_data + 1, max_texture_buf = init_info.max_materials, max_sets = init_info.max_object_data + init_info.max_materials + 1;
                            VkDescriptorPoolSize desc_pool_sizes[2];
                            desc_pool_sizes[0] = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, max_unif_buf};
                            desc_pool_sizes[1] = {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, max_texture_buf};
                            _e.base.vk_desc_pool = vkt::create_desc_pool(_c.vk_device, _c.dfunc, max_sets, 2, desc_pool_sizes);

                            const VkDeviceSize buffer_size = sizeof(float) * 16 * 2 + sizeof(float) * 4 * 3;
                            _e.scene_buffer.create_mapped(_c.vk_device, _c.dfunc, _c.vma_allocator, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
                            VkDescriptorSetAllocateInfo desc_alloc_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, nullptr, _e.base.vk_desc_pool, 1, &_e.base.vk_desc_lay_scene};
                            vk_assert(_c.dfunc.vk_allocate_descriptor_sets(_c.vk_device, &desc_alloc_info, &_e.base.vk_desc_set_scene));
                            VkDescriptorBufferInfo buffer_info = {_e.scene_buffer.vkh, 0, buffer_size};
                            VkWriteDescriptorSet desc_write =
                                {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, _e.base.vk_desc_set_scene, 0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &buffer_info, nullptr};
                            _c.dfunc.vk_update_descriptor_sets(_c.vk_device, 1, &desc_write, 0, nullptr);
                     }
                     {
                            VkShaderModule vertmod = vkt::create_shader_module(_c.vk_device, _c.dfunc, init_info.shadvert_size, init_info.shadvert_code);
                            VkShaderModule fragmod = vkt::create_shader_module(_c.vk_device, _c.dfunc, init_info.shadfrag_size, init_info.shadfrag_code);

                            std::array<vkt::GlslType, 3> attributes_binding_0 = {vkt::GLSL_VEC3, vkt::GLSL_VEC2, vkt::GLSL_VEC3};
                            vkt::VertexInputBinding binding_0 = {VK_VERTEX_INPUT_RATE_VERTEX, (uint32_t)attributes_binding_0.size(), attributes_binding_0.data()};
                            VkViewport viewport = {0, 0, (float)_c.gpu.vk_extent.width, (float)_c.gpu.vk_extent.height, 0.0f, 1.0f};
                            VkRect2D scissor = {{0, 0}, _c.gpu.vk_extent};
                            vkt::PipelineInfo pip_info =
                                {1, &binding_0, vertmod, fragmod, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, viewport, scissor, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE,
                                _c.sample_count, true, _e.base.vk_pipeline_layout, _e.base.vk_render_pass};
                            _e.base.vk_pipeline = vkt::create_pipeline(_c.vk_device, _c.dfunc, pip_info);
                            _c.dfunc.vk_destroy_shader_module(_c.vk_device, vertmod, nullptr);
                            _c.dfunc.vk_destroy_shader_module(_c.vk_device, fragmod, nullptr);
                     }
                     {
                            _e.base.objects.resize(init_info.max_object_data);
                            const VkDeviceSize buffer_size = sizeof(float) * 16;
                            std::vector<VkDescriptorSet> descriptors(_e.base.objects.size());
                            std::vector<VkDescriptorSetLayout> layouts(_e.base.objects.size(), _e.base.vk_desc_lay_object);
                            VkDescriptorSetAllocateInfo desc_alloc_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, nullptr, _e.base.vk_desc_pool, (uint32_t)init_info.max_object_data, layouts.data()};
                            vk_assert(_c.dfunc.vk_allocate_descriptor_sets(_c.vk_device, &desc_alloc_info, descriptors.data()));
                            for (size_t i = 0; i < _e.base.objects.size(); ++i)
                            {
                                   ObjectData *object_data = new ObjectData();
                                   object_data->index_alloc = i;
                                   object_data->vk_desc_set = descriptors[i];
                                   object_data->buffer.create_mapped(
                                       _c.vk_device, _c.dfunc, _c.vma_allocator, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
                                   VkDescriptorBufferInfo buffer_info = {object_data->buffer.vkh, 0, buffer_size};
                                   VkWriteDescriptorSet desc_write =
                                       {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, object_data->vk_desc_set, 0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &buffer_info, nullptr};
                                   _c.dfunc.vk_update_descriptor_sets(_c.vk_device, 1, &desc_write, 0, nullptr);
                                   _e.base.objects[i] = std::pair<bool, ObjectData *>(false, object_data);
                            }
                     }
                     /*PARTICLES*/
                     {
                            std::array<vkt::DescriptorSetBindingInfo, 3> bindings;
                            bindings[0] = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT};
                            bindings[1] = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT};
                            bindings[2] = {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT};
                            std::array<vkt::DescriptorSetInfo, 2> sets;
                            sets[0] = {1, &bindings[0]};
                            sets[1] = {2, &bindings[1]};
                            vkt::PipelineLayoutInfo pip_lay_info = {(uint32_t)sets.size(), sets.data()};
                            VkDescriptorSetLayout layouts[2];
                            _e.psys.vk_pipeline_layout = vkt::create_pipeline_layout(_c.vk_device, _c.dfunc, pip_lay_info, layouts);
                            _e.psys.vk_desc_lay_scene = layouts[0];
                            _e.psys.vk_desc_lay_material = layouts[1];

                            const uint32_t max_unif_buf = 1 + init_info.max_pmaterials, max_texture_buf = init_info.max_pmaterials, max_sets = init_info.max_pmaterials + 1;
                            VkDescriptorPoolSize desc_pool_sizes[2];
                            desc_pool_sizes[0] = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, max_unif_buf};
                            desc_pool_sizes[1] = {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, max_texture_buf};
                            _e.psys.vk_desc_pool = vkt::create_desc_pool(_c.vk_device, _c.dfunc, max_sets, 2, desc_pool_sizes);

                            const VkDeviceSize buffer_size = sizeof(float) * 16 * 2 + sizeof(float) * 4 * 3;
                            VkDescriptorSetAllocateInfo desc_alloc_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, nullptr, _e.psys.vk_desc_pool, 1, &_e.psys.vk_desc_lay_scene};
                            vk_assert(_c.dfunc.vk_allocate_descriptor_sets(_c.vk_device, &desc_alloc_info, &_e.psys.vk_desc_set_scene));
                            VkDescriptorBufferInfo buffer_info = {_e.scene_buffer.vkh, 0, buffer_size};
                            VkWriteDescriptorSet desc_write =
                                {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, _e.psys.vk_desc_set_scene, 0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &buffer_info, nullptr};
                            _c.dfunc.vk_update_descriptor_sets(_c.vk_device, 1, &desc_write, 0, nullptr);
                     }
                     {
                            VkShaderModule vertmod = vkt::create_shader_module(_c.vk_device, _c.dfunc, init_info.pshadvert_size, init_info.pshadvert_code);
                            VkShaderModule fragmod = vkt::create_shader_module(_c.vk_device, _c.dfunc, init_info.pshadfrag_size, init_info.pshadfrag_code);

                            std::array<vkt::GlslType, 2> attributes_binding_0 = {vkt::GLSL_VEC2, vkt::GLSL_VEC2};
                            std::array<vkt::GlslType, 3> attributes_binding_1 = {vkt::GLSL_VEC3, vkt::GLSL_VEC2, vkt::GLSL_FLOAT};
                            std::array<vkt::VertexInputBinding, 2> bindings;
                            bindings[0] = {VK_VERTEX_INPUT_RATE_VERTEX, (uint32_t)attributes_binding_0.size(), attributes_binding_0.data()};
                            bindings[1] = {VK_VERTEX_INPUT_RATE_INSTANCE, (uint32_t)attributes_binding_1.size(), attributes_binding_1.data()};
                            VkViewport viewport = {0, 0, (float)_c.gpu.vk_extent.width, (float)_c.gpu.vk_extent.height, 0.0f, 1.0f};
                            VkRect2D scissor = {{0, 0}, _c.gpu.vk_extent};
                            vkt::PipelineInfo pip_info =
                                {(uint32_t)bindings.size(), bindings.data(), vertmod, fragmod, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, viewport, scissor, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE,
                                 VK_FRONT_FACE_CLOCKWISE, _c.sample_count, false, _e.psys.vk_pipeline_layout, _e.base.vk_render_pass};
                            _e.psys.vk_pipeline = vkt::create_pipeline(_c.vk_device, _c.dfunc, pip_info);
                            _c.dfunc.vk_destroy_shader_module(_c.vk_device, vertmod, nullptr);
                            _c.dfunc.vk_destroy_shader_module(_c.vk_device, fragmod, nullptr);
                     }
                     {
                            const size_t size = _quad_vertex_size + _quad_index_size;
                            void *data_pointer = _e.psys.quad_buf.create_mapped(
                                _c.vk_device, _c.dfunc, _c.vma_allocator, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
                            memcpy((char *)data_pointer, _quad_vertices, _quad_vertex_size);
                            memcpy((char *)data_pointer + _quad_vertex_size, _quad_indices, _quad_index_size);
                            _e.psys.quad_buf.unmap(_c.vma_allocator);
                     }
                     engine_update();
                     _e.init = true;
              }

              void end()
              {
                     engine_update();
                     _c.dfunc.vk_device_wait_idle(_c.vk_device);

                     _e.psys.quad_buf.destroy(_c.vk_device, _c.dfunc, _c.vma_allocator);
                     _c.dfunc.vk_destroy_pipeline(_c.vk_device, _e.psys.vk_pipeline, nullptr);
                     _c.dfunc.vk_destroy_pipeline_layout(_c.vk_device, _e.psys.vk_pipeline_layout, nullptr);
                     _c.dfunc.vk_destroy_descriptor_pool(_c.vk_device, _e.psys.vk_desc_pool, nullptr);
                     _c.dfunc.vk_destroy_descriptor_set_layout(_c.vk_device, _e.psys.vk_desc_lay_material, nullptr);
                     _c.dfunc.vk_destroy_descriptor_set_layout(_c.vk_device, _e.psys.vk_desc_lay_scene, nullptr);

                     for (size_t i = 0; i < _e.base.objects.size(); ++i)
                     {
                            vk_assert(_c.dfunc.vk_free_descriptor_sets(_c.vk_device, _e.base.vk_desc_pool, 1, &_e.base.objects[i].second->vk_desc_set));
                            _e.base.objects[i].second->buffer.destroy(_c.vk_device, _c.dfunc, _c.vma_allocator);
                            delete _e.base.objects[i].second;
                     }
                     _c.dfunc.vk_destroy_pipeline(_c.vk_device, _e.base.vk_pipeline, nullptr);
                     _c.dfunc.vk_destroy_pipeline_layout(_c.vk_device, _e.base.vk_pipeline_layout, nullptr);
                     _e.scene_buffer.destroy(_c.vk_device, _c.dfunc, _c.vma_allocator);
                     _c.dfunc.vk_destroy_descriptor_pool(_c.vk_device, _e.base.vk_desc_pool, nullptr);
                     _c.dfunc.vk_destroy_descriptor_set_layout(_c.vk_device, _e.base.vk_desc_lay_material, nullptr);
                     _c.dfunc.vk_destroy_descriptor_set_layout(_c.vk_device, _e.base.vk_desc_lay_object, nullptr);
                     _c.dfunc.vk_destroy_descriptor_set_layout(_c.vk_device, _e.base.vk_desc_lay_scene, nullptr);
                     for (size_t i = 0; i < _e.base.vk_framebuffers.size(); ++i)
                     {
                            _c.dfunc.vk_destroy_framebuffer(_c.vk_device, _e.base.vk_framebuffers[i], nullptr);
                     }
                     _c.dfunc.vk_destroy_render_pass(_c.vk_device, _e.base.vk_render_pass, nullptr);

                     _e.color_res.destroy(_c.vk_device, _c.dfunc, _c.vma_allocator);
                     _e.depth_res.destroy(_c.vk_device, _c.dfunc, _c.vma_allocator);
                     _e.init = false;
              }

              void engine_update()
              {
                     if (!_e.operations.empty() || _e.updated == true)
                     {
                            _c.dfunc.vk_device_wait_idle(_c.vk_device);
                     }
                     if (!_e.operations.empty())
                     {
                            for (const auto &func : _e.operations)
                            {
                                   func();
                            }
                            _e.operations.clear();
                     }
                     if (_e.updated == true)
                     {
                            for (size_t i = 0; i < _e.vk_command_buffers.size(); i++)
                            {
                                   vkt::begin_command_buffer(_c.vk_device, _c.dfunc, 0, _e.vk_command_buffers[i]);
                                   /*BASE*/
                                   VkRenderPassBeginInfo render_pass_info{};
                                   render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                                   render_pass_info.renderPass = _e.base.vk_render_pass;
                                   render_pass_info.framebuffer = _e.base.vk_framebuffers[i];
                                   render_pass_info.renderArea.offset = {0, 0};
                                   render_pass_info.renderArea.extent = _c.gpu.vk_extent;
                                   VkClearValue clear_values[2];
                                   clear_values[0].color = {0.1f, 0.3f, 0.5f, 1.0f};
                                   clear_values[1].depthStencil = {1.0f, 0};
                                   render_pass_info.clearValueCount = 2;
                                   render_pass_info.pClearValues = clear_values;
                                   _c.dfunc.vk_cmd_begin_render_pass(_e.vk_command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
                                   _c.dfunc.vk_cmd_bind_pipeline(_e.vk_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _e.base.vk_pipeline);
                                   _c.dfunc.vk_cmd_bind_descriptor_sets(
                                       _e.vk_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _e.base.vk_pipeline_layout, 0, 1, &_e.base.vk_desc_set_scene, 0, nullptr);
                                   for (size_t j = 0; j < _e.base.batches.size(); ++j)
                                   {
                                          if (_e.base.batches[j]->objects.empty())
                                          {
                                                 continue;
                                          }
                                          VkDeviceSize offset = 0;
                                          _c.dfunc.vk_cmd_bind_vertex_buffers(_e.vk_command_buffers[i], 0, 1, &_e.base.batches[j]->mesh->buffer.vkh, &offset);
                                          _c.dfunc.vk_cmd_bind_index_buffer(_e.vk_command_buffers[i], _e.base.batches[j]->mesh->buffer.vkh, _e.base.batches[j]->mesh->vertices_size, VK_INDEX_TYPE_UINT32);
                                          _c.dfunc.vk_cmd_bind_descriptor_sets(
                                              _e.vk_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _e.base.vk_pipeline_layout, 2, 1, &_e.base.batches[j]->material->vk_desc_set,
                                              0, nullptr);
                                          for (size_t k = 0; k < _e.base.batches[j]->objects.size(); ++k)
                                          {
                                                 _c.dfunc.vk_cmd_bind_descriptor_sets(
                                                     _e.vk_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _e.base.vk_pipeline_layout, 1, 1,
                                                     &_e.base.batches[j]->objects[k]->vk_desc_set, 0, nullptr);
                                                 _c.dfunc.vk_cmd_draw_indexed(_e.vk_command_buffers[i], _e.base.batches[j]->mesh->index_count, 1, 0, 0, 0);
                                          }
                                   }
                                   /*PARTICLE SYSTEM*/
                                   _c.dfunc.vk_cmd_bind_pipeline(_e.vk_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _e.psys.vk_pipeline);
                                   _c.dfunc.vk_cmd_bind_descriptor_sets(
                                       _e.vk_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _e.psys.vk_pipeline_layout, 0, 1, &_e.psys.vk_desc_set_scene, 0, nullptr);
                                   VkDeviceSize offset = 0;
                                   _c.dfunc.vk_cmd_bind_vertex_buffers(_e.vk_command_buffers[i], 0, 1, &_e.psys.quad_buf.vkh, &offset);
                                   _c.dfunc.vk_cmd_bind_index_buffer(_e.vk_command_buffers[i], _e.psys.quad_buf.vkh, _quad_vertex_size, VK_INDEX_TYPE_UINT32);
                                   for (size_t j = 0; j < _e.psys.particle_systems.size(); ++j)
                                   {
                                          _c.dfunc.vk_cmd_bind_vertex_buffers(_e.vk_command_buffers[i], 1, 1, &_e.psys.particle_systems[j]->inst_buf.vkh, &offset);
                                          _c.dfunc.vk_cmd_bind_descriptor_sets(
                                              _e.vk_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _e.psys.vk_pipeline_layout, 1, 1, &_e.psys.particle_systems[j]->vk_desc_set,
                                              0, nullptr);
                                          _c.dfunc.vk_cmd_draw_indexed(_e.vk_command_buffers[i], _quad_index_count, _e.psys.particle_systems[j]->particle_count, 0, 0, 0);
                                   }

                                   _c.dfunc.vk_cmd_end_render_pass(_e.vk_command_buffers[i]);

                                   vk_assert(_c.dfunc.vk_end_command_buffer(_e.vk_command_buffers[i]));
                            }
                            _e.updated = false;
                     }
              }

              ObjectData *engine_alloc_object_data()
              {
                     for (auto &object : _e.base.objects)
                     {
                            if (!object.first)
                            {
                                   object.first = true;
                                   return object.second;
                            }
                     }
                     return nullptr;
              }

              void engine_free_object_data(ObjectData *object_data)
              {
                     _e.base.objects[object_data->index_alloc].first = false;
              }

              Mesh *create_mesh(void *vertices_data, void *indices_data, size_t vertices_size, size_t indices_size, size_t index_count)
              {
                     Mesh *mesh = new Mesh();
                     size_t size = vertices_size + indices_size;
                     void *data_pointer = mesh->buffer.create_mapped(
                         _c.vk_device, _c.dfunc, _c.vma_allocator, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
                     memcpy((char *)data_pointer, vertices_data, vertices_size);
                     memcpy((char *)data_pointer + vertices_size, indices_data, indices_size);
                     mesh->buffer.unmap(_c.vma_allocator);
                     mesh->index_count = index_count;
                     mesh->vertices_size = vertices_size;
                     return mesh;
              }

              void destroy_mesh(Mesh *mesh)
              {
                     _e.operations.push_back(
                         [mesh]() -> void
                         {
                                mesh->buffer.destroy(_c.vk_device, _c.dfunc, _c.vma_allocator);
                                delete mesh;
                         });
              }

              Texture *create_texture(uint32_t width, uint32_t height, void *pixels)
              {
                     Texture *texture = new Texture();
                     texture->image.create_2d(
                         _c.vk_device, _c.dfunc, _c.vma_allocator, VK_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL,
                         VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, {}, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

                     vkt::Buffer trans;
                     VkDeviceSize size = (VkDeviceSize)width * (VkDeviceSize)height * 4;
                     void *mapped = trans.create_mapped(_c.vk_device, _c.dfunc, _c.vma_allocator, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
                     memcpy(mapped, pixels, size);

                     vkt::begin_command_buffer(_c.vk_device, _c.dfunc, 0, _c.vk_cmd);
                     VkImageMemoryBarrier barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, nullptr, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, texture->image.vkh, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};
                     _c.dfunc.vk_cmd_pipeline_barrier(_c.vk_cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
                     VkBufferImageCopy copy = {0, 0, 0, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1}, {0, 0, 0}, {(uint32_t)width, (uint32_t)height, 1}};
                     _c.dfunc.vk_cmd_copy_buffer_to_image(_c.vk_cmd, trans.vkh, texture->image.vkh, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
                     barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                     barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                     barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                     barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                     _c.dfunc.vk_cmd_pipeline_barrier(_c.vk_cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
                     vkt::end_launch_command_buffer(_c.vk_device, _c.dfunc, _c.vk_graphics_queue, _c.vk_fence, _c.vk_cmd);

                     VkSamplerCreateInfo sampler_info = {
                         VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO, nullptr, 0, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT,
                         VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, 0.0f, VK_FALSE, 0.0f, VK_FALSE, VK_COMPARE_OP_ALWAYS, 0.0f, 0.0f,
                         VK_BORDER_COLOR_INT_OPAQUE_BLACK, VK_FALSE};
                     vk_assert(_c.dfunc.vk_create_sampler(_c.vk_device, &sampler_info, nullptr, &texture->vk_sampler));

                     _c.dfunc.vk_wait_for_fences(_c.vk_device, 1, &_c.vk_fence, VK_TRUE, UINT64_MAX);
                     _c.dfunc.vk_reset_command_buffer(_c.vk_cmd, 0);
                     _c.dfunc.vk_reset_fences(_c.vk_device, 1, &_c.vk_fence);
                     trans.destroy(_c.vk_device, _c.dfunc, _c.vma_allocator);

                     return texture;
              }

              void destroy_texture(Texture *texture)
              {
                     _e.operations.push_back(
                         [texture]() -> void
                         {
                                _c.dfunc.vk_destroy_sampler(_c.vk_device, texture->vk_sampler, nullptr);
                                texture->image.destroy(_c.vk_device, _c.dfunc, _c.vma_allocator);
                                delete texture;
                         });
              }

              Material *create_material(Texture *texture)
              {
                     Material *material = new Material();
                     _e.operations.push_back(
                         [material, texture]() -> void
                         {
                                VkDescriptorSetAllocateInfo desc_alloc_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, nullptr, _e.base.vk_desc_pool, 1, &_e.base.vk_desc_lay_material};
                                vk_assert(_c.dfunc.vk_allocate_descriptor_sets(_c.vk_device, &desc_alloc_info, &material->vk_desc_set));
                                VkDescriptorImageInfo image_info = {texture->vk_sampler, texture->image.vk_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
                                VkWriteDescriptorSet desc_write =
                                    {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, material->vk_desc_set, 0, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &image_info, nullptr, nullptr};
                                _c.dfunc.vk_update_descriptor_sets(_c.vk_device, 1, &desc_write, 0, nullptr);
                         });
                     return material;
              }

              void destroy_material(Material *material)
              {
                     _e.operations.push_back(
                         [material]() -> void
                         {
                                _c.dfunc.vk_free_descriptor_sets(_c.vk_device, _e.base.vk_desc_pool, 1, &material->vk_desc_set);
                         });
              }

              Batch *create_batch(Mesh *mesh, Material *material)
              {
                     Batch *batch = new Batch();
                     batch->mesh = mesh;
                     batch->material = material;
                     _e.base.batches.push_back(batch);
                     return batch;
              }

              void destroy_batch(Batch *batch)
              {
                     for (size_t i = 0; i < _e.base.batches.size(); ++i)
                     {
                            if (batch == _e.base.batches[i])
                            {
                                   _e.base.batches.erase(_e.base.batches.begin() + i);
                                   delete batch;
                                   return;
                            }
                     }
              }

              ObjectData *alloc_object_data(Batch *batch)
              {
                     ObjectData *object_data = engine_alloc_object_data();
                     batch->objects.push_back(object_data);
                     _e.updated = true;
                     return object_data;
              }

              void free_object_data(Batch *batch, ObjectData *object_data)
              {
                     for (size_t i = 0; i < batch->objects.size(); ++i)
                     {
                            if (object_data == batch->objects[i])
                            {
                                   batch->objects.erase(batch->objects.begin() + i);
                                   _e.updated = true;
                                   engine_free_object_data(object_data);
                                   return;
                            }
                     }
              }

              ParticleSystem *create_particle_system(Texture *texture, size_t particle_count, float grid_x, float grid_y)
              {
                     ParticleSystem *particle_system = new ParticleSystem();
                     particle_system->particle_count = (uint32_t)particle_count;
                     const VkDeviceSize inst_size =
                         (vkt::sizeof_glsl(vkt::GLSL_VEC3) + vkt::sizeof_glsl(vkt::GLSL_VEC2) + vkt::sizeof_glsl(vkt::GLSL_FLOAT)) * particle_count;
                     void *data_pointer = particle_system->inst_buf.create_mapped(
                         _c.vk_device, _c.dfunc, _c.vma_allocator, inst_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
                     data_pointer = particle_system->material_buf.create_mapped(
                         _c.vk_device, _c.dfunc, _c.vma_allocator, inst_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
                     memcpy((char *)data_pointer, &grid_x, vkt::sizeof_glsl(vkt::GLSL_FLOAT));
                     memcpy((char *)data_pointer + vkt::sizeof_glsl(vkt::GLSL_FLOAT), &grid_y, vkt::sizeof_glsl(vkt::GLSL_FLOAT));
                     _e.operations.push_back(
                         [particle_system, texture]() -> void
                         {
                                VkDescriptorSetAllocateInfo desc_alloc_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, nullptr, _e.psys.vk_desc_pool, 1, &_e.psys.vk_desc_lay_material};
                                vk_assert(_c.dfunc.vk_allocate_descriptor_sets(_c.vk_device, &desc_alloc_info, &particle_system->vk_desc_set));
                                VkDescriptorBufferInfo buffer_info = {particle_system->material_buf.vkh, 0, vkt::sizeof_glsl(vkt::GLSL_VEC2)};
                                VkDescriptorImageInfo image_info = {texture->vk_sampler, texture->image.vk_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
                                std::array<VkWriteDescriptorSet, 2> desc_writes;
                                desc_writes[0] = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, particle_system->vk_desc_set, 0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &buffer_info, nullptr};
                                desc_writes[1] = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, particle_system->vk_desc_set, 1, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &image_info, nullptr, nullptr};
                                _c.dfunc.vk_update_descriptor_sets(_c.vk_device, (uint32_t)desc_writes.size(), desc_writes.data(), 0, nullptr);
                         });
                     _e.psys.particle_systems.push_back(particle_system);
                     _e.updated = true;
                     return particle_system;
              }

              void destroy_particle_system(ParticleSystem *particle_system)
              {
                     for (size_t i = 0; i < _e.psys.particle_systems.size(); ++i)
                     {
                            if (particle_system == _e.psys.particle_systems[i])
                            {
                                   _e.psys.particle_systems.erase(_e.psys.particle_systems.begin() + i);
                                   _e.updated = true;
                            }
                     }
                     _e.operations.push_back(
                         [particle_system]() -> void
                         {
                                _c.dfunc.vk_free_descriptor_sets(_c.vk_device, _e.psys.vk_desc_pool, 1, &particle_system->vk_desc_set);
                                particle_system->material_buf.unmap(_c.vma_allocator);
                                particle_system->material_buf.destroy(_c.vk_device, _c.dfunc, _c.vma_allocator);
                                particle_system->inst_buf.unmap(_c.vma_allocator);
                                particle_system->inst_buf.destroy(_c.vk_device, _c.dfunc, _c.vma_allocator);
                         });
              }

              void update_particle(ParticleSystem *particle_system, size_t index, const void *data)
              {
                     const VkDeviceSize inst_size =
                         (vkt::sizeof_glsl(vkt::GLSL_VEC3) + vkt::sizeof_glsl(vkt::GLSL_VEC2) + vkt::sizeof_glsl(vkt::GLSL_FLOAT));
                     const VkDeviceSize offset = index * inst_size;
                     memcpy((char *)particle_system->inst_buf.mapped + offset, data, inst_size);
              }

              void update_object_data(ObjectData *object_data, const void *data)
              {
                     memcpy((char *)object_data->buffer.mapped, data, sizeof(float) * 16);
              }

              void update_scene_data(size_t offset, size_t size, const void *data)
              {
                     memcpy((char *)_e.scene_buffer.mapped + offset, data, size);
              }
       }
}