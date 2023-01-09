#pragma once

#ifndef NDEBUG 

  #include <stdio.h>

  #define LOG(x, args...) \
    printf(x, ##args); printf("\n");
  
  #define LOG_AT(x, args...) \
    printf(x, ##args); printf("\n\t- at %s:%s\n", __FILE__, __LINE__);

#else
  #define LOG(x)
  #define LOG_AT(x)
#endif