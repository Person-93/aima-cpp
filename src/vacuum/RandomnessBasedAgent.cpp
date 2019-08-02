#include "RandomnessBasedAgent.hpp"
#include <random>
#include "util/define_logger.hpp"
#include "core/Exception.hpp"
#include "vacuum/LocalVacuumEnvironmentPercept.hpp"
#include "BasicVacuumEnvironment.hpp"
#include "core/Action.hpp"

using namespace aima::vacuum;

DEFINE_LOGGER( RandomnessBasedAgent )

namespace {
    enum class Choices { UP, DOWN, RIGHT, LEFT };
}

std::unique_ptr<aima::core::Action> RandomnessBasedAgent::execute( const core::Percept& percept ) {
    auto p                                         = dynamic_cast<const LocalVacuumEnvironmentPercept*>(&percept);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
    if ( !p ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because{ "Received percept of unrecognized type" } );
    }
#pragma clang diagnostic pop

    if ( p->agentLocationState == LocationState::DIRTY ) {
        return BasicVacuumEnvironment::ACTION_SUCK.clone();
    }
    static std::mt19937 engine{ std::random_device{}() };
    using T = std::underlying_type_t<Choices>;
    static std::uniform_int_distribution<T> distribution{ static_cast<T>(Choices::UP),
                                                          static_cast<T>(Choices::LEFT) };
    auto                                    choice = static_cast<Choices>(distribution( engine ));
    switch ( choice ) {
        case Choices::UP: return BasicVacuumEnvironment::ACTION_MOVE_UP.clone();
        case Choices::DOWN: return BasicVacuumEnvironment::ACTION_MOVE_DOWN.clone();
        case Choices::RIGHT: return BasicVacuumEnvironment::ACTION_MOVE_RIGHT.clone();
        case Choices::LEFT: return BasicVacuumEnvironment::ACTION_MOVE_LEFT.clone();
    }
}