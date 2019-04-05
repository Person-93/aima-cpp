#include "DemoRegistry.hpp"
#include <utility>
#include <string_view>
#include "forward.hpp"


using namespace aima::IntegratedRunner::detail;

static DemoRegistry registry;

void ::aima::IntegratedRunner::detail::registerDemo( std::string_view name, IntegratedDemoFactory factory ) {
    registry.emplace_back( name, std::move( factory ));
}

const DemoRegistry& ::aima::IntegratedRunner::detail::demoRegistry() {
    return registry;
}