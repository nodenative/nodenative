#include "native/native.h"
#include "gtest/gtest.h"

TEST(WorkerTest, simple)
{
    bool called = false;
    native::loop currLoop(true);
    {
        native::worker(currLoop, [&called](){
            called = true;
        });
    }

    // At this point the worker callback may be called already

    currLoop.run();

    EXPECT_EQ(called, true);
}

TEST(WorkerTest, DefaultLoop)
{
    bool called = false;
    {
        native::worker([&called](){
            called = true;
        });
    }

    // At this point the worker callback may be called already

    native::run();

    EXPECT_EQ(called, true);
}

