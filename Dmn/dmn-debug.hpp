/**
 * Copyright ©  2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-debug.hpp
 * @brief The header file for dmn-debug which implements debug print macro
 *        that can be enable or disable through CPP define NDEBUG.
 */

#ifndef DMN_DEBUG_HPP_

#define DMN_DEBUG_HPP_

#include <iostream>

#ifdef NDEBUG
#define DMN_DEBUG_PRINT(print_stmt)                                            \
  do {                                                                         \
  } while (false)
#else
#define DMN_DEBUG_PRINT(print_stmt)                                            \
  do {                                                                         \
    (print_stmt);                                                              \
  } while (false)
#endif

#endif // DMN_DEBUG_HPP_
