#pragma once

#include "util/AttributesMixin.hpp"

namespace aima::core {
    class Percept {
    public:
        virtual ~Percept() = default;

    protected:
        virtual void print( std::ostream& ) const {};

        friend std::ostream& operator<<( std::ostream& out, const Percept& percept ) {
            out << '[';
            percept.print( out );
            return out << ']';
        }
    };
}
