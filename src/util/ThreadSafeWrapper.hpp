#pragma once

#include <shared_mutex>
#include <functional>
#include <string>
#include <string_view>
#include <mutex>
#include <utility>
#include <type_traits>
#include <boost/type_traits/has_operator.hpp> // IWYU pragma: keep
#include "util/type_traits.hpp"               // IWYU pragma: keep

namespace aima::util {
    struct CleanReadTag {};
    struct DirtyReadTag {};
    struct WriteTag {};

    namespace detail {
        template< typename T, typename ... Args >
        using result_if_invocable = std::enable_if_t<std::is_invocable_v<T, Args...>, std::invoke_result_t<T, Args...>>;
    }

    /**
     * This is a wrapper around an object that is meant to provide thread safe access while allowing
     * readers non-blocking access. It does this by maintaining two copies of the object.
     * All writes to the primary copy will eventually be duplicated for the secondary copy.
     * Reading can be done in two modes: clean read and dirty read.
     * If another thread is in middle of writing to the object, then a clean read will block until the writer is
     * finished.
     * A dirty read will read from whichever copy is currently available. This is guaranteed not to block, but it might
     * not be up to date
     * @tparam T
     */
    template< typename T >
    class ThreadSafeWrapper {
    public:
        static_assert( std::is_copy_constructible_v<T> );
        using type = T;

        explicit ThreadSafeWrapper( const T& t ) : primary( t ), secondary( t ) {}

        template< std::enable_if_t<std::is_move_constructible_v<T>, int> = 0 >
        ThreadSafeWrapper( T&& t ) : // NOLINT(google-explicit-constructor)
                primary( std::move( t )), secondary( primary ) {}

        ThreadSafeWrapper( const ThreadSafeWrapper& other ) : primary( other.cleanRead()), secondary( primary ) {}

        template< std::enable_if_t<std::is_move_constructible_v<T>, int> = 0 >
        ThreadSafeWrapper( ThreadSafeWrapper&& other ) noexcept( false ) : // NOLINT(google-explicit-constructor)
                primary( other.moveRead()), secondary( primary ) {}

        template< typename ... Args >
        explicit ThreadSafeWrapper( Args&& ... args ) : primary( std::forward( args )... ), secondary( primary ) {}

        template< typename Function >
        std::invoke_result_t<Function, T&> access( Function&& function ) {
            // If there was an exception in the function that the user passed in, we want to try and sync anyways,
            // but swallow any exceptions and re-throw the original exception.
            // If the user's function doesn't throw, but there's an exception while syncing, we just let it bubble up.

            LockHelper lockForWritingToPrimary( primaryMutex, secondaryMutex );

            struct SyncHelper {
                ~SyncHelper() noexcept( false ) {
                    try {
                        T temp = t.primary;
                        lockForWritingToPrimary.unlock();

                        LockHelper lockForWritingToSecondary( t.secondaryMutex, t.primaryMutex );
                        if constexpr ( std::is_move_assignable_v<T> ) t.secondary = std::move( temp );
                        else t.secondary = temp;
                    }
                    catch ( ... ) {
                        if ( std::uncaught_exceptions() == exceptionCount ) throw;
                    }
                }

                ThreadSafeWrapper& t;
                LockHelper       & lockForWritingToPrimary;
                int exceptionCount = std::uncaught_exceptions();
            }          syncHelper{ *this, lockForWritingToPrimary };

            return std::forward<Function>( function )( primary );
        }

        template< typename Function >
        std::invoke_result_t<Function, const T&> sharedAccessClean( Function&& function ) const {
            std::shared_lock lock( primaryMutex );
            return std::forward<Function>( function )( primary );
        }

        template< typename Function >
        std::invoke_result_t<Function, const T&> sharedAccessDirty( Function function ) const {
            std::shared_lock primaryLock( primaryMutex, std::try_to_lock );
            if ( primaryLock ) return std::forward<Function>( function )( primary );

            std::shared_lock secondaryLock( secondaryMutex, std::try_to_lock );
            if ( secondaryLock ) return std::forward<Function>( function )( secondary );

            while ( true ) {
                if ( primaryLock.try_lock()) return std::forward<Function>( function )( primary );
                if ( secondaryLock.try_lock()) return std::forward<Function>( function )( secondary );
            }
        }

        T cleanRead() const {
            return sharedAccessClean( []( const T& t ) { return t; } );
        }

