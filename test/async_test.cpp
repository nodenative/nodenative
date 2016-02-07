#include "native/native.h"
#include "gtest/gtest.h"

TEST(Asynctest, async)
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

TEST(Asynctest, asyncDefaultLoop)
{
    bool called = false;
    {
        native::async([&called](){
            called = true;
        });
    }
    EXPECT_EQ(called, false);

    native::run();

    EXPECT_EQ(called, true);
}

TEST(Asynctest, asyncWithParamaterRef)
{
    bool called = false;
    native::loop currLoop(true);
    {
        native::async(currLoop, [](bool &iCalled){
            iCalled = true;
        }, called);
    }
    EXPECT_EQ(called, false);

    currLoop.run();

    EXPECT_EQ(called, true);
}

TEST(Asynctest, asyncWithParamaterValue)
{
    bool called = false, asyncCalled = false;
    native::loop currLoop(true);
    {
        native::async(currLoop, [&asyncCalled](bool iCalled){
            iCalled = true;
            asyncCalled = true;
        }, called);
    }
    EXPECT_EQ(asyncCalled, false);
    EXPECT_EQ(called, false);

    currLoop.run();

    EXPECT_EQ(asyncCalled, true);
    EXPECT_EQ(called, false);
}

TEST(Asynctest, asyncWithReturnRef)
{
    bool called = false;
    native::loop currLoop(true);
    {
        native::async(currLoop, [&called]() -> bool&{
            return called;
        }).then([](bool& iCalled) {
            iCalled = true;
        });
    }
    EXPECT_EQ(called, false);

    currLoop.run();

    EXPECT_EQ(called, true);
}

TEST(Asynctest, asyncWithReturnValue)
{
    bool called = false, asyncCalled = false;
    native::loop currLoop(true);
    {
        native::async(currLoop, [&called]() -> bool {
            return called;
        }).then([&asyncCalled](bool iCalled) {
            asyncCalled = true;
            iCalled = true;
        });
    }
    EXPECT_EQ(asyncCalled, false);
    EXPECT_EQ(called, false);

    currLoop.run();

    EXPECT_EQ(asyncCalled, true);
    EXPECT_EQ(called, false);
}

TEST(Asynctest, asyncMultipThenValueOrder)
{
    std::string order;

    native::loop currLoop(true);
    {
        auto async1 = native::async(currLoop, [&order]() {
            order += "1";
        });
        auto async2 = async1.then([&order]() {
            order += ",2";
        });
        async1.then([&order]() {
            order += ",21";
        });
        async2.then([&order]{
            order += ",3";
        });
        async2.then([&order]() {
            order += ",31";
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

TEST(Asynctest, asyncError)
{
    bool exceptionReceived = false;
    bool thenProcessed = false;
    bool afterErrorProcessed = false;
    const std::string errorText("TestError");

    native::loop currLoop(true);
    {
        native::async(currLoop, [&errorText]() {
            throw native::FutureError(errorText);
        }).then([&thenProcessed](){
            thenProcessed = true;
        }).error([&exceptionReceived, &errorText](const native::FutureError& iError){
            exceptionReceived = true;
            EXPECT_EQ(iError.message(), errorText);
            // By not throwing any error, it means that the state is resolved
        }).then([&afterErrorProcessed, &exceptionReceived](){
            EXPECT_EQ(exceptionReceived, true);
            afterErrorProcessed = true;
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
