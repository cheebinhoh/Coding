/**
 * Copyright ©  2025 Chee Bin HOH. All rights reserved.
 */

#ifndef HAL_DEBUG_HPP_HAVE_SEEN

#define HAL_DEBUG_HPP_HAVE_SEEN

#include <iostream>

#ifdef NDEBUG
#define HAL_DEBUG_PRINT(printstmt)                                             \
  do {                                                                         \
    ;                                                                          \
  } while (false)
#else
#define HAL_DEBUG_PRINT(printstmt)                                             \
  do {                                                                         \
    (printstmt);                                                               \
  } while (false)
#endif

#endif /* HAL_DEBUG_HPP_HAVE_SEEN */
