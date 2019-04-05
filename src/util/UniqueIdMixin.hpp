#pragma once

#include <atomic>
#include <functional>

namespace aima::util {
    template< typename T, bool visible = false >
    class UniqueIdMixin;

    namespace detail {
        template< class T >
        class UniqueIdVisibleBase;

        template< class T >
        class UniqueIdBase {
            UniqueIdBase() noexcept = default;

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

            /** A function object that can be used for hashing in unordered maps and sets */
            struct hash {
                std::size_t operator()( const T& t ) const noexcept {
                    return std::hash<std::decay_t<typeof( unique_id_ )>>()( t.unique_id_ );
                }
            };

            friend bool operator==( const T& a, const T& b ) noexcept {
                return a.unique_id_ == b.unique_id_;
            }

            UniqueIdBase( const UniqueIdBase<T>& ) noexcept = default;

            UniqueIdBase( UniqueIdBase<T>&& ) noexcept = default;

            UniqueIdBase& operator=( const UniqueIdBase<T>& ) noexcept = default;

            UniqueIdBase& operator=( UniqueIdBase<T>&& ) noexcept = default;

        protected:
            ~UniqueIdBase() = default;
        };

        template< class T >
        class UniqueIdVisibleBase : public UniqueIdBase<T> {
            friend class UniqueIdMixin<T, true>;

        public:
            /**
             * @return The unique id
             */
            uint_least32_t id() const { return this->unique_id_; }

        protected:
            ~UniqueIdVisibleBase() = default;
        };
    }

    /**
     * A mixin that supplies a sorting function for use in sets and maps
     *
     * Here is an example usage.
     *
     * @code
     * class Thing : UniqueId&lt;Thing&gt; {};
     *
     * std::set&lt;Thing, Thing::less&gt; things;
     * std::unordered_set&lt;Thing, Thing::hash&gt; unorderedThings;
     * @endcode
     *
     * @tparam T The derived type. Uses CRTP.
     * @tparam visible  Should the unique ID be visible?
     *              If this is true, the class will have a public method <code>id()</code>.
     *              Default is false.
     */
    template< class T, bool visible >
    class UniqueIdMixin : public std::conditional_t<visible, detail::UniqueIdVisibleBase<T>, detail::UniqueIdBase<T>> {
        UniqueIdMixin() noexcept = default;

        UniqueIdMixin( const UniqueIdMixin& ) noexcept = default;

        UniqueIdMixin( UniqueIdMixin&& ) noexcept = default;

        UniqueIdMixin& operator=( const UniqueIdMixin& ) noexcept = default;

        UniqueIdMixin& operator=( UniqueIdMixin&& ) noexcept = default;

        ~UniqueIdMixin() = default;

        friend T;
    };
}
