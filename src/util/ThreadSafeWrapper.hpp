#pragma once

#include <shared_mutex>
#include <functional>

namespace aima::util {
    struct CleanReadTag {};
    struct DirtyReadTag {};
    struct WriteTag {};

    template< typename T >
    class ThreadSafeWrapper {
    public:
        ThreadSafeWrapper( const T& t ) : primary( t ), secondary( t ) {} // NOLINT(google-explicit-constructor)

        ThreadSafeWrapper( T&& t ) : // NOLINT(google-explicit-constructor)
                primary( std::move( t )), secondary( primary ) {}

        ThreadSafeWrapper( const ThreadSafeWrapper& other ) : primary( other.cleanRead()), secondary( primary ) {}

        ThreadSafeWrapper( ThreadSafeWrapper&& other ) noexcept( false ) :
                primary( other.moveRead()), secondary( primary ) {}

        template< typename ... Args >
        explicit ThreadSafeWrapper( Args ... args ): primary( std::forward( args )... ), secondary( primary ) {}

        inline void access( std::function<void( T& )> function ) {
            this->access<void>( function );
        }

        template< typename U >
        U access( std::function<U( T& )> function ) {
            LockHelper lockForWritingToPrimary( primaryMutex, secondaryMutex );

            bool       triedToSync = false;
            const auto synchronize = [ & ]() {
                T value = primary;
                lockForWritingToPrimary.unlock();

                LockHelper lockForWritingToSecondary( secondaryMutex, primaryMutex );
                triedToSync = true;
                secondary   = value;
            };

            try {
                if constexpr ( std::is_void_v<U> ) {
                    function( primary );
                    synchronize();
                    return;
                }
                else {
                    U result = function( primary );
                    synchronize();
                    return result;
                }
            }
            catch ( ... ) {
                try {
                    if ( !triedToSync ) synchronize();
                }
                catch ( ... ) {}
                throw;
            }
        }

        template< typename U >
        U sharedAccessClean( std::function<U( const T& )> function ) const {
            std::shared_lock lock( primaryMutex );
            return function( primary );
        }

        template< typename U >
        U sharedAccessDirty( std::function<U( const T& )> function ) const {
            std::shared_lock primaryLock( primaryMutex, std::try_to_lock );
            if ( primaryLock ) return function( primary );
            std::shared_lock secondaryLock( secondaryMutex, std::try_to_lock );
            if ( secondaryLock ) return function( secondary );
            while ( true ) {
                if ( primaryLock.try_lock()) return function( primary );
                if ( secondaryLock.try_lock()) return function( secondary );
            }
        }

        T cleanRead() const {
            return sharedAccessClean( std::function( []( const T& t ) { return t; } ));
        }

        T dirtyRead() const {
            return sharedAccessDirty( std::function( []( const T& t ) { return t; } ));
        }

        T&& moveRead() {
            return access( [ this ]( T& t ) {
                return std::move( t );
            } );
        }

        ThreadSafeWrapper& operator++() {
            access( []( T& t ) {
                ++t;
            } );
            return *this;
        }

        ThreadSafeWrapper operator++( int ) const& { // NOLINT(cert-dcl21-cpp)
            return sharedAccessClean( []( const T& t ) {
                return t++;
            } );
        }

        ThreadSafeWrapper& operator--() {
            access( []( T& t ) {
                --t;
            } );
            return *this;
        }

        ThreadSafeWrapper operator--( int ) const& { // NOLINT(cert-dcl21-cpp)
            return sharedAccessClean( []( const T& t ) {
                return t--;
            } );
        }

        template< typename ... Args >
        auto operator()( Args ... args ) const {
            return operator()( CleanReadTag{}, std::forward( args )... );
        }

        template< typename ... Args >
        auto operator()( CleanReadTag, Args ... args ) const {
            return sharedAccessClean( [ &args... ]( const T& t ) { t( args... ); } );
        }

        template< typename ... Args >
        auto operator()( DirtyReadTag, Args ... args ) const {
            return sharedAccessDirty( [ &args... ]( const T& t ) { t( args... ); } );
        }

        template< typename ... Args >
        auto operator()( WriteTag, Args ... args ) {
            return access( [ &args... ]( T& t ) { t( args... ); } );
        }

#define ASSIGNMENT_OPERATOR( OP )                                 \
        template< typename U >                                    \
        ThreadSafeWrapper& operator OP ( const ThreadSafeWrapper<U>& other ) {  \
            access( std::function( [ this, &other ]( T& t ) {     \
                t OP other.cleanRead();                           \
            } ));                                                 \
            return *this;                                         \
        }                                                         \
        template< typename U >                                    \
        ThreadSafeWrapper& operator OP ( ThreadSafeWrapper<U>&& other ) { \
            access( std::function( [ this, &other ]( T& t ) {     \
                t OP other.moveRead();                            \
            } ));                                                 \
            return *this;                                         \
        }                                                         \
        template< typename U >                                    \
        ThreadSafeWrapper& operator OP ( const U& other ) {       \
            access( std::function( [ this, &other ]( T& t ) {     \
                t OP other;                                       \
            } ));                                                 \
            return *this;                                         \
        }                                                         \
        template< typename U >                                    \
        ThreadSafeWrapper& operator OP ( U&& other ) {            \
            access( std::function( [ this, &other ]( T& t ) {     \
                t OP std::move( other );                          \
            } ));                                                 \
            return *this;                                         \
        }

        ASSIGNMENT_OPERATOR( = )

        ASSIGNMENT_OPERATOR( += )

        ASSIGNMENT_OPERATOR( -= )

        ASSIGNMENT_OPERATOR( *= )

        ASSIGNMENT_OPERATOR( /= )

        ASSIGNMENT_OPERATOR( %= )

        ASSIGNMENT_OPERATOR( <<= )

        ASSIGNMENT_OPERATOR( >>= )

        ASSIGNMENT_OPERATOR( &= )

        ASSIGNMENT_OPERATOR( ^= )

        ASSIGNMENT_OPERATOR( |= )

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

            ~LockHelper() {
                if ( locked ) unlock();
            }

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
    auto operator OP ( const ThreadSafeWrapper<T>& a, const ThreadSafeWrapper<U>& b ) { \
        return a.sharedAccessClean( [ &b ]( auto& aUnpacked ) {   \
            return b.sharedAccessClean( [ &aUnpacked ]( auto& bUnpacked ) { \
                return aUnpacked OP bUnpacked;                    \
            } );                                                  \
        } );                                                      \
    }                                                             \
    template< typename T, typename U >                            \
    auto operator OP ( const ThreadSafeWrapper<T>& a, const U& b ) { \
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
}
