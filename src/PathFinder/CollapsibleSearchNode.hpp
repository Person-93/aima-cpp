#pragma once

#include "SearchNode.hpp"

namespace aima::path_finder {
    struct CollapsibleSearchNode : public SearchNode {
        using SearchNode::SearchNode;
        float estimate{};
    };
}
