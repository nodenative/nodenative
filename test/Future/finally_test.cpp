#include "native/native.hpp"
#include "gtest/gtest.h"

#include <thread>

TEST(FutureFinallyTest, FromAsyncResolved) {
  bool calledAsync = false;
  bool calledFinally = false;
  bool calledFinallyThen = false;
  std::thread::id mainThreadId = std::this_thread::get_id();

  std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
  {
    native::async(currLoop,
                  [&calledAsync, &mainThreadId]() {
                    calledAsync = true;
                    std::thread::id currThreadId = std::this_thread::get_id();
                    EXPECT_EQ(mainThreadId, currThreadId);
                  })
        .finally([&calledFinally, &calledAsync, mainThreadId]() {
          EXPECT_EQ(calledAsync, true);
          calledFinally = true;
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
        })
        .then([&calledFinallyThen, &calledFinally, mainThreadId]() {
          EXPECT_EQ(calledFinally, true);
          calledFinallyThen = true;
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
        });
  }
  EXPECT_EQ(calledAsync, false);
  EXPECT_EQ(calledFinally, false);
  EXPECT_EQ(calledFinallyThen, false);

  currLoop->run();

  EXPECT_EQ(calledAsync, true);
  EXPECT_EQ(calledFinally, true);
  EXPECT_EQ(calledFinallyThen, true);
}

TEST(FutureFinallyTest, FromAsyncRejected) {
  bool calledAsync = false;
  bool calledFinally = false;
  bool calledFinallyThen = false;
  std::thread::id mainThreadId = std::this_thread::get_id();

  std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
  {
    native::async(currLoop,
                  [&calledAsync, mainThreadId]() {
                    calledAsync = true;
                    std::thread::id currThreadId = std::this_thread::get_id();
                    EXPECT_EQ(mainThreadId, currThreadId);
                    throw native::FutureError("Reject"); // !!! reject the future object
                  })
        .finally([&calledFinally, &calledAsync, mainThreadId]() {
          EXPECT_EQ(calledAsync, true);
          calledFinally = true;
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
        })
        .then([&calledFinallyThen, &calledFinally, mainThreadId]() {
          EXPECT_EQ(calledFinally, true);
          calledFinallyThen = true;
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
        });
  }
  EXPECT_EQ(calledAsync, false);
  EXPECT_EQ(calledFinally, false);
  EXPECT_EQ(calledFinallyThen, false);

  currLoop->run();

  EXPECT_EQ(calledAsync, true);
  EXPECT_EQ(calledFinally, true);
  EXPECT_EQ(calledFinallyThen, true);
}

TEST(FutureFinallyTest, AvoidValueResolved) {
  bool calledAsync = false;
  bool calledFinally = false;
  bool calledFinallyThen = false;
  std::thread::id mainThreadId = std::this_thread::get_id();

  std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
  {
    native::async(currLoop,
                  [&calledAsync, &mainThreadId]() -> int {
                    calledAsync = true;
                    std::thread::id currThreadId = std::this_thread::get_id();
                    EXPECT_EQ(mainThreadId, currThreadId);
                    return 10;
                  })
        .finally([&calledFinally, &calledAsync, mainThreadId]() -> double {
          EXPECT_EQ(calledAsync, true);
          calledFinally = true;
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
          return 20.0;
        })
        .then([&calledFinallyThen, &calledFinally, mainThreadId](double iValue) {
          EXPECT_EQ(iValue, 20.0);
          EXPECT_EQ(calledFinally, true);
          calledFinallyThen = true;
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
        });
  }
  EXPECT_EQ(calledAsync, false);
  EXPECT_EQ(calledFinally, false);
  EXPECT_EQ(calledFinallyThen, false);

  currLoop->run();

  EXPECT_EQ(calledAsync, true);
  EXPECT_EQ(calledFinally, true);
  EXPECT_EQ(calledFinallyThen, true);
}

