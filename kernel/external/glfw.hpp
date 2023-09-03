#pragma once
#ifndef __GLFW_HPP__
#define __GLFW_HPP__

#include "../common.hpp"

#ifdef LINUX
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include "glfw/include/GLFW/glfw3.h"
#include "glfw/include/GLFW/glfw3native.h"

#endif //__GLFW_HPP__