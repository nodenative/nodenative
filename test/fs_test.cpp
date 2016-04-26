#include "native/native.hpp"
#include "gtest/gtest.h"

TEST(FsTest, stringToFlags) {
    EXPECT_EQ(native::fs::stringToFlags("r")  , O_RDONLY);
    EXPECT_EQ(native::fs::stringToFlags("rs") , O_RDONLY | O_SYNC);
    EXPECT_EQ(native::fs::stringToFlags("sr") , O_RDONLY | O_SYNC);
    EXPECT_EQ(native::fs::stringToFlags("rw") , O_RDWR);
    EXPECT_EQ(native::fs::stringToFlags("r+") , O_RDWR);
    EXPECT_EQ(native::fs::stringToFlags("rs+"), O_RDWR | O_SYNC);
    EXPECT_EQ(native::fs::stringToFlags("sr+"), O_RDWR | O_SYNC);
    EXPECT_EQ(native::fs::stringToFlags("w")  , O_TRUNC | O_CREAT | O_WRONLY);
    EXPECT_EQ(native::fs::stringToFlags("wx") , O_TRUNC | O_CREAT | O_WRONLY | O_EXCL);
    EXPECT_EQ(native::fs::stringToFlags("xw") , O_TRUNC | O_CREAT | O_WRONLY | O_EXCL);
    EXPECT_EQ(native::fs::stringToFlags("w+") , O_TRUNC | O_CREAT | O_RDWR | O_RDWR);
    EXPECT_EQ(native::fs::stringToFlags("wx+"), O_TRUNC | O_CREAT | O_RDWR | O_RDWR | O_EXCL);
    EXPECT_EQ(native::fs::stringToFlags("xw+"), O_TRUNC | O_CREAT | O_RDWR | O_RDWR | O_EXCL);
    EXPECT_EQ(native::fs::stringToFlags("a")  , O_APPEND | O_CREAT | O_WRONLY);
    EXPECT_EQ(native::fs::stringToFlags("ax") , O_APPEND | O_CREAT | O_WRONLY | O_EXCL);
    EXPECT_EQ(native::fs::stringToFlags("xa") , O_APPEND | O_CREAT | O_WRONLY | O_EXCL);
    EXPECT_EQ(native::fs::stringToFlags("a+") , O_APPEND | O_CREAT | O_RDWR);
    EXPECT_EQ(native::fs::stringToFlags("ax+") , O_APPEND | O_CREAT | O_RDWR | O_EXCL);
    EXPECT_EQ(native::fs::stringToFlags("xa+") , O_APPEND | O_CREAT | O_RDWR | O_EXCL);
    EXPECT_EQ(native::fs::stringToFlags("z") , 0);
}

TEST(FsTest, open) {
}