TEST(FutureFinallyTest, AvoidValueRejected) {
  bool calledAsync = false;
  bool calledFinally = false;
  bool calledFinallyThen = false;
  std::thread::id mainThreadId = std::this_thread::get_id();

  std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
  {
    native::async(currLoop,
                  [&calledAsync, mainThreadId]() -> int {
                    calledAsync = true;
                    std::thread::id currThreadId = std::this_thread::get_id();
                    EXPECT_EQ(mainThreadId, currThreadId);
                    throw native::FutureError("Reject"); // !!! reject the future object
                  })
        .finally([&calledFinally, &calledAsync, mainThreadId]() -> double {
          EXPECT_EQ(calledAsync, true);
          calledFinally = true;
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
          return 20.0;
        })
        .then([&calledFinallyThen, &calledFinally, mainThreadId](double iValue) {
          EXPECT_EQ(iValue, 20.0);
          EXPECT_EQ(calledFinally, true);
          calledFinallyThen = true;
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
        });
  }
  EXPECT_EQ(calledAsync, false);
  EXPECT_EQ(calledFinally, false);
  EXPECT_EQ(calledFinallyThen, false);

  currLoop->run();

  EXPECT_EQ(calledAsync, true);
  EXPECT_EQ(calledFinally, true);
  EXPECT_EQ(calledFinallyThen, true);
}

TEST(FutureFinallyTest, AvoidFutureValueResolved) {
  bool calledAsync = false;
  bool calledFinally = false;
  bool calledFinallyThen = false;
  std::thread::id mainThreadId = std::this_thread::get_id();

  std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
  {
    native::async(currLoop,
                  [&calledAsync, &mainThreadId]() -> native::Future<int> {
                    native::Future<int> future = native::async([&calledAsync, &mainThreadId]() -> int {
                      calledAsync = true;
                      return 10;
                    });

                    std::thread::id currThreadId = std::this_thread::get_id();
                    EXPECT_EQ(mainThreadId, currThreadId);
                    EXPECT_EQ(calledAsync, false);
                    return future;
                  })
        .finally([&calledFinally, &calledAsync, mainThreadId]() -> native::Future<double> {
          native::Future<double> future = native::async([&calledFinally, &calledAsync, mainThreadId]() -> double {
            calledFinally = true;
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
            return 20.0;
          });
          EXPECT_EQ(calledAsync, true);
          EXPECT_EQ(calledFinally, false);
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
          return future;
        })
        .then([&calledFinallyThen, &calledFinally, mainThreadId](double iValue) {
          EXPECT_EQ(iValue, 20.0);
          EXPECT_EQ(calledFinally, true);
          calledFinallyThen = true;
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
        });
  }
  EXPECT_EQ(calledAsync, false);
  EXPECT_EQ(calledFinally, false);
  EXPECT_EQ(calledFinallyThen, false);

  currLoop->run();

  EXPECT_EQ(calledAsync, true);
  EXPECT_EQ(calledFinally, true);
  EXPECT_EQ(calledFinallyThen, true);
}

TEST(FutureFinallyTest, AvoidFutureValueRejected) {
  bool calledAsync = false;
  bool calledFinally = false;
  bool calledFinallyThen = false;
  std::thread::id mainThreadId = std::this_thread::get_id();

  std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
  {
    native::async(currLoop,
                  [&calledAsync, mainThreadId]() -> native::Future<int> {
                    native::Future<int> future = native::async([&calledAsync, mainThreadId]() -> int {
                      calledAsync = true;
                      std::thread::id currThreadId = std::this_thread::get_id();
                      EXPECT_EQ(mainThreadId, currThreadId);
                      throw native::FutureError("Reject"); // !!! reject the future object
                    });
                    std::thread::id currThreadId = std::this_thread::get_id();
                    EXPECT_EQ(mainThreadId, currThreadId);
                    EXPECT_EQ(calledAsync, false);
                    return future;
                  })
        .finally([&calledFinally, &calledAsync, mainThreadId]() -> native::Future<double> {
          native::Future<double> future = native::async([&calledFinally, &calledAsync, mainThreadId]() -> double {
            calledFinally = true;
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
            return 20.0;
          });
          EXPECT_EQ(calledAsync, true);
          EXPECT_EQ(calledFinally, false);
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
          return future;
        })
        .then([&calledFinallyThen, &calledFinally, mainThreadId](double iValue) {
          EXPECT_EQ(iValue, 20.0);
          EXPECT_EQ(calledFinally, true);
          calledFinallyThen = true;
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
        });
  }
  EXPECT_EQ(calledAsync, false);
  EXPECT_EQ(calledFinally, false);
  EXPECT_EQ(calledFinallyThen, false);

  currLoop->run();

  EXPECT_EQ(calledAsync, true);
  EXPECT_EQ(calledFinally, true);
  EXPECT_EQ(calledFinallyThen, true);
}

