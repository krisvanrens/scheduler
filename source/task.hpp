#include <functional>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

template<typename>
class task;

template<typename Ret, typename... Args>
class task<Ret(Args...)> {
  struct concept_t {
    virtual ~concept_t()          = default;
    virtual Ret invoke(Args&&...) = 0;
  };

  template<typename Function>
  struct model_t final : concept_t {
    Function function_;

    template<typename Function_ = Function>
    explicit model_t(Function_&& function)
      : function_{std::forward<Function>(function)} {
    }

    Ret invoke(Args&&... args) {
      return std::invoke(function_, std::move(args)...);
    }
  };

  std::unique_ptr<concept_t> model_;

public:
  constexpr task() = default;

  template<typename Function>
  explicit constexpr task(Function&& function)
    : model_{std::make_unique<model_t<std::decay_t<Function>>>(std::forward<Function>(function))} {
  }

  task(task&&) noexcept = default;
  task& operator=(task&&) noexcept = default;

  explicit operator bool() const noexcept {
    return !model_;
  }

  Ret operator()(Args&&... args) {
    if (!model_) {
      throw std::logic_error("task not initialized");
    }

    return model_->invoke(std::move(args)...);
  }
};
