#pragma once

#include <unordered_set>
#include <memory>
#include <utility>
#include <new>
#include "Point.hpp"

namespace aima::path_finder {
    /**
     * This class represents a node in a path search
     */
    class SearchNode {
    public:
        SearchNode( std::weak_ptr<SearchNode> parent, Point location, float pathCost ) :
                parent{ std::move( parent ) }, location{ location }, pathCost{ pathCost } {}

        SearchNode( const SearchNode& ) = delete;

        SearchNode( SearchNode&& ) = delete;

        const std::shared_ptr<SearchNode>& addChild( std::shared_ptr<SearchNode> child ) {
            return *children.emplace( std::move( child )).first;
        }

        std::weak_ptr<SearchNode>                       parent;
        Point                                           location;
        float                                           pathCost;
        std::unordered_set<std::shared_ptr<SearchNode>> children;

    private:
        friend class PathFinderAgent;

        void* operator new( size_t size ) { return ::operator new( size ); }

        void operator delete( void* p ) noexcept { ::operator delete( p ); }
    };
}
