#pragma once

#include <utility>
#include <vector>

namespace aima::util {
    template< typename T >
    class Container {
    public:
        using Storage                = std::vector<T>;
        using value_type             = typename Storage::value_type;
        using allocator_type         = typename Storage::allocator_type;
        using size_type              = typename Storage::size_type;
        using difference_type        = typename Storage::difference_type;
        using reference              = typename Storage::reference;
        using const_reference        = typename Storage::const_reference;
        using pointer                = typename Storage::pointer;
        using const_pointer          = typename Storage::const_pointer;
        using iterator               = typename Storage::iterator;
        using const_iterator         = iterator;
        using reverse_iterator       = typename Storage::reverse_iterator;
        using const_reverse_iterator = reverse_iterator;

        Container() noexcept = default;

        Container( const Container& ) = delete;

        Container( Container&& ) = delete;

        Container& operator=( const Container& ) = delete;

        Container&& operator=( Container&& ) = delete;

        template< typename... Args >
        reference emplace_back( Args&& ... args ) { return storage.emplace_back( std::forward<Args>( args )... ); }

        reference at( size_type pos ) const { return storage.at( pos ); }

        [[nodiscard]]  reference operator[]( size_type pos ) const { return storage[ pos ]; }

        [[nodiscard]]  reference front() const { return storage.front(); }

        [[nodiscard]] reference back() const { return storage.back(); }

        [[nodiscard]] iterator begin() const { return storage.begin(); }

        [[nodiscard]]  iterator end() const { return storage.end(); }

        [[nodiscard]] bool empty() const noexcept { return storage.empty(); }

        [[nodiscard]] size_type size() const noexcept { return storage.size(); }

    private:
        mutable Storage storage;
    };
}
