#include "gtest/gtest.h"
#include "native/native.h"

TEST(TestGTest, EqConstInt)
{
    EXPECT_EQ(1, 1);
}

TEST(TestGTest, EqConstDouble)
{
    EXPECT_EQ(1.0, 1.0);
}
