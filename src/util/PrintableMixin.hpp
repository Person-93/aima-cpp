#pragma once

#include <iosfwd>

namespace aima::util {
    template< typename T >
    class PrintableMixin {
    public:
        friend std::ostream& operator<<( std::ostream& ostream, const T& t ) {
            t.print( ostream );
            return ostream;
        }

    private:
        friend T;

        virtual ~PrintableMixin() = default;
    };
}
