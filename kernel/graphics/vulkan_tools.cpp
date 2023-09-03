#include "vulkan_tools.hpp"

#include <iostream>

#ifdef LINUX
#include <dlfcn.h>
#endif //LINUX

namespace vkt
{
       void *load_vulkan_dl(InstanceVkFunc &func)
       {
#ifdef LINUX
              void *vk_dl_handler = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
              if (!vk_dl_handler)
              {
                     vk_dl_handler = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
              }
#endif //LINUXI
              if (!vk_dl_handler)
              {
                     return nullptr;
              }
#ifdef LINUX
              func.vk_get_instance_proc_addr = (PFN_vkGetInstanceProcAddr)dlsym(vk_dl_handler, "vkGetInstanceProcAddr");
#endif //LINUX

              if (!func.vk_get_instance_proc_addr)
              {
                     return nullptr;
              }

              PFN_vkGetInstanceProcAddr load = func.vk_get_instance_proc_addr;
              func.vk_create_instance = (PFN_vkCreateInstance)load(NULL, "vkCreateInstance");
              func.vk_enumerate_instance_extension_properties = (PFN_vkEnumerateInstanceExtensionProperties)load(NULL, "vkEnumerateInstanceExtensionProperties");
              func.vk_enumerate_instance_layer_properties = (PFN_vkEnumerateInstanceLayerProperties)load(NULL, "vkEnumerateInstanceLayerProperties");
              func.vk_enumerate_instance_version = (PFN_vkEnumerateInstanceVersion)load(NULL, "vkEnumerateInstanceVersion");

              return vk_dl_handler;
       }

       void unload_vulkan_dl(void *dl_handler)
       {
#ifdef LINUX
              dlclose(dl_handler);
#endif //LINUX
       }

