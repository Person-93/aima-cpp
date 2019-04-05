#include "TrivialVacuumDemo.hpp"
#include "util/define_logger.hpp"


using aima::demos::TrivialVacuumDemo;

DEFINE_LOGGER( TrivialVacuumDemo );

aima::demos::TrivialVacuumDemo::TrivialVacuumDemo( std::ostream& out ) : Demo( out ) { TRACE; }

aima::core::Environment& TrivialVacuumDemo::environment() noexcept { return environment_; }

void TrivialVacuumDemo::init() { environment_.addAgent( agent ); }

