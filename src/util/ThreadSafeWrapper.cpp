#include "ThreadSafeWrapper.hpp"

namespace aima::util {
    template
    class ThreadSafeWrapper<std::string>;

    template
    class ThreadSafeWrapper<std::string_view>;

    template
    class ThreadSafeWrapper<bool>;

    template
    class ThreadSafeWrapper<double>;
}