TEST(FutureFinallyTest, AvoidFutureVoidResolved) {
  bool calledAsync = false;
  bool calledFinally = false;
  bool calledFinallyThen = false;
  std::thread::id mainThreadId = std::this_thread::get_id();

  std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
  {
    native::async(currLoop,
                  [&calledAsync, &mainThreadId]() -> native::Future<void> {
                    native::Future<void> future =
                        native::async([&calledAsync, &mainThreadId]() { calledAsync = true; });

                    std::thread::id currThreadId = std::this_thread::get_id();
                    EXPECT_EQ(mainThreadId, currThreadId);
                    EXPECT_EQ(calledAsync, false);
                    return future;
                  })
        .finally([&calledFinally, &calledAsync, mainThreadId]() -> native::Future<double> {
          native::Future<double> future = native::async([&calledFinally, &calledAsync, mainThreadId]() -> double {
            calledFinally = true;
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
            return 20.0;
          });
          EXPECT_EQ(calledAsync, true);
          EXPECT_EQ(calledFinally, false);
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
          return future;
        })
        .then([&calledFinallyThen, &calledFinally, mainThreadId](double iValue) {
          EXPECT_EQ(iValue, 20.0);
          EXPECT_EQ(calledFinally, true);
          calledFinallyThen = true;
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
        });
  }
  EXPECT_EQ(calledAsync, false);
  EXPECT_EQ(calledFinally, false);
  EXPECT_EQ(calledFinallyThen, false);

  currLoop->run();

  EXPECT_EQ(calledAsync, true);
  EXPECT_EQ(calledFinally, true);
  EXPECT_EQ(calledFinallyThen, true);
}

TEST(FutureFinallyTest, AvoidFutureVoidRejected) {
  bool calledAsync = false;
  bool calledFinally = false;
  bool calledFinallyThen = false;
  std::thread::id mainThreadId = std::this_thread::get_id();

  std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
  {
    native::async(currLoop,
                  [&calledAsync, mainThreadId]() -> native::Future<void> {
                    native::Future<void> future = native::async([&calledAsync, mainThreadId]() {
                      calledAsync = true;
                      std::thread::id currThreadId = std::this_thread::get_id();
                      EXPECT_EQ(mainThreadId, currThreadId);
                      throw native::FutureError("Reject"); // !!! reject the future object
                    });
                    std::thread::id currThreadId = std::this_thread::get_id();
                    EXPECT_EQ(mainThreadId, currThreadId);
                    EXPECT_EQ(calledAsync, false);
                    return future;
                  })
        .finally([&calledFinally, &calledAsync, mainThreadId]() -> native::Future<double> {
          native::Future<double> future = native::async([&calledFinally, &calledAsync, mainThreadId]() -> double {
            calledFinally = true;
            std::thread::id currThreadId = std::this_thread::get_id();
            EXPECT_EQ(mainThreadId, currThreadId);
            return 20.0;
          });
          EXPECT_EQ(calledAsync, true);
          EXPECT_EQ(calledFinally, false);
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
          return future;
        })
        .then([&calledFinallyThen, &calledFinally, mainThreadId](double iValue) {
          EXPECT_EQ(iValue, 20.0);
          EXPECT_EQ(calledFinally, true);
          calledFinallyThen = true;
          std::thread::id currThreadId = std::this_thread::get_id();
          EXPECT_EQ(mainThreadId, currThreadId);
        });
  }
  EXPECT_EQ(calledAsync, false);
  EXPECT_EQ(calledFinally, false);
  EXPECT_EQ(calledFinallyThen, false);

  currLoop->run();

  EXPECT_EQ(calledAsync, true);
  EXPECT_EQ(calledFinally, true);
  EXPECT_EQ(calledFinallyThen, true);
}
