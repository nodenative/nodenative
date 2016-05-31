#include "native/native.hpp"
#include "gtest/gtest.h"

#include <thread>

TEST(WorkerTest, simple) {
  bool called = false;
  bool called2 = false;
  std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
  std::thread::id mainThreadId = std::this_thread::get_id();
  {
    native::worker(currLoop, [&called, &mainThreadId]() {
      called = true;
      std::thread::id currThreadId = std::this_thread::get_id();
      EXPECT_NE(mainThreadId, currThreadId);
    }).then([&called, &called2, &mainThreadId]() {
      EXPECT_EQ(called, true);
      called2 = true;
      std::thread::id currThreadId = std::this_thread::get_id();
      EXPECT_EQ(mainThreadId, currThreadId);
    });
  }

  // At this point the worker callback may be called already

  currLoop->run();

  EXPECT_EQ(called, true);
  EXPECT_EQ(called2, true);
}

TEST(WorkerTest, DefaultLoop) {
  bool called = false;
  bool called2 = false;
  std::thread::id mainThreadId = std::this_thread::get_id();
  {
    native::worker([&called, &mainThreadId]() {
      called = true;
      std::thread::id currThreadId = std::this_thread::get_id();
      EXPECT_NE(mainThreadId, currThreadId);
    }).then([&called, &called2, &mainThreadId]() {
      EXPECT_EQ(called, true);
      called2 = true;
      std::thread::id currThreadId = std::this_thread::get_id();
      EXPECT_EQ(mainThreadId, currThreadId);
    });
  }

  // At this point the worker callback may be called already

  native::run();

  EXPECT_EQ(called, true);
  EXPECT_EQ(called2, true);
}

TEST(WorkerTest, ReturnValue) {
  bool called = false;
  bool called2 = false;
  int expectedValue = 1;
  std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
  std::thread::id mainThreadId = std::this_thread::get_id();
  {
    native::worker(currLoop, [&called, &mainThreadId, &expectedValue]() -> int {
      called = true;
      std::thread::id currThreadId = std::this_thread::get_id();
      EXPECT_NE(mainThreadId, currThreadId);
      return expectedValue;
    }).then([&called, &called2, &mainThreadId, &expectedValue](int iValue) {
      EXPECT_EQ(called, true);
      called2 = true;
      std::thread::id currThreadId = std::this_thread::get_id();
      EXPECT_EQ(mainThreadId, currThreadId);
      EXPECT_EQ(expectedValue, iValue);
      EXPECT_NE(&expectedValue, &iValue);
    });
  }

  // At this point the worker callback may be called already

  currLoop->run();

  EXPECT_EQ(called, true);
  EXPECT_EQ(called2, true);
}

TEST(WorkerTest, ReturnValueRef) {
  bool called = false;
  bool called2 = false;
  int expectedValue = 1;
  std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
  std::thread::id mainThreadId = std::this_thread::get_id();
  {
    native::worker(currLoop, [&called, &mainThreadId, &expectedValue]() -> int & {
      called = true;
      std::thread::id currThreadId = std::this_thread::get_id();
      EXPECT_NE(mainThreadId, currThreadId);
      return expectedValue;
    }).then([&called, &called2, &mainThreadId, &expectedValue](int &iValue) {
      EXPECT_EQ(called, true);
      called2 = true;
      std::thread::id currThreadId = std::this_thread::get_id();
      EXPECT_EQ(mainThreadId, currThreadId);
      EXPECT_EQ(expectedValue, iValue);
      EXPECT_EQ(&expectedValue, &iValue);
    });
  }

  // At this point the worker callback may be called already

  currLoop->run();

  EXPECT_EQ(called, true);
  EXPECT_EQ(called2, true);
}

