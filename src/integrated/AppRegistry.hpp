#pragma once

#include <integrated/AppRegistryEntry.hpp>
#include "util/Container.hpp"


namespace aima::IntegratedRunner::detail {
    using AppRegistry = util::Container<AppRegistryEntry>;

    const AppRegistry& appRegistry();
}
