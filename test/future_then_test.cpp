#include "native/native.h"
#include "gtest/gtest.h"

TEST(FutureThentest, order)
{
    std::string order;
    std::string expectedOrder;
    native::loop currLoop(true);
    //{
        native::Promise<void> p(currLoop);
        native::Future<void> f1 = p.getFuture().then([&order]{
            order+="1";
        });

        native::Future<void> f2 = f1.then([&order]{
            order+=",2";
        });

        native::Future<void> f21 = f2.then([&order]{
            order+=",21";
        });

        native::Future<void> f22 = f21.then([&order]{
            order+=",22";
        });

        native::Future<void> f23 = f21.then([&order]{
            order+=",23";
        });

        native::Future<void> f3 = f2.then([&order]{
            order+=",3";
        });

        native::Future<void> f4 = f3.then([&order]{
            order+=",4";
        });

        EXPECT_EQ(expectedOrder, order);

        p.setValue();

        EXPECT_EQ(expectedOrder, order);
    //}

    EXPECT_EQ(expectedOrder, order);

    currLoop.run();

    expectedOrder = "1,2,3,4, 21, 22, 23";
    EXPECT_EQ(expectedOrder, order);
}

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

