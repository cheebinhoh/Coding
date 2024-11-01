#include "template-class.hpp"

template <typename T> const T &SampleTemplate<T>::GetMember() const {
  return m_member;
}

template class SampleTemplate<int>;
