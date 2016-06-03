#include "native/native.hpp"
#include "gtest/gtest.h"

#include <thread>

TEST(FutureErrorTest, basic) {
  bool calledAsync = false;
  bool calledError = false;
  bool calledFinallyThen = false;
  bool calledFinallyThen2 = false;
  std::thread::id mainThreadId = std::this_thread::get_id();

  std::shared_ptr<native::Loop> currLoop = native::Loop::Create();
  {
    native::async(currLoop,
                  [&calledAsync, mainThreadId]() {
                    calledAsync = true;
                    std::thread::id currThreadId = std::this_thread::get_id();
                    EXPECT_EQ(mainThreadId, currThreadId);
                    throw native::FutureError("Reject"); // !!! reject the future object
                  })
        .then([&calledFinallyThen, &calledError, mainThreadId]() { calledFinallyThen = true; })
        .error([&calledError, &calledAsync, mainThreadId](const native::FutureError &e) {
          EXPECT_EQ(e.message(), "Reject");
          EXPECT_EQ(calledAsync, true);
          calledError = true;
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
          // At this point the error is resolved
        })
        .then([&calledFinallyThen2, &calledError, mainThreadId]() {
          EXPECT_EQ(calledError, true);
          calledFinallyThen2 = true;
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
        });
  }
  EXPECT_EQ(calledAsync, false);
  EXPECT_EQ(calledFinallyThen, false);
  EXPECT_EQ(calledError, false);
  EXPECT_EQ(calledFinallyThen2, false);

  currLoop->run();

  EXPECT_EQ(calledAsync, true);
  EXPECT_EQ(calledFinallyThen, false); // skipped because of error
  EXPECT_EQ(calledError, true);
  EXPECT_EQ(calledFinallyThen2, true);
}

TEST(FutureErrorTest, rejectFromError) {
  bool calledAsync = false;
  bool calledError = false;
  bool calledFinallyThen = false;
  bool calledFinallyThen2 = false;
  std::thread::id mainThreadId = std::this_thread::get_id();

  std::shared_ptr<native::Loop> currLoop = native::Loop::Create();
  {
    native::async(currLoop,
                  [&calledAsync, mainThreadId]() {
                    calledAsync = true;
                    std::thread::id currThreadId = std::this_thread::get_id();
                    EXPECT_EQ(mainThreadId, currThreadId);
                    throw native::FutureError("Reject"); // !!! reject the future object
                  })
        .then([&calledFinallyThen, &calledError, mainThreadId]() { calledFinallyThen = true; })
        .error([&calledError, &calledAsync, mainThreadId](const native::FutureError &e) {
          EXPECT_EQ(e.message(), "Reject");
          EXPECT_EQ(calledAsync, true);
          calledError = true;
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
          throw native::FutureError("Reject"); // !!! reject even from error
        })
        .then([&calledFinallyThen2, &calledError, mainThreadId]() { calledFinallyThen2 = true; });
  }
  EXPECT_EQ(calledAsync, false);
  EXPECT_EQ(calledFinallyThen, false);
  EXPECT_EQ(calledError, false);
  EXPECT_EQ(calledFinallyThen2, false);

  currLoop->run();

  EXPECT_EQ(calledAsync, true);
  EXPECT_EQ(calledFinallyThen, false); // skipped because of error
  EXPECT_EQ(calledError, true);
  EXPECT_EQ(calledFinallyThen2, false); // skipped because rejected from error
}
