#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../source/task.hpp"

#include <doctest/doctest.h>

#include <functional>
#include <utility>

template<typename Function>
constexpr std::function<Function> make_function() {
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

template<typename Function>
struct unwrap_function;

template<typename Ret, typename... Args>
struct unwrap_function<Ret(Args...)> {
  using return_t = Ret;
  using args_t   = std::tuple<Args...>;
};

template<typename Function>
constexpr typename unwrap_function<Function>::args_t make_args() {
  if constexpr (std::is_same_v<Function, void()> || std::is_same_v<Function, int()>) {
    return {};
  } else if constexpr (std::is_same_v<Function, void(int)> || std::is_same_v<Function, int(int)>) {
    return {42};
  } else if constexpr (std::is_same_v<Function, void(int, float)> || std::is_same_v<Function, int(int, float)>) {
    return {42, 3.14f};
  }

  return {};
}

struct test_t {
  int value = {};
};

TYPE_TO_STRING(void());
TYPE_TO_STRING(int());
TYPE_TO_STRING(void(int));
TYPE_TO_STRING(int(int));
TYPE_TO_STRING(void(int, float));
TYPE_TO_STRING(int(int, float));

TEST_SUITE("task") {
  TEST_CASE_TEMPLATE("Default construction", T, void(), int(), void(int), int(int), void(int, float), int(int, float)) {
    task<T> t;

    CHECK(t);
    CHECK_THROWS(std::apply(t, make_args<T>()));
  }

  TEST_CASE_TEMPLATE("Non-default construction", T, void(), int(), void(int), int(int), void(int, float), int(int, float)) {
    task<T> t{make_function<T>()};

    CHECK(!t);

    std::apply(t, make_args<T>());
  }

  TEST_CASE_TEMPLATE("Move construction", T, void(), int(), void(int), int(int), void(int, float), int(int, float)) {
    task<T> t1{make_function<T>()};
    std::apply(t1, make_args<T>());

    task<T> t2{std::move(t1)};

    CHECK(!t2);
    std::apply(t2, make_args<T>());

    CHECK(t1);
    CHECK_THROWS(std::apply(t1, make_args<T>()));
  }

  TEST_CASE_TEMPLATE("Move assignment", T, void(), int(), void(int), int(int), void(int, float), int(int, float)) {
    task<T> t1{make_function<T>()};
    task<T> t2;

    std::apply(t1, make_args<T>());
    REQUIRE(t2);

    t2 = std::move(t1);

    CHECK(!t2);
    std::apply(t2, make_args<T>());

    CHECK(t1);
    CHECK_THROWS(std::apply(t1, make_args<T>()));
  }

  TEST_CASE("Task argument propagation (matched signatures)") {
    SUBCASE("value") {
      task<void(test_t)> t{[](test_t arg) { CHECK(arg.value == 42); }};

      t(test_t{42});

      test_t x1{42};
      t(x1);

      const test_t x2{42};
      t(x2);
    }

    SUBCASE("const lvalue reference") {
      task<void(const test_t&)> t{[](const test_t& arg) { CHECK(arg.value == 42); }};

      t(test_t{42});

      test_t x1{42};
      t(x1);

      const test_t x2{42};
      t(x2);
    }

    SUBCASE("lvalue reference") {
      task<void(test_t&)> t{[](test_t& arg) { CHECK(arg.value == 42); }};

      test_t x{42};
      t(x);
    }

    SUBCASE("rvalue reference") {
      task<void(test_t&&)> t{[](test_t&& arg) { CHECK(arg.value == 42); }};

      t(test_t{42});

      test_t x{42};
      t(std::move(x));
    }
  }

  TEST_CASE("Task argument propagation (mismatched signatures)") {
    SUBCASE("const lvalue reference to value") {
      task<void(test_t)> t{[](const test_t &arg) { CHECK(arg.value == 42); }};

      t(test_t{42});

      test_t x1{42};
      t(x1);

      const test_t x2{42};
      t(x2);
    }

    SUBCASE("rvalue reference to value") {
      task<void(test_t)> t{[](test_t &&arg) { CHECK(arg.value == 42); }};

      t(test_t{42});

      const test_t x{42};
      t(std::move(x));
    }

    SUBCASE("value to const lvalue reference") {
      task<void(const test_t&)> t{[](test_t arg) { CHECK(arg.value == 42); }};

      t(test_t{42});

      test_t x1{42};
      t(x1);

      const test_t x2{42};
      t(x2);
    }

    SUBCASE("value to lvalue reference") {
      task<void(test_t&)> t{[](test_t arg) { CHECK(arg.value == 42); }};

      test_t x{42};
      t(x);
    }

    SUBCASE("lvalue reference to rvalue reference") {
      task<void(test_t&&)> t{[](test_t arg) { CHECK(arg.value == 42); }};

      t(test_t{42});
    }

    SUBCASE("const lvalue reference to rvalue reference") {
      task<void(test_t&&)> t{[](const test_t& arg) { CHECK(arg.value == 42); }};

      t(test_t{42});

      test_t x{42};
      t(std::move(x));
    }
  }

} // TEST_SUITE
