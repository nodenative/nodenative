#include "native/native.h"
#include "gtest/gtest.h"

TEST(Asynctest, async)
{
    bool called = false;
    native::loop currLoop(true);
    native::async(currLoop, [&called](){
        called = true;
    });
    EXPECT_EQ(called, false);

    currLoop.run();

    EXPECT_EQ(called, true);
}

TEST(Asynctest, asyncWithParamaterRef)
{
    bool called = false;
    native::loop currLoop(true);
    native::async(currLoop, [](bool &iCalled){
        iCalled = true;
    }, called);
    EXPECT_EQ(called, false);

    currLoop.run();

    EXPECT_EQ(called, true);
}

TEST(Asynctest, asyncWithParamaterValue)
{
    bool called = false, asyncCalled = false;
    native::loop currLoop(true);
    native::async(currLoop, [&asyncCalled](bool iCalled){
        iCalled = true;
        asyncCalled = true;
    }, called);
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
    native::async(currLoop, [&called]() -> bool&{
        return called;
    }).then([](bool& iCalled) {
        iCalled = true;
    });
    EXPECT_EQ(called, false);

    currLoop.run();

    EXPECT_EQ(called, true);
}

TEST(Asynctest, asyncWithReturnValue)
{
    bool called = false, asyncCalled = false;
    native::loop currLoop(true);
    native::async(currLoop, [&called]() -> bool {
        return called;
    }).then([&asyncCalled](bool iCalled) {
        asyncCalled = true;
        iCalled = true;
    });
    EXPECT_EQ(asyncCalled, false);
    EXPECT_EQ(called, false);

    currLoop.run();

    EXPECT_EQ(asyncCalled, true);
    EXPECT_EQ(called, false);
}

TEST(Asynctest, asyncMultipThenOrder)
{
    std::string order;

    native::loop currLoop(true);
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

    std::string expectedorder;
    EXPECT_EQ(order, expectedorder);

    currLoop.run();

    // Maybe this is an issue, for non async callback first are resolved dependences callbacks
    // An solution would be each callback from "then" to call async, this may resolve the issue
    expectedorder = "1,2,3,31,21";
    EXPECT_EQ(order, expectedorder);
}
