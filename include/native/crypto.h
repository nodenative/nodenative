#ifndef __NATIVE_CRYPTO_H__
#define __NATIVE_CRYPTO_H__

#include "crypto/PBKDF2.h"
#include "crypto/SecureContext.h"

namespace native {
namespace crypto {

/** Init crypto module. This method can be called multiple times, but will be initiated only first time
 */
void initCrypto();

} // namespace crypto
} // namespace native

#endif // __NATIVE_CRYPTO_H__
