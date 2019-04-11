#pragma once

#include <memory>
#include <iosfwd>
#include "util/StringBuilder.hpp"

namespace aima::core {
    /**
     * Represents what an agent perceives at a given point in time
     */
    class Percept {
    public:
        Percept& operator=( const Percept& ) = delete;

        Percept& operator=( Percept&& ) = delete;

        virtual std::unique_ptr<Percept> clone() const = 0;

        virtual ~Percept() = default;

        explicit operator std::string() const {
            return util::StringBuilder( 1024 ) << *this;
        }

        friend std::ostream& operator<<( std::ostream& out, const Percept& percept ) {
            out << '[';
            percept.print( out );
            out << ']';
            return out;
        }

    protected:
        /**
         * This method should return a user-friendly string that represents the percept
         */
        virtual void print( std::ostream& ) const = 0;

        Percept() = default;

        /**
         * This method is protected to prevent slicing
         */
        Percept( const Percept& ) = default;
    };

}
