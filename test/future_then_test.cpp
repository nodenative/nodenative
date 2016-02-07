#include "native/native.h"
#include "gtest/gtest.h"

TEST(FutureThentest, ReturnFuture)
{
    bool called = false;
    native::loop currLoop(true);
    {
        native::async(currLoop, [&called](){
            called = true;
        });
    }
    EXPECT_EQ(called, false);

    currLoop.run();

    EXPECT_EQ(called, true);
}

