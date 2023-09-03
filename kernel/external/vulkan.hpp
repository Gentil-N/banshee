#pragma once
#ifndef __VULKAN_HPP__
#define __VULKAN_HPP__

#include "../common.hpp"

#define VK_NO_PROTOTYPES
#ifdef LINUX
#define VK_USE_PLATFORM_XLIB_KHR
#endif //LINUX

#include "vulkan-headers/include/vulkan/vulkan.h"

#endif //__VULKAN_HPP__