#include "gtest/gtest.h"
//#include "native/native.h"
#include "native/crypto.h"

TEST(Crypto, EqConstInt)
{
    EXPECT_EQ(1, 1);
}

TEST(Crypto, PBKDF2)
{
    const std::string password("password");
    std::string passwordEncExp("\x1E\\\xFF\xDB\xB3*\x1DM\x95\xCB\xC9\xD9VZ\xF1" "eK\xB1\xAD\xCF" "9\xFC\x7F\xB7\x95\xF0\xA5\x87\xD3" "f\xD9" "b\xFD\xDEgH'\xBF\x8Ep\xB0\xD8\xEA\x9E\b\x1D{\xC9\xEA\x86");
    std::string passwordEnc;
    const std::string salt("test");
    const std::string digest("sha1");
    EXPECT_NO_THROW(native::crypto::PBKDF2(password, salt, 2000, 50, digest, passwordEnc));
    EXPECT_EQ(passwordEnc.size(), 50u);
    EXPECT_EQ(passwordEnc, passwordEncExp);
    EXPECT_EQ(1.0, 1.0);
}

TEST(Crypto, PBKDF2InvalidDigest)
{
    const std::string password("password");
    std::string passwordEnc;
    const std::string salt("test");
    const std::string digest("sha1Invalid");
    EXPECT_ANY_THROW(native::crypto::PBKDF2(password, salt, 2000, 50, digest, passwordEnc));
}

