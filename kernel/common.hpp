#pragma once
#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#define ORION "Orion"
#define ORN_VERS_MAJ 1
#define ORN_VERS_MIN 0

#if defined(__linux__) && !defined(__ANDROID__)
#define LINUX
#include <dlfcn.h>
#include <X11/Xlib.h>
#else
#error "no other platforms supported yet"
#endif

/*STD*/
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <array>
#include <fstream>
#include <functional>
#include <utility>
#include <exception>
#include <limits>
#include <iostream>

/*UTILS*/
#include "utils/utils.hpp"

#endif //__COMMON_HPP__