#pragma once

#include "core/Agent.hpp"

namespace aima::vacuum {
    class RandomnessBasedAgent final : public core::Agent {
    public:
        RandomnessBasedAgent( unsigned long x, unsigned long y ) : x{ x }, y{ y } {}

        std::unique_ptr<Agent> clone() const override {
            return std::unique_ptr<Agent>{ new RandomnessBasedAgent{ *this }};
        }

        std::unique_ptr<core::Action> execute( const core::Percept& percept ) override;

    private:
        unsigned long x;
        unsigned long y;
    };
}
