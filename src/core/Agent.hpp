#pragma once

#include <memory>
#include <atomic>
#include "util/UniqueIdMixin.hpp" // IWYU pragma: export


namespace aima::core {
    class Percept;

    class Action;

    /**
     * Represents an agent. Sub-classes are intended to hold the logic (and state, if any) of a given agent type.
     */
    class Agent : public util::UniqueIdMixin<Agent, true> {
    public:
        Agent() = default;

        Agent& operator=( const Agent& other ) = delete;

        Agent& operator=( Agent&& ) = delete;

        virtual std::unique_ptr<Agent> clone() const = 0;

        virtual const Action& execute( const Percept& percept ) = 0;

        virtual bool isAlive() const { return alive; }

        virtual void isAlive( bool alive ) { this->alive = alive; }

        virtual ~Agent() = default;

    protected:
        Agent( const Agent& other ) : alive( other.alive.load()) {}

    private:
        std::atomic_bool alive = true;
    };
}
