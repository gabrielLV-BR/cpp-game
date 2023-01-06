// taken from
// https://github.com/gabrielLV-BR/cpp-vulkan/blob/main/src/utils/debug.hpp
// stole from myself

#pragma once

#ifndef NDEBUG 

  #include <vulkan/vulkan.h>

  #include <stdio.h>
  #include <error.h>
  #include <exception>
  #include <stdexcept>

  // Generic form
  #define ASSERT(x,y) if(!(x)) { \
    fprintf(stderr, "\n\033[0;31m[ASSERT FAILED]\33[0m { %s } %s\n\t- at %s:%d\n\n", #x, y, __FILE__, __LINE__); \
    throw std::runtime_error("Check error log"); \
  }

  // Vulkan form
  #define VK_ASSERT(x) if(x != VK_SUCCESS) { \
    fprintf(stderr, "\n\033[0;31m[ERROR]\33[0m <%s> at %s:%d\n\n", #x, __FILE__, __LINE__); \
    throw std::runtime_error("Check error log"); \
  }
#else

  #define VK_ASSERT(x) x
  #define ASSERT(x) x

#endif