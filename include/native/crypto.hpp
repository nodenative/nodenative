#ifndef __NATIVE_CRYPTO_HPP__
#define __NATIVE_CRYPTO_HPP__

#include "crypto/PBKDF2.hpp"
//#include "crypto/SecureContext.hpp"

namespace native {
namespace crypto {

/** Init crypto module. This method can be called multiple times, but will be initiated only first time
 */
void initCrypto();

} // namespace crypto
} // namespace native

#endif /* __NATIVE_CRYPTO_HPP__ */