        T dirtyRead() const {
            return sharedAccessDirty( []( const T& t ) { return t; } );
        }

        T&& moveRead() {
            struct SyncHelper {
                ~SyncHelper() try {
                    LockHelper lock( t.secondaryMutex, t.primaryMutex );
                    t.secondary = t.primary;
                }
                catch ( ... ) {}

                ThreadSafeWrapper& t;
            } syncHelper{ *this };

            LockHelper lock( primaryMutex, secondaryMutex );
            return std::move( primary );
        }

        template< typename SFINAE = ThreadSafeWrapper& >
        std::enable_if_t<boost::has_pre_increment<T>::value, SFINAE>
        operator++() {
            access( []( T& t ) {
                ++t;
            } );
            return *this;
        }

        template< typename SFINAE = ThreadSafeWrapper >
        std::enable_if_t<boost::has_post_increment<T>::value, SFINAE> // NOLINT(cert-dcl21-cpp)
        operator++( int ) const& {
            return sharedAccessClean( []( const T& t ) {
                T temp = t;
                return ++temp;
            } );
        }

        template< typename SFINAE = ThreadSafeWrapper& >
        std::enable_if_t<boost::has_pre_decrement<T>::value, SFINAE> operator--() {
            access( []( T& t ) {
                --t;
            } );
            return *this;
        }

        template< typename SFINAE = ThreadSafeWrapper >
        std::enable_if_t<boost::has_post_decrement<T>::value, SFINAE> // NOLINT(cert-dcl21-cpp)
        operator--( int ) const& {
            return sharedAccessClean( []( const T& t ) {
                T temp = t;
                return --temp;
            } );
        }

        template< typename ... Args >
        auto operator()( Args&& ... args ) const ->
        std::enable_if_t<!std::is_same_v<CleanReadTag, first_entity<Args...>> &&
                         !std::is_same_v<DirtyReadTag, first_entity<Args...>> &&
                         !std::is_same_v<WriteTag, first_entity<Args...>>,
                         detail::result_if_invocable<const T, Args...>> {
            return operator()( CleanReadTag{}, std::forward<Args>( args )... );
        }

        template< typename ... Args >
        detail::result_if_invocable<const T, Args...>
        operator()( CleanReadTag, Args&& ... args ) const {
            return sharedAccessClean( [ & ]( const T& t ) { std::invoke( t, args... ); } );
        }

        template< typename ... Args >
        detail::result_if_invocable<const T, Args...>
        operator()( DirtyReadTag, Args&& ... args ) const {
            return sharedAccessDirty( [ & ]( const T& t ) { std::invoke( t, args... ); } );
        }

        template< typename ... Args >
        detail::result_if_invocable<T, Args...>
        operator()( WriteTag, Args&& ... args ) {
            return access( [ & ]( T& t ) { std::invoke( t, args... ); } );
        }

        explicit operator T() const { return cleanRead(); }

#define ASSIGNMENT_OPERATOR( OP, SFINAE )                    \
        template< typename U >                               \
        std::enable_if_t<SFINAE<T, const U&>::value, ThreadSafeWrapper&> \
        inline operator OP ( const U& other ) {              \
            access( [ & ]( T& t ) {                          \
                t OP other;                                  \
            } );                                             \
            return *this;                                    \
        }                                                    \
        template< typename U >                               \
        std::enable_if_t<not SFINAE<T&, const U&>::value, ThreadSafeWrapper&> \
        inline operator OP ( const U& other ) = delete;      \
        template< typename U >                               \
        std::enable_if_t<SFINAE<T&, U&&>::value, ThreadSafeWrapper&> \
        operator OP ( U&& other ) {                          \
            access( [ & ]( T& t ) {                          \
                t OP other;                                  \
            } );                                             \
            return *this;                                    \
        }                                                    \
        template< typename U >                               \
        std::enable_if_t<SFINAE<T&, const U&>::value, ThreadSafeWrapper&> \
        inline operator OP ( const ThreadSafeWrapper<U>& other ) {        \
            access( [ & ]( T& t ) {                          \
                t OP other.cleanRead();                      \
            } );                                             \
            return *this;                                    \
        }                                                    \
        template< typename U >                               \
        std::enable_if_t<not SFINAE<T&, const U&>::value, ThreadSafeWrapper&> \
        inline operator OP ( const ThreadSafeWrapper<U>& other ) = delete;    \
        template< typename U >                               \
        std::enable_if_t<SFINAE<T&, U&&>::value, ThreadSafeWrapper&> \
        inline operator OP ( ThreadSafeWrapper<U>&& other ) {        \
            access( [ & ]( T& t ) {                          \
                t OP other.moveRead();                       \
            } );                                             \
            return *this;                                    \
        }

