#pragma once

#include <atomic>
#include <string_view>
#include <cstddef>
#include <memory>
#include <utility>
#include <mutex>
#include "EnvironmentDestroyedView.hpp"

// IWYU pragma: no_include "App.hpp"

namespace aima::core { class App; } // IWYU pragma: keep

namespace aima::IntegratedRunner {
    namespace detail {
        /**
         * This class owns the memory in which heap-allocated Apps live
         */
        class ControlBlock : private core::EnvironmentDestroyedView {
        public:
            using App         = aima::core::App;
            using InternalPtr = App*;

            explicit ControlBlock( App* ptr );

            App* operator*() { return internalPtr; }

            const App* operator*() const { return internalPtr; }

            App* operator->() { return operator*(); }

            const App* operator->() const { return operator*(); }

            void point() noexcept;

            void unPoint() noexcept;

            void addWeak() noexcept;

            void removeWeak() noexcept;

            bool isAlive() const noexcept { return ptrCount > 0; }

        private:
            ~ControlBlock() = default;

            void notify() noexcept override;

            void tryErase() noexcept;

            std::mutex       mutex;
            InternalPtr      internalPtr;
            std::atomic_uint ptrCount          = 1;
            std::atomic_uint weakPtrCount      = 0;
            std::atomic_bool heldByEnvironment = true;
        };
    }

    class AppPtr;

    /**
     * Weak pointer for the app smart pointer
     */
    class WeakAppPtr {
    public:
        constexpr WeakAppPtr() noexcept : pControlBlock( nullptr ) {}

        explicit WeakAppPtr( const AppPtr& ptr ) noexcept;

        WeakAppPtr( const WeakAppPtr& other ) noexcept;

        WeakAppPtr( WeakAppPtr&& other ) noexcept;

        WeakAppPtr& operator=( const WeakAppPtr& other ) noexcept;

        WeakAppPtr& operator=( WeakAppPtr&& other ) noexcept;

        WeakAppPtr& operator=( const AppPtr& other ) noexcept;

        WeakAppPtr& operator=( AppPtr&& other ) noexcept;

        WeakAppPtr& operator=( std::nullptr_t ) noexcept;

        ~WeakAppPtr();

        explicit operator bool();

        void reset() noexcept;

        AppPtr lock() noexcept;

        const AppPtr lock() const noexcept;

    private:
        friend AppPtr;

        detail::ControlBlock* pControlBlock;
    };

    /**
     * Smart pointer to point to App objects.
     * The normal std::shared_ptr does not suffice because the app must not be destructed until
     * the environment it contains is ready to stop.
     */
    class AppPtr {
    public:
        constexpr AppPtr() : pControlBlock( nullptr ) {}

        template< typename AppType >
        explicit AppPtr( AppType* p ) {
            static_assert( std::is_base_of_v<aima::core::App, AppType> );
            static_assert( std::is_convertible_v<AppType*, aima::core::App*>,
                           "Must publicly inherit from aima::util::App" );

            pControlBlock = p ? new detail::ControlBlock( p ) : nullptr;
        }

        template< typename AppType >
        explicit AppPtr( std::unique_ptr<AppType>&& p ) {
            static_assert( std::is_base_of_v<aima::core::App, AppType> );
            static_assert( std::is_convertible_v<AppType*, aima::core::App*>,
                           "Must publicly inherit from aima::util::App" );

            pControlBlock = p ? new detail::ControlBlock( p.release()) : nullptr;
        }

        AppPtr( const AppPtr& other ) noexcept : pControlBlock( other.pControlBlock ) {
            if ( pControlBlock ) pControlBlock->point();
        }

        AppPtr( AppPtr&& other ) noexcept : pControlBlock( other.pControlBlock ) {
            other.pControlBlock = nullptr;
        }

        AppPtr( const WeakAppPtr& weakAppPtr ) noexcept; // NOLINT(google-explicit-constructor)

        AppPtr( WeakAppPtr&& weakAppPtr ) noexcept; // NOLINT(google-explicit-constructor)

        AppPtr& operator=( const AppPtr& other ) noexcept;

        AppPtr& operator=( AppPtr&& other ) noexcept;

        AppPtr& operator=( const WeakAppPtr& weakAppPtr ) noexcept;

        AppPtr& operator=( WeakAppPtr&& weakAppPtr ) noexcept;

        template< typename AppType >
        AppPtr& operator=( AppType* p ) {
            reset( p );
            return *this;
        }

        AppPtr& operator=( std::nullptr_t ) noexcept;

        ~AppPtr() { reset(); }

        core::App* operator*() noexcept { return **pControlBlock; }

        const core::App* operator*() const noexcept { return **pControlBlock; }

        core::App* operator->() noexcept { return operator*(); }

        const core::App* operator->() const noexcept { return operator*(); }

        explicit operator bool() { return pControlBlock; }

        void reset() noexcept;

        template< typename AppType >
        void reset( AppType* p ) {
            if ( pControlBlock ) pControlBlock->unPoint();
            pControlBlock = p ? new detail::ControlBlock( p ) : nullptr;
        }

        WeakAppPtr getWeakPtr() const noexcept { return WeakAppPtr( *this ); }

    private:
        friend WeakAppPtr;

        detail::ControlBlock* pControlBlock;
    };

    template< typename AppType, typename ... Args >
    AppPtr makeAppPtr( Args&& ... args ) {
        static_assert( std::is_base_of_v<aima::core::App, AppType> );
        static_assert( std::is_convertible_v<AppType*, aima::core::App*>,
                       "Must publicly inherit from aima::util::App" );

        return AppPtr( new AppType( std::forward<Args>( args )... ));
    }
}
