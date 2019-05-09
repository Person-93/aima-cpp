#pragma once

#include "util/UniqueIdMixin.hpp"

namespace aima::core {
    class EnvironmentDestroyedView : public util::UniqueIdMixin<EnvironmentDestroyedView> {
        friend class Environment;

    protected:
        ~EnvironmentDestroyedView() = default;

    private:
        virtual void notify() = 0;
    };
}
