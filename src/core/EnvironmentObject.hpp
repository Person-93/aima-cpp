#pragma once

#include <memory>
#include "util/UniqueIdMixin.hpp"

namespace aima::core {
    /**
     * Represents an object that exists in the environment.
     */
    class EnvironmentObject : public util::UniqueIdMixin<EnvironmentObject, true> {
    public:
        EnvironmentObject() = default;

        EnvironmentObject( const EnvironmentObject& ) = delete;

        EnvironmentObject( EnvironmentObject&& ) = delete;

        EnvironmentObject& operator=( const EnvironmentObject& ) = delete;

        EnvironmentObject& operator=( EnvironmentObject&& ) = delete;

        virtual std::unique_ptr<EnvironmentObject> clone() = 0;

        virtual ~EnvironmentObject() = default;
    };
}