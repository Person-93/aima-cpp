#pragma once

#include "util/PrintableMixin.hpp"

namespace aima::core {
    class Percept : public util::PrintableMixin<Percept> {
    public:
        virtual ~Percept() = default;

        virtual void print( std::ostream& ) const = 0;
    };
}