       void load_instance_func(VkInstance instance, InstanceVkFunc &func)
       {
              PFN_vkGetInstanceProcAddr load = func.vk_get_instance_proc_addr;
              func.vk_get_device_proc_addr = (PFN_vkGetDeviceProcAddr)load(instance, "vkGetDeviceProcAddr");
              func.vk_destroy_instance = (PFN_vkDestroyInstance)load(instance, "vkDestroyInstance");
              func.vk_enumerate_physical_devices = (PFN_vkEnumeratePhysicalDevices)load(instance, "vkEnumeratePhysicalDevices");
              func.vk_get_physical_device_features = (PFN_vkGetPhysicalDeviceFeatures)load(instance, "vkGetPhysicalDeviceFeatures");
              func.vk_get_physical_device_format_properties = (PFN_vkGetPhysicalDeviceFormatProperties)load(instance, "vkGetPhysicalDeviceFormatProperties");
              func.vk_get_physical_device_image_format_properties = (PFN_vkGetPhysicalDeviceImageFormatProperties)load(instance, "vkGetPhysicalDeviceImageFormatProperties");
              func.vk_get_physical_device_properties = (PFN_vkGetPhysicalDeviceProperties)load(instance, "vkGetPhysicalDeviceProperties");
              func.vk_get_physical_device_queue_family_properties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)load(instance, "vkGetPhysicalDeviceQueueFamilyProperties");
              func.vk_get_physical_device_memory_properties = (PFN_vkGetPhysicalDeviceMemoryProperties)load(instance, "vkGetPhysicalDeviceMemoryProperties");
              func.vk_create_device = (PFN_vkCreateDevice)load(instance, "vkCreateDevice");
              func.vk_enumerate_instance_extension_properties = (PFN_vkEnumerateInstanceExtensionProperties)load(instance, "vkEnumerateInstanceExtensionProperties");
              func.vk_enumerate_device_extension_properties = (PFN_vkEnumerateDeviceExtensionProperties)load(instance, "vkEnumerateDeviceExtensionProperties");
              func.vk_enumerate_device_layer_properties = (PFN_vkEnumerateDeviceLayerProperties)load(instance, "vkEnumerateDeviceLayerProperties");
              func.vk_get_physical_device_sparse_image_format_properties = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties)load(instance, "vkGetPhysicalDeviceSparseImageFormatProperties");
              func.vk_destroy_surface_khr = (PFN_vkDestroySurfaceKHR)load(instance, "vkDestroySurfaceKHR");
              func.vk_get_physical_device_surface_support_khr = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)load(instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
              func.vk_get_physical_device_surface_capabilities_khr = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)load(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
              func.vk_get_physical_device_surface_formats_khr = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)load(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
              func.vk_get_physical_device_surface_present_modes_khr = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)load(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
#ifdef LINUX
              func.vk_create_surface = (PFN_vkCreateXlibSurfaceKHR)load(instance, "vkCreateXlibSurfaceKHR");
#endif //LINUX
              func.vk_create_debug_utils_messenger_ext = (PFN_vkCreateDebugUtilsMessengerEXT)load(instance, "vkCreateDebugUtilsMessengerEXT");
              func.vk_destroy_debug_utils_messenger_ext = (PFN_vkDestroyDebugUtilsMessengerEXT)load(instance, "vkDestroyDebugUtilsMessengerEXT");
       }

       void load_device_func(VkInstance instance, const InstanceVkFunc &ifunc, VkDevice device, DeviceVkFunc &dfunc)
       {
              PFN_vkGetDeviceProcAddr load = ifunc.vk_get_device_proc_addr;
              dfunc.vk_destroy_device = (PFN_vkDestroyDevice)load(device, "vkDestroyDevice");
              dfunc.vk_get_device_queue = (PFN_vkGetDeviceQueue)load(device, "vkGetDeviceQueue");
              dfunc.vk_queue_submit = (PFN_vkQueueSubmit)load(device, "vkQueueSubmit");
              dfunc.vk_queue_wait_idle = (PFN_vkQueueWaitIdle)load(device, "vkQueueWaitIdle");
              dfunc.vk_device_wait_idle = (PFN_vkDeviceWaitIdle)load(device, "vkDeviceWaitIdle");
              dfunc.vk_allocate_memory = (PFN_vkAllocateMemory)load(device, "vkAllocateMemory");
              dfunc.vk_free_memory = (PFN_vkFreeMemory)load(device, "vkFreeMemory");
              dfunc.vk_map_memory = (PFN_vkMapMemory)load(device, "vkMapMemory");
              dfunc.vk_unmap_memory = (PFN_vkUnmapMemory)load(device, "vkUnmapMemory");
              dfunc.vk_flush_mapped_memory_ranges = (PFN_vkFlushMappedMemoryRanges)load(device, "vkFlushMappedMemoryRanges");
              dfunc.vk_invalidate_mapped_memory_ranges = (PFN_vkInvalidateMappedMemoryRanges)load(device, "vkInvalidateMappedMemoryRanges");
              dfunc.vk_get_device_memory_commitment = (PFN_vkGetDeviceMemoryCommitment)load(device, "vkGetDeviceMemoryCommitment");
              dfunc.vk_bind_buffer_memory = (PFN_vkBindBufferMemory)load(device, "vkBindBufferMemory");
              dfunc.vk_bind_image_memory = (PFN_vkBindImageMemory)load(device, "vkBindImageMemory");
              dfunc.vk_get_buffer_memory_requirements = (PFN_vkGetBufferMemoryRequirements)load(device, "vkGetBufferMemoryRequirements");
              dfunc.vk_get_image_memory_requirements = (PFN_vkGetImageMemoryRequirements)load(device, "vkGetImageMemoryRequirements");
              dfunc.vk_get_image_sparse_memory_requirements = (PFN_vkGetImageSparseMemoryRequirements)load(device, "vkGetImageSparseMemoryRequirements");
              dfunc.vk_queue_bind_sparse = (PFN_vkQueueBindSparse)load(device, "vkQueueBindSparse");
              dfunc.vk_create_fence = (PFN_vkCreateFence)load(device, "vkCreateFence");
              dfunc.vk_destroy_fence = (PFN_vkDestroyFence)load(device, "vkDestroyFence");
              dfunc.vk_reset_fences = (PFN_vkResetFences)load(device, "vkResetFences");
              dfunc.vk_get_fence_status = (PFN_vkGetFenceStatus)load(device, "vkGetFenceStatus");
              dfunc.vk_wait_for_fences = (PFN_vkWaitForFences)load(device, "vkWaitForFences");
              dfunc.vk_create_semaphore = (PFN_vkCreateSemaphore)load(device, "vkCreateSemaphore");
              dfunc.vk_destroy_semaphore = (PFN_vkDestroySemaphore)load(device, "vkDestroySemaphore");
              dfunc.vk_create_event = (PFN_vkCreateEvent)load(device, "vkCreateEvent");
              dfunc.vk_destroy_event = (PFN_vkDestroyEvent)load(device, "vkDestroyEvent");
              dfunc.vk_get_event_status = (PFN_vkGetEventStatus)load(device, "vkGetEventStatus");
              dfunc.vk_set_event = (PFN_vkSetEvent)load(device, "vkSetEvent");
              dfunc.vk_reset_event = (PFN_vkResetEvent)load(device, "vkResetEvent");
              dfunc.vk_create_query_pool = (PFN_vkCreateQueryPool)load(device, "vkCreateQueryPool");
              dfunc.vk_destroy_query_pool = (PFN_vkDestroyQueryPool)load(device, "vkDestroyQueryPool");
              dfunc.vk_get_query_pool_results = (PFN_vkGetQueryPoolResults)load(device, "vkGetQueryPoolResults");
              dfunc.vk_create_buffer = (PFN_vkCreateBuffer)load(device, "vkCreateBuffer");
              dfunc.vk_destroy_buffer = (PFN_vkDestroyBuffer)load(device, "vkDestroyBuffer");
              dfunc.vk_create_buffer_view = (PFN_vkCreateBufferView)load(device, "vkCreateBufferView");
              dfunc.vk_destroy_buffer_view = (PFN_vkDestroyBufferView)load(device, "vkDestroyBufferView");
              dfunc.vk_create_image = (PFN_vkCreateImage)load(device, "vkCreateImage");
              dfunc.vk_destroy_image = (PFN_vkDestroyImage)load(device, "vkDestroyImage");
              dfunc.vk_get_image_subresource_layout = (PFN_vkGetImageSubresourceLayout)load(device, "vkGetImageSubresourceLayout");
              dfunc.vk_create_image_view = (PFN_vkCreateImageView)load(device, "vkCreateImageView");
              dfunc.vk_destroy_image_view = (PFN_vkDestroyImageView)load(device, "vkDestroyImageView");
              dfunc.vk_create_shader_module = (PFN_vkCreateShaderModule)load(device, "vkCreateShaderModule");
              dfunc.vk_destroy_shader_module = (PFN_vkDestroyShaderModule)load(device, "vkDestroyShaderModule");
              dfunc.vk_create_pipeline_cache = (PFN_vkCreatePipelineCache)load(device, "vkCreatePipelineCache");
              dfunc.vk_destroy_pipeline_cache = (PFN_vkDestroyPipelineCache)load(device, "vkDestroyPipelineCache");
              dfunc.vk_get_pipeline_cache_data = (PFN_vkGetPipelineCacheData)load(device, "vkGetPipelineCacheData");
              dfunc.vk_merge_pipeline_caches = (PFN_vkMergePipelineCaches)load(device, "vkMergePipelineCaches");
              dfunc.vk_create_graphics_pipelines = (PFN_vkCreateGraphicsPipelines)load(device, "vkCreateGraphicsPipelines");
              dfunc.vk_create_compute_pipelines = (PFN_vkCreateComputePipelines)load(device, "vkCreateComputePipelines");
              dfunc.vk_destroy_pipeline = (PFN_vkDestroyPipeline)load(device, "vkDestroyPipeline");
              dfunc.vk_create_pipeline_layout = (PFN_vkCreatePipelineLayout)load(device, "vkCreatePipelineLayout");
              dfunc.vk_destroy_pipeline_layout = (PFN_vkDestroyPipelineLayout)load(device, "vkDestroyPipelineLayout");
              dfunc.vk_create_sampler = (PFN_vkCreateSampler)load(device, "vkCreateSampler");
              dfunc.vk_destroy_sampler = (PFN_vkDestroySampler)load(device, "vkDestroySampler");
              dfunc.vk_create_descriptor_set_layout = (PFN_vkCreateDescriptorSetLayout)load(device, "vkCreateDescriptorSetLayout");
              dfunc.vk_destroy_descriptor_set_layout = (PFN_vkDestroyDescriptorSetLayout)load(device, "vkDestroyDescriptorSetLayout");
              dfunc.vk_create_descriptor_pool = (PFN_vkCreateDescriptorPool)load(device, "vkCreateDescriptorPool");
              dfunc.vk_destroy_descriptor_pool = (PFN_vkDestroyDescriptorPool)load(device, "vkDestroyDescriptorPool");
              dfunc.vk_reset_descriptor_pool = (PFN_vkResetDescriptorPool)load(device, "vkResetDescriptorPool");
              dfunc.vk_allocate_descriptor_sets = (PFN_vkAllocateDescriptorSets)load(device, "vkAllocateDescriptorSets");
              dfunc.vk_free_descriptor_sets = (PFN_vkFreeDescriptorSets)load(device, "vkFreeDescriptorSets");
              dfunc.vk_update_descriptor_sets = (PFN_vkUpdateDescriptorSets)load(device, "vkUpdateDescriptorSets");
              dfunc.vk_create_framebuffer = (PFN_vkCreateFramebuffer)load(device, "vkCreateFramebuffer");
              dfunc.vk_destroy_framebuffer = (PFN_vkDestroyFramebuffer)load(device, "vkDestroyFramebuffer");
              dfunc.vk_create_render_pass = (PFN_vkCreateRenderPass)load(device, "vkCreateRenderPass");
              dfunc.vk_destroy_render_pass = (PFN_vkDestroyRenderPass)load(device, "vkDestroyRenderPass");
              dfunc.vk_get_render_area_granularity = (PFN_vkGetRenderAreaGranularity)load(device, "vkGetRenderAreaGranularity");
              dfunc.vk_create_command_pool = (PFN_vkCreateCommandPool)load(device, "vkCreateCommandPool");
              dfunc.vk_destroy_command_pool = (PFN_vkDestroyCommandPool)load(device, "vkDestroyCommandPool");
              dfunc.vk_reset_command_pool = (PFN_vkResetCommandPool)load(device, "vkResetCommandPool");
              dfunc.vk_allocate_command_buffers = (PFN_vkAllocateCommandBuffers)load(device, "vkAllocateCommandBuffers");
              dfunc.vk_free_command_buffers = (PFN_vkFreeCommandBuffers)load(device, "vkFreeCommandBuffers");
              dfunc.vk_begin_command_buffer = (PFN_vkBeginCommandBuffer)load(device, "vkBeginCommandBuffer");
              dfunc.vk_end_command_buffer = (PFN_vkEndCommandBuffer)load(device, "vkEndCommandBuffer");
              dfunc.vk_reset_command_buffer = (PFN_vkResetCommandBuffer)load(device, "vkResetCommandBuffer");
              dfunc.vk_cmd_bind_pipeline = (PFN_vkCmdBindPipeline)load(device, "vkCmdBindPipeline");
              dfunc.vk_cmd_set_viewport = (PFN_vkCmdSetViewport)load(device, "vkCmdSetViewport");
              dfunc.vk_cmd_set_scissor = (PFN_vkCmdSetScissor)load(device, "vkCmdSetScissor");
              dfunc.vk_cmd_set_line_width = (PFN_vkCmdSetLineWidth)load(device, "vkCmdSetLineWidth");
              dfunc.vk_cmd_set_depth_bias = (PFN_vkCmdSetDepthBias)load(device, "vkCmdSetDepthBias");
              dfunc.vk_cmd_set_blend_constants = (PFN_vkCmdSetBlendConstants)load(device, "vkCmdSetBlendConstants");
              dfunc.vk_cmd_set_depth_bounds = (PFN_vkCmdSetDepthBounds)load(device, "vkCmdSetDepthBounds");
              dfunc.vk_cmd_set_stencil_compare_mask = (PFN_vkCmdSetStencilCompareMask)load(device, "vkCmdSetStencilCompareMask");
              dfunc.vk_cmd_set_stencil_write_mask = (PFN_vkCmdSetStencilWriteMask)load(device, "vkCmdSetStencilWriteMask");
              dfunc.vk_cmd_set_stencil_reference = (PFN_vkCmdSetStencilReference)load(device, "vkCmdSetStencilReference");
              dfunc.vk_cmd_bind_descriptor_sets = (PFN_vkCmdBindDescriptorSets)load(device, "vkCmdBindDescriptorSets");
              dfunc.vk_cmd_bind_index_buffer = (PFN_vkCmdBindIndexBuffer)load(device, "vkCmdBindIndexBuffer");
              dfunc.vk_cmd_bind_vertex_buffers = (PFN_vkCmdBindVertexBuffers)load(device, "vkCmdBindVertexBuffers");
              dfunc.vk_cmd_draw = (PFN_vkCmdDraw)load(device, "vkCmdDraw");
              dfunc.vk_cmd_draw_indexed = (PFN_vkCmdDrawIndexed)load(device, "vkCmdDrawIndexed");
              dfunc.vk_cmd_draw_indirect = (PFN_vkCmdDrawIndirect)load(device, "vkCmdDrawIndirect");
              dfunc.vk_cmd_draw_indexed_indirect = (PFN_vkCmdDrawIndexedIndirect)load(device, "vkCmdDrawIndexedIndirect");
              dfunc.vk_cmd_dispatch = (PFN_vkCmdDispatch)load(device, "vkCmdDispatch");
              dfunc.vk_cmd_dispatch_indirect = (PFN_vkCmdDispatchIndirect)load(device, "vkCmdDispatchIndirect");
              dfunc.vk_cmd_copy_buffer = (PFN_vkCmdCopyBuffer)load(device, "vkCmdCopyBuffer");
              dfunc.vk_cmd_copy_image = (PFN_vkCmdCopyImage)load(device, "vkCmdCopyImage");
              dfunc.vk_cmd_blit_image = (PFN_vkCmdBlitImage)load(device, "vkCmdBlitImage");
              dfunc.vk_cmd_copy_buffer_to_image = (PFN_vkCmdCopyBufferToImage)load(device, "vkCmdCopyBufferToImage");
              dfunc.vk_cmd_copy_image_to_buffer = (PFN_vkCmdCopyImageToBuffer)load(device, "vkCmdCopyImageToBuffer");
              dfunc.vk_cmd_update_buffer = (PFN_vkCmdUpdateBuffer)load(device, "vkCmdUpdateBuffer");
              dfunc.vk_cmd_fill_buffer = (PFN_vkCmdFillBuffer)load(device, "vkCmdFillBuffer");
              dfunc.vk_cmd_clear_color_image = (PFN_vkCmdClearColorImage)load(device, "vkCmdClearColorImage");
              dfunc.vk_cmd_clear_depth_stencil_image = (PFN_vkCmdClearDepthStencilImage)load(device, "vkCmdClearDepthStencilImage");
              dfunc.vk_cmd_clear_attachments = (PFN_vkCmdClearAttachments)load(device, "vkCmdClearAttachments");
              dfunc.vk_cmd_resolve_image = (PFN_vkCmdResolveImage)load(device, "vkCmdResolveImage");
              dfunc.vk_cmd_set_event = (PFN_vkCmdSetEvent)load(device, "vkCmdSetEvent");
              dfunc.vk_cmd_reset_event = (PFN_vkCmdResetEvent)load(device, "vkCmdResetEvent");
              dfunc.vk_cmd_wait_events = (PFN_vkCmdWaitEvents)load(device, "vkCmdWaitEvents");
              dfunc.vk_cmd_pipeline_barrier = (PFN_vkCmdPipelineBarrier)load(device, "vkCmdPipelineBarrier");
              dfunc.vk_cmd_begin_query = (PFN_vkCmdBeginQuery)load(device, "vkCmdBeginQuery");
              dfunc.vk_cmd_end_query = (PFN_vkCmdEndQuery)load(device, "vkCmdEndQuery");
              dfunc.vk_cmd_reset_query_pool = (PFN_vkCmdResetQueryPool)load(device, "vkCmdResetQueryPool");
              dfunc.vk_cmd_write_timestamp = (PFN_vkCmdWriteTimestamp)load(device, "vkCmdWriteTimestamp");
              dfunc.vk_cmd_copy_query_pool_results = (PFN_vkCmdCopyQueryPoolResults)load(device, "vkCmdCopyQueryPoolResults");
              dfunc.vk_cmd_push_constants = (PFN_vkCmdPushConstants)load(device, "vkCmdPushConstants");
              dfunc.vk_cmd_begin_render_pass = (PFN_vkCmdBeginRenderPass)load(device, "vkCmdBeginRenderPass");
              dfunc.vk_cmd_next_subpass = (PFN_vkCmdNextSubpass)load(device, "vkCmdNextSubpass");
              dfunc.vk_cmd_end_render_pass = (PFN_vkCmdEndRenderPass)load(device, "vkCmdEndRenderPass");
              dfunc.vk_cmd_execute_commands = (PFN_vkCmdExecuteCommands)load(device, "vkCmdExecuteCommands");
              dfunc.vk_create_swapchain_khr = (PFN_vkCreateSwapchainKHR)load(device, "vkCreateSwapchainKHR");
              dfunc.vk_destroy_swapchain_khr = (PFN_vkDestroySwapchainKHR)load(device, "vkDestroySwapchainKHR");
              dfunc.vk_get_swaphain_images_khr = (PFN_vkGetSwapchainImagesKHR)load(device, "vkGetSwapchainImagesKHR");
              dfunc.vk_queue_present_khr = (PFN_vkQueuePresentKHR)load(device, "vkQueuePresentKHR");
              dfunc.vk_acquire_next_image_khr = (PFN_vkAcquireNextImageKHR)load(device, "vkAcquireNextImageKHR");
       }

       bool check_instance_layers(const InstanceVkFunc &func, const char **required_layers, uint32_t layer_count)
       {
              uint32_t count = 0;
              vk_assert(func.vk_enumerate_instance_layer_properties(&count, NULL));
              std::vector<VkLayerProperties> properties(count);
              vk_assert(func.vk_enumerate_instance_layer_properties(&count, properties.data()));

              for (uint32_t i = 0; i < layer_count; ++i)
              {
                     const char *required_layer = required_layers[i];
                     bool found = false;
                     for (size_t j = 0; j < count; ++j)
                     {
                            if (strcmp(properties[j].layerName, required_layer) == 0)
                            {
                                   found = true;
                                   break;
                            }
                     }
                     if (!found)
                     {
                            return false;
                     }
              }
              return true;
       }

       bool check_instance_extensions(const InstanceVkFunc &func, const char **required_extensions, uint32_t extensions_count)
       {
              uint32_t count = 0;
              vk_assert(func.vk_enumerate_instance_extension_properties(NULL, &count, NULL));
              std::vector<VkExtensionProperties> properties(count);
              vk_assert(func.vk_enumerate_instance_extension_properties(NULL, &count, properties.data()));

              for (uint32_t i = 0; i < extensions_count; ++i)
              {
                     const char *required_extension = required_extensions[i];
                     bool found = false;
                     for (size_t j = 0; j < count; ++j)
                     {
                            if (strcmp(properties[j].extensionName, required_extension) == 0)
                            {
                                   found = true;
                                   break;
                            }
                     }
                     if (!found)
                     {
                            return false;
                     }
              }
              return true;
       }

       bool check_physical_device_extensions(VkPhysicalDevice physical_device, const InstanceVkFunc &func, const char **required_extensions, uint32_t extension_count)
       {
              uint32_t count = 0;
              vk_assert(func.vk_enumerate_device_extension_properties(physical_device, NULL, &count, NULL));
              std::vector<VkExtensionProperties> extensions(count);
              vk_assert(func.vk_enumerate_device_extension_properties(physical_device, NULL, &count, extensions.data()));

              for (uint32_t i = 0; i < extension_count; ++i)
              {
                     const char *required_extension = required_extensions[i];
                     bool found = false;
                     for (uint32_t j = 0; j < count; ++j)
                     {
                            if (strcmp(extensions[j].extensionName, required_extension) == 0)
                            {
                                   found = true;
                                   break;
                            }
                     }
                     if (!found)
                     {
                            return false;
                     }
              }
              return true;
       }

       VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR> &surface_formats)
       {
              for (VkSurfaceFormatKHR surface_format : surface_formats)
              {
                     if (surface_format.format == VK_FORMAT_R8G8B8A8_UNORM && surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                     {
                            return surface_format;
                     }
              }
              return surface_formats[0];
       }

       VkExtent2D choose_extent(VkSurfaceCapabilitiesKHR surface_capabilities, uint32_t width, uint32_t height)
       {
              if (surface_capabilities.currentExtent.width != UINT32_MAX && surface_capabilities.currentExtent.height != UINT32_MAX)
              {
                     return surface_capabilities.currentExtent;
              }
              VkExtent2D extent;
              extent.width = vk_max(surface_capabilities.minImageExtent.width, vk_min(width, surface_capabilities.maxImageExtent.width));
              extent.height = vk_max(surface_capabilities.minImageExtent.height, vk_min(height, surface_capabilities.maxImageExtent.height));
              return extent;
       }

       VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR> &present_modes)
       {
              for (VkPresentModeKHR present_mode : present_modes)
              {
                     if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
                     {
                            return present_mode;
                     }
              }
              return VK_PRESENT_MODE_FIFO_KHR;
       }

       std::vector<GPU> enumerate_gpus(VkInstance instance, const InstanceVkFunc &func, VkSurfaceKHR surface, uint32_t width, uint32_t height)
       {
              uint32_t count = 0;
              vk_assert(func.vk_enumerate_physical_devices(instance, &count, NULL));
              std::vector<VkPhysicalDevice> physical_devices(count);
              vk_assert(func.vk_enumerate_physical_devices(instance, &count, physical_devices.data()));
              std::vector<GPU> selected;
              for (uint32_t i = 0; i < count; ++i)
              {
                     const uint32_t extension_count = 1;
                     const char *required_extension[1] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
                     if (!check_physical_device_extensions(physical_devices[i], func, required_extension, extension_count))
                     {
                            continue;
                     }
                     GPU curr_gpu = {physical_devices[i], UINT32_MAX, UINT32_MAX};
                     uint32_t queue_count = 0;
                     func.vk_get_physical_device_queue_family_properties(curr_gpu.vk_physical_device, &queue_count, NULL);
                     std::vector<VkQueueFamilyProperties> queue_family_properties(queue_count);
                     func.vk_get_physical_device_queue_family_properties(curr_gpu.vk_physical_device, &queue_count, queue_family_properties.data());
                     for (uint32_t j = 0; j < queue_count; ++j)
                     {
                            if (queue_family_properties[j].queueCount > 0 && (queue_family_properties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
                            {
                                   curr_gpu.graphics_queue_index = j;
                            }
                            VkBool32 pres_supported = VK_FALSE;
                            vk_assert(func.vk_get_physical_device_surface_support_khr(curr_gpu.vk_physical_device, j, surface, &pres_supported));
                            if (pres_supported && queue_family_properties[j].queueCount > 0)
                            {
                                   curr_gpu.present_queue_index = j;
                            }
                            if (curr_gpu.graphics_queue_index == curr_gpu.present_queue_index && curr_gpu.graphics_queue_index != UINT32_MAX)
                            {
                                   break;
                            }
                     }
                     if (curr_gpu.graphics_queue_index != UINT32_MAX && curr_gpu.present_queue_index != UINT32_MAX)
                     {
                            selected.push_back(curr_gpu);
                     }
              }
              for (GPU &curr_gpu : selected)
              {
                     func.vk_get_physical_device_properties(curr_gpu.vk_physical_device, &curr_gpu.vk_properties);
                     curr_gpu.has_same_queue = (curr_gpu.graphics_queue_index == curr_gpu.present_queue_index);
                     func.vk_get_physical_device_features(curr_gpu.vk_physical_device, &curr_gpu.vk_features);
                     {
                            uint32_t count = 0;
                            vk_assert(func.vk_get_physical_device_surface_present_modes_khr(curr_gpu.vk_physical_device, surface, &count, NULL));
                            std::vector<VkPresentModeKHR> present_modes(count);
                            vk_assert(func.vk_get_physical_device_surface_present_modes_khr(curr_gpu.vk_physical_device, surface, &count, present_modes.data()));
                            curr_gpu.vk_present_mode = choose_present_mode(present_modes);
                     }
                     {
                            uint32_t count = 0;
                            vk_assert(func.vk_get_physical_device_surface_formats_khr(curr_gpu.vk_physical_device, surface, &count, NULL));
                            std::vector<VkSurfaceFormatKHR> surface_formats(count);
                            vk_assert(func.vk_get_physical_device_surface_formats_khr(curr_gpu.vk_physical_device, surface, &count, surface_formats.data()));
                            curr_gpu.vk_surface_format = choose_surface_format(surface_formats);
                     }
                     vk_assert(func.vk_get_physical_device_surface_capabilities_khr(curr_gpu.vk_physical_device, surface, &curr_gpu.vk_surface_capabilities));
                     curr_gpu.vk_extent = choose_extent(curr_gpu.vk_surface_capabilities, width, height);
              }
              return selected;
       }

       VkShaderModule create_shader_module(VkDevice device, const DeviceVkFunc &func, size_t size, const void *code)
       {
              VkShaderModuleCreateInfo module_info = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, NULL, 0, size, (uint32_t *)code};
              VkShaderModule module;
              vk_assert(func.vk_create_shader_module(device, &module_info, nullptr, &module));
              return module;
       }

       VkImageView create_image_view(
           VkDevice device, const DeviceVkFunc &func, VkImage image, VkImageViewType image_view_type, VkFormat format, VkComponentMapping components,
           VkImageSubresourceRange subresource_range)
       {
              VkImageViewCreateInfo image_view_info = init::image_view(image, image_view_type, format, components, subresource_range);
              VkImageView image_view;
              vk_assert(func.vk_create_image_view(device, &image_view_info, NULL, &image_view));
              return image_view;
       }

       VkImage create_image(
           VkDevice device, const DeviceVkFunc &func, VkImageCreateFlags flags, VkFormat format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mip_levels, uint32_t array_layers,
           VkSampleCountFlagBits sample_count, VkImageTiling tiling, VkImageUsageFlags usage, VkSharingMode sharing_mode, uint32_t queue_family_index_count,
           const uint32_t *queue_family_indices, VkImageLayout initial_layout)
       {
              VkImageCreateInfo image_info = init::image(
                  flags, format, width, height, depth, mip_levels, array_layers, sample_count, tiling, usage, sharing_mode, queue_family_index_count, queue_family_indices, initial_layout);
              VkImage image;
              vk_assert(func.vk_create_image(device, &image_info, NULL, &image));
              return image;
       }

       VkDescriptorSetLayout create_desc_set_layout(VkDevice device, const DeviceVkFunc &func, uint32_t binding_count, const VkDescriptorSetLayoutBinding *bindings)
       {
              VkDescriptorSetLayoutCreateInfo desc_set_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, NULL, 0, binding_count, bindings};
              VkDescriptorSetLayout desc_set_layout;
              vk_assert(func.vk_create_descriptor_set_layout(device, &desc_set_info, NULL, &desc_set_layout));
              return desc_set_layout;
       }

       VkDescriptorPool create_desc_pool(VkDevice device, const DeviceVkFunc &func, uint32_t max_sets, uint32_t pool_size_count, const VkDescriptorPoolSize *pool_sizes)
       {
              VkDescriptorPoolCreateInfo desc_pool_info =
                  {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, NULL, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, max_sets, pool_size_count, pool_sizes};
              VkDescriptorPool desc_pool;
              vk_assert(func.vk_create_descriptor_pool(device, &desc_pool_info, NULL, &desc_pool));
              return desc_pool;
       }

       void alloc_command_buffers(VkDevice device, const DeviceVkFunc &func, VkCommandPool command_pool, VkCommandBufferLevel level, uint32_t count, VkCommandBuffer *command_buffers)
       {
              VkCommandBufferAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr, command_pool, level, count};
              vk_assert(func.vk_allocate_command_buffers(device, &alloc_info, command_buffers));
       }

       void begin_command_buffer(VkDevice device, const DeviceVkFunc &func, VkCommandBufferUsageFlags usage, VkCommandBuffer command_buffer)
       {
              VkCommandBufferBeginInfo begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, usage, nullptr};
              vk_assert(func.vk_begin_command_buffer(command_buffer, &begin_info));
       }

       void end_launch_command_buffer(VkDevice device, const DeviceVkFunc &func, VkQueue queue, VkFence fence, VkCommandBuffer command_buffer)
       {
              vk_assert(func.vk_end_command_buffer(command_buffer));
              VkSubmitInfo info = {};
              info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
              info.commandBufferCount = 1;
              info.pCommandBuffers = &command_buffer;
              vk_assert(func.vk_queue_submit(queue, 1, &info, fence));
       }

       void Buffer::create(VkDevice device, const DeviceVkFunc &func, VmaAllocator vma_allocator, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage vma_usage)
       {
              VkBufferCreateInfo buffer_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, NULL, 0, size, usage, VK_SHARING_MODE_EXCLUSIVE, 0, NULL};
              vk_assert(func.vk_create_buffer(device, &buffer_info, NULL, &vkh));
              VmaAllocationCreateInfo alloc_info = {0};
              alloc_info.usage = vma_usage;
              vk_assert(vmaAllocateMemoryForBuffer(vma_allocator, vkh, &alloc_info, &allocation, NULL));
              vk_assert(vmaBindBufferMemory(vma_allocator, allocation, vkh));
       }

       void *Buffer::create_mapped(VkDevice device, const DeviceVkFunc &func, VmaAllocator vma_allocator, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage vma_usage)
       {
              create(device, func, vma_allocator, size, usage, vma_usage);
              return map(vma_allocator);
       }

       void Buffer::destroy(VkDevice device, const DeviceVkFunc &func, VmaAllocator vma_allocator)
       {
              if (mapped)
              {
                     vmaUnmapMemory(vma_allocator, allocation);
              }
              vmaFreeMemory(vma_allocator, allocation);
              func.vk_destroy_buffer(device, vkh, NULL);
       }

       void *Buffer::map(VmaAllocator vma_allocator)
       {
              vk_assert(vmaMapMemory(vma_allocator, allocation, &mapped));
              return mapped;
       }

       void Buffer::unmap(VmaAllocator vma_allocator)
       {
              vmaUnmapMemory(vma_allocator, allocation);
              mapped = nullptr;
       }

       void Image::create(
           VkDevice device, const DeviceVkFunc &func, VmaAllocator vma_allocator, VkImageCreateFlags flags, VkImageViewType image_view_type, VkFormat format, uint32_t width,
           uint32_t height, uint32_t depth, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits sample_count, VkImageTiling tiling, VkImageUsageFlags usage,
           VkComponentMapping components, VkImageSubresourceRange subresource_range, VkSharingMode sharing_mode,
           uint32_t queue_family_index_count, const uint32_t *queue_family_indices, VkImageLayout initial_layout)
       {
              vkh = create_image(
                  device, func, flags, format, width, height, depth, mip_levels, array_layers, sample_count, tiling, usage, sharing_mode, queue_family_index_count,
                  queue_family_indices, initial_layout);
              VmaAllocationCreateInfo alloc_info = {0};
              alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
              vk_assert(vmaAllocateMemoryForImage(vma_allocator, vkh, &alloc_info, &allocation, NULL));
              vk_assert(vmaBindImageMemory(vma_allocator, allocation, vkh));
              vk_view = create_image_view(device, func, vkh, image_view_type, format, components, subresource_range);
       }

       void Image::destroy(VkDevice device, const DeviceVkFunc &func, VmaAllocator vma_allocator)
       {
              func.vk_destroy_image_view(device, vk_view, NULL);
              vmaFreeMemory(vma_allocator, allocation);
              func.vk_destroy_image(device, vkh, NULL);
       }

       /*void DescriptorSetPool::create(VkDevice device, const DeviceVkFunc &func, const DescriptorSetPoolInfo &desc_pool_set_info, VkDescriptorSetLayout desc_set_layout)
       {
              std::vector<VkDescriptorPoolSize> desc_pool_sizes;
              if(desc_pool_set_info.uniform_buffer_count > 0)
              {
                     desc_pool_sizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, desc_pool_set_info.uniform_buffer_count * desc_pool_set_info.set_count});
              }
              if(desc_pool_set_info.combined_image_sample_count > 0)
              {
                     desc_pool_sizes.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, desc_pool_set_info.combined_image_sample_count * desc_pool_set_info.set_count});
              }
              vk_desc_pool = create_desc_pool(device, func, desc_pool_set_info.set_count, (uint32_t)desc_pool_sizes.size(), desc_pool_sizes.data());
              std::vector<VkDescriptorSetLayout> layouts(desc_pool_set_info.set_count, desc_set_layout);
              size_t node_count = desc_pool_set_info.set_count + 1;
              std::vector<VkDescriptorSet> desc_sets(node_count);
              VkDescriptorSetAllocateInfo desc_alloc_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, nullptr, vk_desc_pool, desc_pool_set_info.set_count, layouts.data()};
              vk_assert(func.vk_allocate_descriptor_sets(device, &desc_alloc_info, desc_sets.data() + 1));
              desc_sets[0] = VK_NULL_HANDLE;
              head = utl::create_node_list_next<VkDescriptorSet>(node_count, desc_sets.data() + 1);
       }

       void DescriptorSetPool::destroy(VkDevice device, const DeviceVkFunc &func)
       {
              if(!head->is_front())
              {
                     throw std::runtime_error("memory leak in decriptors_set_pool");
              }
              utl::delete_all_next_nodes(head);
              utl::delete_node(head);
              func.vk_destroy_descriptor_pool(device, vk_desc_pool, NULL);
       }

       utl::node<VkDescriptorSet> DescriptorSetPool::alloc()
       {
              utl::node<VkDescriptorSet> desc_set = head->next();
              desc_set->disconnect();
              head->connect_previous(desc_set);
              return desc_set;
       }

       void DescriptorSetPool::free(utl::node<VkDescriptorSet> desc_set)
       {
              desc_set->disconnect();
              head->connect_next(desc_set);
       }*/

       size_t sizeof_glsl(GlslType type)
       {
              switch (type)
              {
              case GLSL_INT:
                     return sizeof(int);
              case GLSL_FLOAT:
                     return sizeof(float);
              case GLSL_VEC2:
                     return sizeof(float) * 2;
              case GLSL_VEC3:
                     return sizeof(float) * 3;
              case GLSL_VEC4:
                     return sizeof(float) * 4;
              case GLSL_IVEC2:
                     return sizeof(int) * 2;
              case GLSL_IVEC3:
                     return sizeof(int) * 3;
              case GLSL_IVEC4:
                     return sizeof(int) * 4;
              case GLSL_MAT3:
                     return sizeof(float) * 9;
              case GLSL_MAT4:
                     return sizeof(float) * 16;
              default:
                     throw std::runtime_error("glsl type is not valid");
              }
       }

       VkFormat format_glsl(GlslType type)
       {
              switch (type)
              {
              case GLSL_INT:
                     return VK_FORMAT_R32_SINT;
              case GLSL_FLOAT:
                     return VK_FORMAT_R32_SFLOAT;
              case GLSL_VEC2:
                     return VK_FORMAT_R32G32_SFLOAT;
              case GLSL_VEC3:
                     return VK_FORMAT_R32G32B32_SFLOAT;
              case GLSL_VEC4:
                     return VK_FORMAT_R32G32B32A32_SFLOAT;
              case GLSL_IVEC2:
                     return VK_FORMAT_R32G32_SINT;
              case GLSL_IVEC3:
                     return VK_FORMAT_R32G32B32_SINT;
              case GLSL_IVEC4:
                     return VK_FORMAT_R32G32B32A32_SINT;
              case GLSL_MAT3:
                     return VK_FORMAT_R32G32B32_SFLOAT;
              case GLSL_MAT4:
                     return VK_FORMAT_R32G32B32A32_SFLOAT;
              default:
                     throw std::runtime_error("glsl type is not valid");
              }
       }

       VkPipeline create_pipeline(VkDevice device, const DeviceVkFunc &func, const PipelineInfo &pip_info)
       {
              auto vert_info = vkt::init::pip_shader_stage_info(VK_SHADER_STAGE_VERTEX_BIT, pip_info.vertex_module, "main");
              auto frag_info = vkt::init::pip_shader_stage_info(VK_SHADER_STAGE_FRAGMENT_BIT, pip_info.fragment_module, "main");
              VkPipelineShaderStageCreateInfo shad_infos[2] = {vert_info, frag_info};
              std::vector<VkVertexInputBindingDescription> vert_input_bindings(pip_info.vertex_input_binding_count);
              std::vector<VkVertexInputAttributeDescription> vert_input_attributes;
              for (size_t i = 0; i < vert_input_bindings.size(); ++i)
              {
                     vert_input_bindings[i].binding = i;
                     vert_input_bindings[i].inputRate = pip_info.vertex_input_bindings[i].input_rate;
                     uint32_t offset = 0;
                     for (size_t j = 0; j < pip_info.vertex_input_bindings[i].attribute_count; ++j)
                     {
                            VkVertexInputAttributeDescription attribute;
                            attribute.location = vert_input_attributes.size();
                            attribute.binding = i;
                            attribute.format = format_glsl(pip_info.vertex_input_bindings[i].attributes[j]);
                            attribute.offset = offset;
                            vert_input_attributes.push_back(attribute);
                            offset += sizeof_glsl(pip_info.vertex_input_bindings[i].attributes[j]);
                     }
                     vert_input_bindings[i].stride = offset;
              }
              auto vertex_input = vkt::init::pip_vertex_input_info((uint32_t)vert_input_bindings.size(), vert_input_bindings.data(), (uint32_t)vert_input_attributes.size(), vert_input_attributes.data());
              auto input_asm = vkt::init::pip_input_assembly_info(pip_info.topology, VK_FALSE);
              auto viewport_state = vkt::init::pip_viewport_info(1, &pip_info.viewport, 1, &pip_info.scissor);
              auto rasterizer = vkt::init::pip_rasterization_info(VK_FALSE, VK_FALSE, pip_info.polygon_mode, pip_info.cull_mode, pip_info.front_face, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f);
              auto multisampling = vkt::init::pip_multisample_info(pip_info.rasterization_samples, VK_FALSE, 0.0f, nullptr, VK_FALSE, VK_FALSE);
              VkPipelineColorBlendAttachmentState color_blend_attachment{};
              color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
              color_blend_attachment.blendEnable = VK_TRUE;
              color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
              color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
              color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
              color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
              color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
              color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
              auto color_blending = vkt::init::pip_color_blend_info(VK_FALSE, VK_LOGIC_OP_AND, 1, &color_blend_attachment);
              auto depth_stencil = vkt::init::pip_depth_stencil_info(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS, VK_FALSE, VK_FALSE, {}, {}, 0.0f, 1.0f);
              if (!pip_info.enable_depth_test)
              {
                     depth_stencil.depthTestEnable = VK_FALSE;
              }
              VkGraphicsPipelineCreateInfo pipeline_info = {
                  VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, nullptr, 0, 2, shad_infos, &vertex_input, &input_asm, nullptr, &viewport_state, &rasterizer, &multisampling,
                  &depth_stencil, &color_blending, nullptr, pip_info.pipeline_layout, pip_info.render_pass, 0, VK_NULL_HANDLE, 0};
              VkPipeline pipeline;
              vk_assert(func.vk_create_graphics_pipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline));
              return pipeline;
       }

       VkPipelineLayout create_pipeline_layout(VkDevice device, const DeviceVkFunc &func, const PipelineLayoutInfo &pip_lay_info, VkDescriptorSetLayout *desc_set_layouts)
       {
              for (uint32_t i = 0; i < pip_lay_info.desc_set_info_count; ++i)
              {
                     const DescriptorSetInfo &curr_set = pip_lay_info.desc_set_infos[i];
                     std::vector<VkDescriptorSetLayoutBinding> bindings(curr_set.desc_set_binding_info_count);
                     for (size_t j = 0; j < curr_set.desc_set_binding_info_count; ++j)
                     {
                            bindings[j].binding = (uint32_t)j;
                            bindings[j].descriptorCount = curr_set.desc_set_binding_infos[j].desc_count;
                            bindings[j].descriptorType = curr_set.desc_set_binding_infos[j].desc_type;
                            bindings[j].pImmutableSamplers = nullptr;
                            bindings[j].stageFlags = curr_set.desc_set_binding_infos[j].shader_stages;
                     }
                     desc_set_layouts[i] = create_desc_set_layout(device, func, (uint32_t)bindings.size(), bindings.data());
              }
              VkPipelineLayoutCreateInfo pipeline_layout_info = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, nullptr, 0, pip_lay_info.desc_set_info_count, desc_set_layouts, 0, nullptr};
              VkPipelineLayout pipeline_layout;
              vk_assert(func.vk_create_pipeline_layout(device, &pipeline_layout_info, NULL, &pipeline_layout));
              return pipeline_layout;
       }
}