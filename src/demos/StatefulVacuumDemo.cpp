#include "StatefulVacuumDemo.hpp"
#include "util/define_logger.hpp"


using aima::demos::StatefulVacuumDemo;
using aima::core::Environment;

DEFINE_LOGGER( StatefulVacuumDemo );

StatefulVacuumDemo::StatefulVacuumDemo( std::ostream& out ) : Demo( out ), agent( environment_ ) { TRACE; }

Environment& StatefulVacuumDemo::environment() noexcept { return environment_; }

void StatefulVacuumDemo::init() { environment_.addAgent( agent ); }
