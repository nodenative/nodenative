#include "native/native.h"
#include "gtest/gtest.h"

#include <thread>

TEST(AsyncTest, async)
{
    bool called = false;
    std::thread::id mainThreadId = std::this_thread::get_id();

    native::loop currLoop(true);
    {
        native::async(currLoop, [&called, &mainThreadId](){
            called = true;
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });
    }
    EXPECT_EQ(called, false);

    currLoop.run();

    EXPECT_EQ(called, true);
}

TEST(AsyncTest, asyncDefaultLoop)
{
    bool called = false;
    std::thread::id mainThreadId = std::this_thread::get_id();
    {
        native::async([&called, &mainThreadId](){
            called = true;
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });
    }
    EXPECT_EQ(called, false);

    native::run();

    EXPECT_EQ(called, true);
}

TEST(AsyncTest, asyncWithParamaterRef)
{
    bool called = false;
    std::thread::id mainThreadId = std::this_thread::get_id();
    native::loop currLoop(true);
    {
        native::async(currLoop, [&mainThreadId](bool &iCalled){
            iCalled = true;
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        }, called);
    }
    EXPECT_EQ(called, false);

    currLoop.run();

    EXPECT_EQ(called, true);
}

TEST(AsyncTest, asyncWithParamaterValue)
{
    bool called = false, asyncCalled = false;
    std::thread::id mainThreadId = std::this_thread::get_id();
    native::loop currLoop(true);
    {
        native::async(currLoop, [&asyncCalled, &mainThreadId](bool iCalled){
            iCalled = true;
            asyncCalled = true;
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        }, called);
    }
    EXPECT_EQ(asyncCalled, false);
    EXPECT_EQ(called, false);

    currLoop.run();

    EXPECT_EQ(asyncCalled, true);
    EXPECT_EQ(called, false);
}

TEST(AsyncTest, asyncWithReturnRef)
{
    bool called = false;
    std::thread::id mainThreadId = std::this_thread::get_id();
    native::loop currLoop(true);
    {
        native::async(currLoop, [&called, &mainThreadId]() -> bool&{
            return called;
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        }).then([&mainThreadId](bool& iCalled) {
            iCalled = true;
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });
    }
    EXPECT_EQ(called, false);

    currLoop.run();

    EXPECT_EQ(called, true);
}

TEST(AsyncTest, asyncWithReturnValue)
{
    bool called = false, asyncCalled = false;
    std::thread::id mainThreadId = std::this_thread::get_id();
    native::loop currLoop(true);
    {
        native::async(currLoop, [&called, &mainThreadId]() -> bool {
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
            return called;
        }).then([&asyncCalled, &mainThreadId](bool iCalled) {
            asyncCalled = true;
            iCalled = true;
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });
    }
    EXPECT_EQ(asyncCalled, false);
    EXPECT_EQ(called, false);

    currLoop.run();

    EXPECT_EQ(asyncCalled, true);
    EXPECT_EQ(called, false);
}

TEST(AsyncTest, asyncMultipThenValueOrder)
{
    std::string order;
    std::thread::id mainThreadId = std::this_thread::get_id();

    native::loop currLoop(true);
    {
        auto async1 = native::async(currLoop, [&order, &mainThreadId]() {
            order += "1";
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });
        auto async2 = async1.then([&order, &mainThreadId]() {
            order += ",2";
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });
        async1.then([&order, &mainThreadId]() {
            order += ",21";
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });
        async2.then([&order, &mainThreadId]{
            order += ",3";
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });
        async2.then([&order, &mainThreadId]() {
            order += ",31";
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });
    }

    std::string expectedorder;
    EXPECT_EQ(order, expectedorder);

    currLoop.run();

    // Maybe this is an issue, for non async callback first are resolved dependences callbacks
    // An solution would be each callback from "then" to call async, this may resolve the issue
    expectedorder = "1,2,21,3,31";
    EXPECT_EQ(expectedorder, order);
}

