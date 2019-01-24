#pragma once

#include "EnvironmentObject.hpp"

namespace aima::core {
    class Percept;

    class Action;

    class Agent : public EnvironmentObject {
    public:
        virtual const Action& execute( const Percept& percept ) = 0;

        virtual bool isAlive() const { return alive; }

        virtual void setAlive( bool alive ) { this->alive = alive; }

    private:
        bool alive = true;
    };
}
