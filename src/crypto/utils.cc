#include <native/crypto/utils.h>
#include "native/crypto/OpensslHeaders.h"

using namespace native;
using namespace crypto;

namespace {
    // TODO
    /*
    static void crypto_lock_cb(int mode, int n, const char* file, int line) {
    }

    // TODO
    static void crypto_threadid_cb(CRYPTO_THREADID* tid) {
    }
    */
}

void initCrypto() {
    // TODO: make in thread safe
    static bool initiated = false;
    if(initiated) {
        return;
    }

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    //crypto_lock_init();
    // TODO
    //CRYPTO_set_locking_callback(crypto_lock_cb);
    //CRYPTO_THREADID_set_callback(crypto_threadid_cb);
}

