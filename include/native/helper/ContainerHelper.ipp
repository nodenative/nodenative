#ifndef __NATIVE_HELPER_CONTAINERHELPER_IPP__
#define __NATIVE_HELPER_CONTAINERHELPER_IPP__

namespace native {
namespace helper {

template <typename Inner, typename Outer>
ContainerOfHelper<Inner, Outer>::ContainerOfHelper(Inner Outer::*field, Inner *pointer)
    : pointer_(reinterpret_cast<Outer *>(reinterpret_cast<uintptr_t>(pointer) -
                                         reinterpret_cast<uintptr_t>(&(static_cast<Outer *>(0)->*field)))) {}

template <typename Inner, typename Outer>
template <typename TypeName>
ContainerOfHelper<Inner, Outer>::operator TypeName *() const {
  return static_cast<TypeName *>(pointer_);
}

template <typename Inner, typename Outer>
inline ContainerOfHelper<Inner, Outer> ContainerOf(Inner Outer::*field, Inner *pointer) {
  return ContainerOfHelper<Inner, Outer>(field, pointer);
}

} /* namespace helper */
} /* namespace native */

#endif /* __NATIVE_HELPER_CONTAINERHELPER_IPP__ */
