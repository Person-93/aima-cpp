#pragma once

#include <vector>
#include <memory>
#include <utility>
#include "Point.hpp"

namespace aima::path_finder {
    struct SearchNode {
        std::shared_ptr<SearchNode> addChild( SearchNode* child ) {
            return children.emplace_back( child );
        }

        std::weak_ptr<SearchNode>                parent;
        Point                                    location;
        float                                    pathCost;
        std::vector<std::shared_ptr<SearchNode>> children;
    };
}
