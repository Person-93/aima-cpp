#pragma once

#include "util/UniqueIdMixin.hpp"

namespace aima::core {
    class EnvironmentObject : public util::UniqueIdMixin<EnvironmentObject, true> {
    public:
        virtual ~EnvironmentObject() = default;
    };
}