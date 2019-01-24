#pragma once

#include <atomic>

namespace aima::util {
    template< typename T, bool visible = false >
    class UniqueIdMixin;

    template< class T >
    class UniqueIdVisibleBase;

    template< class T >
    class UniqueIdBase {
        UniqueIdBase() noexcept = default;

        friend class UniqueIdVisibleBase<T>;

        static uint_least32_t generateId() noexcept {
            static std::atomic_uint_least32_t counter = 0;
            return ++counter;
        }

        uint_least32_t unique_id_ = generateId();

        friend class UniqueIdMixin<T, true>;

        friend class UniqueIdMixin<T, false>;

        friend class UniqueIdVisibleBase<T>;

    public:
        /** A function object that can be used for sorting in maps and sets */
        struct less {
            bool operator()( const T& lhs, const T& rhs ) const noexcept {
                return lhs.unique_id_ < rhs.unique_id_;
            }
        };

        UniqueIdBase( const UniqueIdBase<T>& ) noexcept = default;

        UniqueIdBase( UniqueIdBase<T>&& ) noexcept = default;

        UniqueIdBase& operator=( const UniqueIdBase<T>& ) noexcept = default;

        UniqueIdBase& operator=( UniqueIdBase<T>&& ) noexcept = default;

        virtual ~UniqueIdBase() = default;
    };

    template< class T >
    class UniqueIdVisibleBase : public UniqueIdBase<T> {
        friend class UniqueIdMixin<T, true>;

    public:
        /**
         * @return The unique id
         */
        uint_least32_t id() const { return this->unique_id_; }
    };

/**
 * A mixin that supplies a sorting function for use in sets and maps
 *
 * Here is an example usage.
 *
 * @code
 * class Thing : UniqueId<Thing> {};
 *
 * std::set<Thing, Thing::less> things;
 * @endcode
 *
 * @tparam T- The derived type. Uses CRTP.
 * @tparam visible- Should the unique ID be visible
 */
    template< class T, bool visible >
    class UniqueIdMixin : public std::conditional_t<visible, UniqueIdVisibleBase<T>, UniqueIdBase<T>> {
        UniqueIdMixin() noexcept = default;

        friend T;
    };
}
