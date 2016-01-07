#include "gtest/gtest.h"
#include "native/native.h"
#include "native/crypto.h"

TEST(Crypto, EqConstInt)
{
    EXPECT_EQ(1, 1);
}

TEST(Crypto, PBKDF2)
{
    std::string password("password");
    std::string passwordEncExp;
    std::string passwordEnc;
    native::crypto::PBKDF2(ipassword, 20, 2000, 50, "sha1", passwordEnc)
    EXPECT_EQ(1.0, 1.0);
}

