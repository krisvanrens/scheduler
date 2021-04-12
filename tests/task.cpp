#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <functional>
#include <utility>

#include "../source/task.hpp"

template<typename Function>
constexpr std::function<Function> make_lambda() {
  if constexpr (std::is_same_v<Function, void()>) {
    return [] {};
  } else if constexpr (std::is_same_v<Function, int()>) {
    return [] { return 42; };
  } else if constexpr (std::is_same_v<Function, void(int)>) {
    return [](int) {};
  } else if constexpr (std::is_same_v<Function, int(int)>) {
    return [](int) { return 42; };
  } else if constexpr (std::is_same_v<Function, void(int, float)>) {
    return [](int, float) {};
  } else if constexpr (std::is_same_v<Function, int(int, float)>) {
    return [](int, float) { return 42; };
  }

  return {};
}

TYPE_TO_STRING(void());
TYPE_TO_STRING(int());
TYPE_TO_STRING(void(int));
TYPE_TO_STRING(int(int));
TYPE_TO_STRING(void(int, float));
TYPE_TO_STRING(int(int, float));

TEST_SUITE("task") {
  TEST_CASE_TEMPLATE("Default construction", T, void(), int()) {
    task<T> t;

    CHECK(t);
    CHECK_THROWS(t());
  }

  TEST_CASE_TEMPLATE("Default construction", T, void(int), int(int)) {
    task<T> t;

    CHECK(t);
    CHECK_THROWS(t(42));
  }

  TEST_CASE_TEMPLATE("Default construction", T, void(int, float), int(int, float)) {
    task<T> t;

    CHECK(t);
    CHECK_THROWS(t(42, 3.14f));
  }

  TEST_CASE_TEMPLATE("Non-default construction", T, void(), int()) {
    task<T> t{make_lambda<T>()};

    CHECK(!t);

    t();
  }

  TEST_CASE_TEMPLATE("Non-default construction", T, void(int), int(int)) {
    task<T> t{make_lambda<T>()};

    CHECK(!t);

    t(42);
  }

  TEST_CASE_TEMPLATE("Non-default construction", T, void(int, float), int(int, float)) {
    task<T> t{make_lambda<T>()};

    CHECK(!t);

    t(42, 3.14f);
  }

  TEST_CASE_TEMPLATE("Move construction", T, void(), int()) {
    task<T> t1{make_lambda<T>()};
    t1();

    task<T> t2{std::move(t1)};

    CHECK(!t2);
    t2();

    CHECK(t1);
    CHECK_THROWS(t1());
  }

  TEST_CASE_TEMPLATE("Move construction", T, void(int), int(int)) {
    task<T> t1{make_lambda<T>()};
    t1(42);

    task<T> t2{std::move(t1)};

    CHECK(!t2);
    t2(42);

    CHECK(t1);
    CHECK_THROWS(t1(42));
  }

  TEST_CASE_TEMPLATE("Move construction", T, void(int, float), int(int, float)) {
    task<T> t1{make_lambda<T>()};
    t1(42, 3.14f);

    task<T> t2{std::move(t1)};

    CHECK(!t2);
    t2(42, 3.14f);

    CHECK(t1);
    CHECK_THROWS(t1(42, 3.14f));
  }

  TEST_CASE_TEMPLATE("Move assignment", T, void(), int()) {
    task<T> t1{make_lambda<T>()};
    task<T> t2;

    t1();
    REQUIRE(t2);

    t2 = std::move(t1);

    CHECK(!t2);
    t2();

    CHECK(t1);
    CHECK_THROWS(t1());
  }

  TEST_CASE_TEMPLATE("Move assignment", T, void(int), int(int)) {
    task<T> t1{make_lambda<T>()};
    task<T> t2;

    t1(42);
    REQUIRE(t2);

    t2 = std::move(t1);

    CHECK(!t2);
    t2(42);

    CHECK(t1);
    CHECK_THROWS(t1(42));
  }

  TEST_CASE_TEMPLATE("Move assignment", T, void(int, float), int(int, float)) {
    task<T> t1{make_lambda<T>()};
    task<T> t2;

    t1(42, 3.14f);
    REQUIRE(t2);

    t2 = std::move(t1);

    CHECK(!t2);
    t2(42, 3.14f);

    CHECK(t1);
    CHECK_THROWS(t1(42, 3.14f));
  }

} // TEST_SUITE
