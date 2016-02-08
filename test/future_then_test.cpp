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

        native::Future<void> f3 = f2.then([&order]{
            order+=",3";
        });

        native::Future<void> f4 = f3.then([&order]{
            order+=",4";
        });

        // Add more async work to second future
        native::Future<void> f21 = f1.then([&order]{
            order+=",21";
        });

        native::Future<void> f22 = f1.then([&order]{
            order+=",22";
        });

        native::Future<void> f23 = f1.then([&order]{
            order+=",23";
        });

        EXPECT_EQ(expectedOrder, order);

        p.setValue();

        EXPECT_EQ(expectedOrder, order);
    //}

    EXPECT_EQ(expectedOrder, order);

    currLoop.run();

    expectedOrder = "1,2,21,22,23,3,4";
    EXPECT_EQ(expectedOrder, order);
}

TEST(FutureThentest, ReturnFuture)
{
    bool called_p1 = false;
    bool called_p2 = false;
    bool called_p1_after = false;
    native::loop currLoop(true);
    {
        native::Promise<void> promise(currLoop);
        promise.getFuture().then([&called_p1, &called_p2]() -> native::Future<void>{
            auto future = native::async([&called_p1, &called_p2](){
                EXPECT_EQ(called_p1, true);
                called_p2 = true;
            });
            called_p1 = true;
            EXPECT_EQ(called_p2, false);
            return future;
        }).then([&called_p1, &called_p2, &called_p1_after](){
            called_p1_after = true;
            EXPECT_EQ(called_p1, true);
            EXPECT_EQ(called_p2, true);
        });

        EXPECT_EQ(called_p1, false);
        EXPECT_EQ(called_p2, false);
        EXPECT_EQ(called_p1_after, false);

        promise.setValue();

        EXPECT_EQ(called_p1, false);
        EXPECT_EQ(called_p2, false);
        EXPECT_EQ(called_p1_after, false);
    }

    EXPECT_EQ(called_p1, false);
    EXPECT_EQ(called_p2, false);
    EXPECT_EQ(called_p1_after, false);

    currLoop.run();

    EXPECT_EQ(called_p1, true);
    EXPECT_EQ(called_p2, true);
    EXPECT_EQ(called_p1_after, true);
}

TEST(FutureThentest, ReturnFutureWithValue)
{
    bool called_p1 = false;
    bool called_p2 = false;
    bool called_p1_after = false;
    native::loop currLoop(true);
    {
        native::Promise<void> promise(currLoop);
        promise.getFuture().then([&called_p1, &called_p2]() -> native::Future<int>{
            auto future = native::async([&called_p1, &called_p2]() -> int{
                EXPECT_EQ(called_p1, true);
                called_p2 = true;
                return 1;
            });
            called_p1 = true;
            EXPECT_EQ(called_p2, false);
            return future;
        }).then([&called_p1, &called_p2, &called_p1_after](int iValue){
            called_p1_after = true;
            EXPECT_EQ(iValue, 1);
            EXPECT_EQ(called_p1, true);
            EXPECT_EQ(called_p2, true);
        });

        EXPECT_EQ(called_p1, false);
        EXPECT_EQ(called_p2, false);
        EXPECT_EQ(called_p1_after, false);

        promise.setValue();

        EXPECT_EQ(called_p1, false);
        EXPECT_EQ(called_p2, false);
        EXPECT_EQ(called_p1_after, false);
    }

    EXPECT_EQ(called_p1, false);
    EXPECT_EQ(called_p2, false);
    EXPECT_EQ(called_p1_after, false);

    currLoop.run();

    EXPECT_EQ(called_p1, true);
    EXPECT_EQ(called_p2, true);
    EXPECT_EQ(called_p1_after, true);
}