        ASSIGNMENT_OPERATOR( // NOLINT(cppcoreguidelines-c-copy-assignment-signature,misc-unconventional-assign-operator)
                =,
                std::is_assignable )

        ASSIGNMENT_OPERATOR( +=, boost::has_plus_assign )

        ASSIGNMENT_OPERATOR( -=, boost::has_minus_assign )

        ASSIGNMENT_OPERATOR( *=, boost::has_multiplies_assign )

        ASSIGNMENT_OPERATOR( /=, boost::has_divides_assign )

        ASSIGNMENT_OPERATOR( %=, boost::has_modulus_assign )

        ASSIGNMENT_OPERATOR( <<=, boost::has_left_shift_assign )

        ASSIGNMENT_OPERATOR( >>=, boost::has_right_shift_assign )

        ASSIGNMENT_OPERATOR( &=, boost::has_bit_and_assign )

        ASSIGNMENT_OPERATOR( ^=, boost::has_bit_xor_assign )

        ASSIGNMENT_OPERATOR( |=, boost::has_bit_or_assign )

#undef ASSIGNMENT_OPERATOR

    private:
        class LockHelper {
        public:
            LockHelper( std::shared_mutex& writtenTo, std::shared_mutex& heldOpen )
                    : writtenTo( writtenTo ), heldOpen( heldOpen ) {
                heldOpen.lock_shared();
                writtenTo.lock();
            }

            void unlock() {
                writtenTo.unlock();
                heldOpen.unlock_shared();
                locked = false;
            }

            ~LockHelper() { if ( locked ) unlock(); }

        private:
            std::shared_mutex& writtenTo;
            std::shared_mutex& heldOpen;
            bool locked = true;
        };

        mutable std::shared_mutex primaryMutex;
        mutable std::shared_mutex secondaryMutex;
        T                         primary;
        T                         secondary;
    };


#define BINARY_OPERATOR( OP )                                     \
    template< typename T, typename U >                            \
    auto operator OP ( const ThreadSafeWrapper<T>& a, const ThreadSafeWrapper<U>& b ) -> decltype( a OP b ) { \
        return a.sharedAccessClean( [ &b ]( auto& aUnpacked ) {   \
            return b.sharedAccessClean( [ &aUnpacked ]( auto& bUnpacked ) { \
                return aUnpacked OP bUnpacked;                    \
            } );                                                  \
        } );                                                      \
    }                                                             \
    template< typename T, typename U >                            \
    auto operator OP ( const ThreadSafeWrapper<T>& a, const U& b ) -> decltype( a OP b ) { \
        return a.sharedAccessClean( [ &b ]( auto& a ) {           \
            return a OP b;                                        \
        } );                                                      \
    }

    BINARY_OPERATOR( + )

    BINARY_OPERATOR( - )

    BINARY_OPERATOR( * )

    BINARY_OPERATOR( / )

    BINARY_OPERATOR( % )

    BINARY_OPERATOR( > )

    BINARY_OPERATOR( >= )

    BINARY_OPERATOR( < )

    BINARY_OPERATOR( <= )

    BINARY_OPERATOR( == )

    BINARY_OPERATOR( != )

    BINARY_OPERATOR( & )

    BINARY_OPERATOR( | )

    BINARY_OPERATOR( ^ )

    BINARY_OPERATOR( << )

    BINARY_OPERATOR( >> )

#undef BINARY_OPERATOR

    extern template
    class ThreadSafeWrapper<std::string>;

    extern template
    class ThreadSafeWrapper<std::string_view>;

    extern template
    class ThreadSafeWrapper<bool>;

    extern template
    class ThreadSafeWrapper<double>;

    using ThreadSafeString     = ThreadSafeWrapper<std::string>;
    using ThreadSafeStringView = ThreadSafeWrapper<std::string_view>;
    using ThreadSafeBool       = ThreadSafeWrapper<bool>;
    using ThreadSafeDouble     = ThreadSafeWrapper<double>;
}
