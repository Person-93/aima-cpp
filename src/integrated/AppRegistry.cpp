#include "AppRegistry.hpp"
#include <utility>
#include <string_view>
#include "forward.hpp"


using namespace aima::IntegratedRunner::detail;

static AppRegistry registry;

void ::aima::IntegratedRunner::detail::registerApp( std::string_view name, AppFactory factory ) {
    registry.emplace_back( name, std::move( factory ));
}

const AppRegistry& ::aima::IntegratedRunner::detail::appRegistry() {
    return registry;
}
