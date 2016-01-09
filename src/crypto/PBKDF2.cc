#include "native/crypto/PBKDF2.h"
#include "native/crypto/utils.h"
#include "native/error.h"
#include "native/crypto/OpensslHeaders.h"

namespace native {
namespace crypto {

bool PBKDF2(const std::string& iPassword,
           const std::string& iSalt,
           const int iIterations,
           const int iKeyLen,
           const std::string& iDigestName,
           std::string &oData) {
    initCrypto();
    oData.resize(iKeyLen);
    const EVP_MD* digest = nullptr;

    if(iDigestName.empty()) {
        digest = EVP_sha1();
    } else {
        digest = EVP_get_digestbyname(iDigestName.c_str());
        if (digest == nullptr) {
            throw exception("Bad digest name");
        }
    }
    return PKCS5_PBKDF2_HMAC(iPassword.c_str(),
                             iPassword.size(),
                             reinterpret_cast<const unsigned char*>(iSalt.c_str()),
                             iSalt.size(),
                             iIterations,
                             digest,
                             iKeyLen,
                             reinterpret_cast<unsigned char*>(const_cast<char*>(oData.c_str())));
}


} // namespace crypto
} // namespace native
