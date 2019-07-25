#pragma once

#include <unordered_set>
#include <memory>
#include <utility>
#include <new>
#include <vector>

namespace aima::traveling_salesman {
    /**
     * This class represents a node in a path search
     */
    class SearchNode {
    public:
        SearchNode( std::shared_ptr<SearchNode> parent, std::vector<size_t> visited, float pathCost ) :
                parent{ std::move( parent ) }, visited{ std::move( visited ) }, pathCost{ pathCost } {}

        SearchNode( const SearchNode& ) = delete;

        SearchNode( SearchNode&& ) = delete;

        std::shared_ptr<SearchNode> parent;
        std::vector<size_t>         visited;
        float                       pathCost;

    private:
        friend class IterativeLengtheningAgent;

        void* operator new( size_t size ) { return ::operator new( size ); }

        void operator delete( void* p ) noexcept { ::operator delete( p ); }
    };
}