// TODO: resolve the potential issue. To reproduce run:
// out/Release/test -- --gtest_repeat=1000 --gtest_break_on_failure --gtest_filter="WorkerTest.ReturnFutureVoid"
TEST(DISABLED_WorkerTest, ReturnFutureVoid) {
  bool called = false;
  bool called2 = false;
  bool called3 = false;
  std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
  std::thread::id mainThreadId = std::this_thread::get_id();
  {
    native::worker(currLoop, [&called, &called3, &mainThreadId]() -> native::Future<void> {
      std::cout << "!!! Inside the worker start\n";
      // TODO: fix the future for multithread
      native::Future<void> future = native::async([&called, &called3, &mainThreadId]() {
        called3 = true;
        std::thread::id currThreadId = std::this_thread::get_id();
        EXPECT_EQ(mainThreadId, currThreadId);
        std::cout << "!!! Future inside the worker\n";
      });

      called = true;
      std::thread::id currThreadId = std::this_thread::get_id();
      EXPECT_NE(mainThreadId, currThreadId);
      std::cout << "!!! Inside the worker\n";
      return future;
    }).then([&called, &called2, &called3, &mainThreadId]() {
      std::cout << "!!! Inside the then\n";
      EXPECT_EQ(called, true);
      EXPECT_EQ(called3, true);
      called2 = true;
      std::thread::id currThreadId = std::this_thread::get_id();
      EXPECT_EQ(mainThreadId, currThreadId);
    });
  }

  // At this point the worker callback may be called already

  currLoop->run();

  EXPECT_EQ(called, true);
  EXPECT_EQ(called2, true);
  EXPECT_EQ(called3, true);
}

TEST(DISABLED_WorkerTest, ReturnFutureValue) {
  bool called = false;
  bool called2 = false;
  bool called3 = false;
  int expectedValue = 1;
  std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
  std::thread::id mainThreadId = std::this_thread::get_id();
  {
    native::worker(currLoop, [&called, &called3, &expectedValue, &mainThreadId]() -> native::Future<int> {
      native::Future<int> future = native::async([&called, &called3, expectedValue, &mainThreadId]() -> int {
        called3 = true;
        std::thread::id currThreadId = std::this_thread::get_id();
        EXPECT_EQ(mainThreadId, currThreadId);
        return expectedValue;
      });
      called = true;
      std::thread::id currThreadId = std::this_thread::get_id();
      EXPECT_NE(mainThreadId, currThreadId);
      return future;
    }).then([&called, &called2, &called3, &mainThreadId, &expectedValue](int iValue) {
      EXPECT_EQ(called, true);
      EXPECT_EQ(called3, true);
      called2 = true;
      std::thread::id currThreadId = std::this_thread::get_id();
      EXPECT_EQ(mainThreadId, currThreadId);
      EXPECT_EQ(expectedValue, iValue);
      EXPECT_NE(&expectedValue, &iValue);
      std::cout << "!!! Inside the then\n";
    });
  }

  // At this point the worker callback may be called already

  currLoop->run();

  EXPECT_EQ(called, true);
  // TODO: Finish the possibility to call the waiter after the future object is resolved
  EXPECT_EQ(called2, true);
  EXPECT_EQ(called3, true);
}

TEST(DISABLED_WorkerTest, ReturnFutureVoidError) {
  bool called = false;
  bool called2 = false;
  bool called3 = false;
  bool called4 = false;
  native::FutureError expectedError("ErrorTest1");

  std::shared_ptr<native::Loop> currLoop = native::Loop::Create(true);
  std::thread::id mainThreadId = std::this_thread::get_id();
  {
    native::worker(currLoop,
                   [&called, &called3, &mainThreadId, &expectedError]() -> native::Future<void> {
                     native::Future<void> future = native::async([&called, &called3, &mainThreadId, &expectedError]() {
                       std::chrono::milliseconds time(100);
                       std::this_thread::sleep_for(time);
                       called3 = true;
                       std::thread::id currThreadId = std::this_thread::get_id();
                       EXPECT_EQ(mainThreadId, currThreadId);
                       // Error?
                       throw expectedError;
                     });

                     called = true;
                     std::thread::id currThreadId = std::this_thread::get_id();
                     EXPECT_NE(mainThreadId, currThreadId);
                     return future;
                   })
        .then([&called2]() {
          // This will not be called
          called2 = true;
          EXPECT_EQ(0, 1);
        })
        .error(
            [&called, &called2, &called3, &called4, &mainThreadId, expectedError](const native::FutureError &iError) {
              EXPECT_EQ(called, true);
              EXPECT_EQ(called2, false);
              EXPECT_EQ(called3, true);
              called4 = true;
              std::thread::id currThreadId = std::this_thread::get_id();
              EXPECT_EQ(mainThreadId, currThreadId);
              EXPECT_EQ(expectedError.message(), iError.message());
            });
  }

  // At this point the worker callback may be called already

  currLoop->run();

  EXPECT_EQ(called, true);
  EXPECT_EQ(called2, false);
  EXPECT_EQ(called3, true);
  EXPECT_EQ(called4, true);
}
