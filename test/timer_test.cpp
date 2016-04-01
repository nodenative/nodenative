#include "gmock/gmock.h"
#include "native/native.hpp"

#include <functional>
#include <chrono>
#include <future>
#include <cstdio>

using namespace native;

TEST(TimerTests, Basic)
{
    std::shared_ptr<Loop> loop = Loop::GetInstanceOrCreateDefault();
    std::weak_ptr<Loop> loopWeak = loop;
    bool isTimeOut = false;
    const uint64_t startTime = loop->now();
    uint64_t elapsedTime = 0U;

    {
        std::shared_ptr<Timer> timer = Timer::Create(loop, [&isTimeOut, &elapsedTime, startTime, loopWeak](){
            isTimeOut = true;
            elapsedTime = loopWeak.lock()->now() - startTime;
        });

        timer->start(100U, 0U);
    }

    EXPECT_EQ(isTimeOut, false);
    EXPECT_EQ(elapsedTime, 0U);

    loop->run();

    std::cout<<"elapsedTime:"<<elapsedTime<<"\n";

    EXPECT_EQ(isTimeOut, true);
    EXPECT_GE(elapsedTime, 100U);
}

TEST(TimerTests, EmptyCallback)
{
    std::shared_ptr<Loop> loop = Loop::GetInstanceOrCreateDefault();
    std::weak_ptr<Loop> loopWeak = loop;
    const uint64_t startTime = loop->now();
    uint64_t elapsedTime = 0U;

    {
        std::shared_ptr<Timer> timer = Timer::Create(loop, {});

        timer->start(100U, 0U);
    }

    EXPECT_EQ(elapsedTime, 0U);

    loop->run();

    elapsedTime = loop->now() - startTime;
    std::cout<<"elapsedTime:"<<elapsedTime<<"\n";

    EXPECT_GE(elapsedTime, 100U);
}

TEST(TimerTests, EmptyCallbackRepeat)
{
    std::shared_ptr<Loop> loop = Loop::GetInstanceOrCreateDefault();
    std::weak_ptr<Loop> loopWeak = loop;
    const uint64_t startTime = loop->now();
    uint64_t elapsedTime = 0U;

    {
        std::shared_ptr<Timer> timer = Timer::Create(loop, {});

        timer->start(100U, 200U);
    }

    EXPECT_EQ(elapsedTime, 0U);

    loop->run();

    elapsedTime = loop->now() - startTime;
    std::cout<<"elapsedTime:"<<elapsedTime<<"\n";

    // We expect that first time to wait to give a chance to set a callback
    EXPECT_GE(elapsedTime, 100U);

    // We expect to be lower than 200ms because the timer should stop if no callback at the first time ms
    EXPECT_LT(elapsedTime, 150U);
}

TEST(TimerTests, Repeat3)
{
    std::shared_ptr<Loop> loop = Loop::GetInstanceOrCreateDefault();
    std::weak_ptr<Loop> loopWeak = loop;
    int timeouts = 0;
    const uint64_t startTime = loop->now();
    uint64_t elapsedTime = 0U;

    {
        std::shared_ptr<Timer> timer = Timer::Create(loop);
        std::weak_ptr<Timer> timerWeak = timer;
        timer->onTimeout([&timeouts, &elapsedTime, startTime, loopWeak, timerWeak](){
            elapsedTime = loopWeak.lock()->now() - startTime;
            if(++timeouts >= 3) {
                timerWeak.lock()->close();
            };
        });

        timer->start(100U, 50U);
    }

    EXPECT_EQ(timeouts, 0);
    EXPECT_EQ(elapsedTime, 0U);

    loop->run();

    std::cout<<"elapsedTime:"<<elapsedTime<<"\n";

    EXPECT_EQ(timeouts, 3);
    // 100 + 50 + 50
    EXPECT_GE(elapsedTime, 200U);
}
