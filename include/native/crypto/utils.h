#ifndef __NATIVE_CRYPTO_UTILS_H__
#define __NATIVE_CRYPTO_UTILS_H__

namespace native {
namespace crypto {

/** Init crypto module. This method can be called multiple times, but will be initiated only first time
 */
void initCrypto();

} // namespace crypto
} // namespace native

#endif // __NATIVE_CRYPTO_UTILS_H__

