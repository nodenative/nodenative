#ifndef __NATIVE_CRYPTO_PBKDF2_HPP__
#define __NATIVE_CRYPTO_PBKDF2_HPP__

#include <string>

namespace native {
namespace crypto {

bool PBKDF2(const std::string& iPassword,
           const std::string& iSalt,
           const int iIterations,
           const int iKeyLen,
           const std::string& iDigest,
           std::string &oData);

} // namespace crypto
} // namespace native

#endif /* __NATIVE_CRYPTO_PBKDF2_HPP__ */

