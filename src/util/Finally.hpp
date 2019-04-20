#pragma once

namespace aima::util {
    template< typename T >
    class Finally {
    public:
        static_assert( std::is_nothrow_invocable_v<T> );

        explicit Finally( T&& t ) noexcept : t( t ), dismissed( false ) {}

        ~Finally() {
            if ( !dismissed ) std::invoke( std::forward<T>(t));
        }

        void dismiss() { dismissed = true; }

    private:
        T& t;
        bool dismissed;
    };
}
