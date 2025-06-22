/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-util.hpp
 * @brief The header file for generic utilities.
 */

#ifndef DMN_UTIL_HPP_

#define DMN_UTIL_HPP_

#include <algorithm>
#include <string>
#include <string_view>

namespace dmn {

/**
 * @brief The method increases the value by 1 and wraps it around to 1 if
 *        the increased number wraps around to zero or negative value.
 *
 * @param value The value to be plus 1 and returned
 *
 * @return The value plus 1 or minimum value 1
 */
template <typename T> inline T incrementByOne(T value) {
  return std::max<T>(1, value + 1);
}

/**
 * @brief The method compares two string values case insensitive or sensitive.
 *
 * @param str1            The 1st string value
 * @param str2            The 2nd string value
 * @param caseInsensitive True if strings are compared in case insensitive,
 *                        or false otherwise
 *
 * @return True if the string values are same, or false otherwise
 */
inline bool stringCompare(const std::string_view str1,
                          const std::string_view str2,
                          bool caseInsensitive = true) {
  std::string strValue1 = std::string(str1);
  std::string strValue2 = std::string(str2);

  if (caseInsensitive) {
    std::transform(strValue1.begin(), strValue1.end(), strValue1.begin(),
                   ::tolower);
    std::transform(strValue2.begin(), strValue2.end(), strValue2.begin(),
                   ::tolower);
  }

  return strValue1 == strValue2;
}

} // namespace dmn

#endif // DMN_UTIL_HPP_
