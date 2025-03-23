/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#ifndef DMN_UTIL_HPP_HAVE_SEEN

#define DMN_UTIL_HPP_HAVE_SEEN

#include <algorithm>

namespace Dmn {

template <typename T> inline T incrementByOne(T value) {
  return std::max<T>(1, value + 1);
}

} /* End of namespace Dmn */

#endif /* end of DMN_UTIL_HPP_HAVE_ SEEN */
