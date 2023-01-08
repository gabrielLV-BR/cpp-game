#pragma once

#ifndef NDEBUG 

  #include <iostream>
  #define LOG(x) \
    std::cout << "\"" << x << "\"\n\t- at " << __FILE__ << ":" << __LINE__ << "\n";

#else
  #define LOG(x)
#endif