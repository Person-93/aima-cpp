#include "GlobalVacuumEnvironmentPercept.hpp"

void aima::vacuum::GlobalVacuumEnvironmentPercept::print( std::ostream& ostream ) const {
    ostream << "AgentLocation=" << agentLocation << ", Environment=";
    for ( unsigned long i = 0; i < locations.size1(); ++i ) {
        for ( unsigned long j = 0; j < locations.size2(); ++j ) {
            ostream << Location{ i, j } << ": ";
            switch ( locations( i, j )) {
                case LocationState::CLEAN: ostream << "CLEAN";
                    break;
                case LocationState::DIRTY: ostream << "DIRTY";
            }
            ostream << ", ";
        }
    }
}
