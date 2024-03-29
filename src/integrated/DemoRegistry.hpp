#pragma once

#include "DemoRegistryEntry.hpp"
#include "util/Container.hpp"

namespace aima::IntegratedRunner::detail {
    using DemoRegistry = util::Container<DemoRegistryEntry>;

    const DemoRegistry& demoRegistry();
}