TEST(AsyncTest, asyncError)
{
    bool exceptionReceived = false;
    bool thenProcessed = false;
    bool afterErrorProcessed = false;
    const std::string errorText("TestError");
    std::thread::id mainThreadId = std::this_thread::get_id();

    native::loop currLoop(true);
    {
        native::async(currLoop, [&errorText, &mainThreadId]() {
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
            throw native::FutureError(errorText);
        }).then([&thenProcessed, &mainThreadId](){
            thenProcessed = true;
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        }).error([&exceptionReceived, &errorText, &mainThreadId](const native::FutureError& iError){
            exceptionReceived = true;
            EXPECT_EQ(iError.message(), errorText);
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
            // By not throwing any error, it means that the state is resolved
        }).then([&afterErrorProcessed, &exceptionReceived, &mainThreadId](){
            EXPECT_EQ(exceptionReceived, true);
            afterErrorProcessed = true;
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
        });
    }

    EXPECT_EQ(exceptionReceived, false);
    EXPECT_EQ(thenProcessed, false);
    EXPECT_EQ(afterErrorProcessed, false);

    currLoop.run();

    EXPECT_EQ(exceptionReceived, true);
    EXPECT_EQ(thenProcessed, false);
    EXPECT_EQ(afterErrorProcessed, true);
}

TEST(AsyncTest, ReturnFuture)
{
    bool called_p1 = false;
    bool called_p2 = false;
    bool called_p1_after = false;
    std::thread::id mainThreadId = std::this_thread::get_id();
    {
        native::async([&called_p1, &called_p2, &mainThreadId]() -> native::Future<void>{
            auto future = native::async([&called_p1, &called_p2](){
                EXPECT_EQ(called_p1, true);
                called_p2 = true;
            });
            called_p1 = true;
            EXPECT_EQ(called_p2, false);
            return future;
        }).then([&called_p1, &called_p2, &called_p1_after, &mainThreadId](){
            called_p1_after = true;
            EXPECT_EQ(called_p1, true);
            EXPECT_EQ(called_p2, true);
        });

        EXPECT_EQ(called_p1, false);
        EXPECT_EQ(called_p2, false);
        EXPECT_EQ(called_p1_after, false);
    }

    EXPECT_EQ(called_p1, false);
    EXPECT_EQ(called_p2, false);
    EXPECT_EQ(called_p1_after, false);

    native::run();

    EXPECT_EQ(called_p1, true);
    EXPECT_EQ(called_p2, true);
    EXPECT_EQ(called_p1_after, true);
}

TEST(AsyncTest, ReturnFutureWithValue)
{
    bool called_p1 = false;
    bool called_p2 = false;
    bool called_p1_after = false;
    std::thread::id mainThreadId = std::this_thread::get_id();
    {
        native::async([&called_p1, &called_p2, &mainThreadId]() -> native::Future<int>{
            auto future = native::async([&called_p1, &called_p2, &mainThreadId]() -> int{
                EXPECT_EQ(called_p1, true);
                called_p2 = true;
                return 1;
            });
            called_p1 = true;
            EXPECT_EQ(called_p2, false);
            return future;
        }).then([&called_p1, &called_p2, &called_p1_after, &mainThreadId](int iValue){
            called_p1_after = true;
            EXPECT_EQ(iValue, 1);
            EXPECT_EQ(called_p1, true);
            EXPECT_EQ(called_p2, true);
        });

        EXPECT_EQ(called_p1, false);
        EXPECT_EQ(called_p2, false);
        EXPECT_EQ(called_p1_after, false);
    }

    EXPECT_EQ(called_p1, false);
    EXPECT_EQ(called_p2, false);
    EXPECT_EQ(called_p1_after, false);

    native::run();

    EXPECT_EQ(called_p1, true);
    EXPECT_EQ(called_p2, true);
    EXPECT_EQ(called_p1_after, true);
}
