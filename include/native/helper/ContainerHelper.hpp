#ifndef __NATIVE_HELPER_CONTAINERHELPER_HPP__
#define __NATIVE_HELPER_CONTAINERHELPER_HPP__

namespace native {
namespace helper {

/// The helper is for doing safe downcasts from base types to derived types.
template <typename Inner, typename Outer> class ContainerOfHelper {
public:
  inline ContainerOfHelper(Inner Outer::*field, Inner *pointer);
  template <typename TypeName> inline operator TypeName *() const;

private:
  Outer *const pointer_;
};

/**
 * Calculate the address of the outer (i.e. embedding) struct from
 * the interior pointer to a data member.
 */
template <typename Inner, typename Outer>
inline ContainerOfHelper<Inner, Outer> ContainerOf(Inner Outer::*field, Inner *pointer);

} /* namespace helper */
} /* namespace native */

#endif /* __NATIVE_HELPER_CONTAINERHELPER_HPP__ */
