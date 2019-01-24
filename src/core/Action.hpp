#pragma once

#include <iosfwd>
#include <map>

#include "util/AttributesMixin.hpp"

namespace aima::core {

    class Action : public util::AttributesMixin<Action> {
    public:
        explicit Action( std::string_view name );

        virtual ~Action() = default;

        virtual bool isNoOp() const;

        std::string_view getName() const;

        static const Action& noOp();
    };

    bool operator==( const Action& a, const Action& b );
}
