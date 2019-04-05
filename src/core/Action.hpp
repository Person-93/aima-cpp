#pragma once

#include <string_view>
#include <iosfwd>
#include <memory>


namespace aima::core {
    /**
     * Represents a single action that can be taken by an agent.
     *
     * Typical use is for the environment class to have several static Action members and for the agent to choose one
     * based on its percept.
     */
    class Action {
    public:
        explicit Action( std::string_view name ) noexcept;

        Action( Action&& ) = delete;

        Action& operator=( const Action& ) = delete;

        Action& operator=( Action&& ) = delete;

        virtual ~Action() = default;

        virtual std::unique_ptr<Action> clone() const;

        std::string_view getName() const { return name; }

        virtual bool isNoOp() const;

        /**
         * An action object that represents not performing an action
         * @return
         */
        static const Action& noOp() noexcept;

        friend bool operator==( const Action& a, const Action& b ) {
            return a.getName() == b.getName();
        }

        friend std::ostream& operator<<( std::ostream& out, const Action& action );

    protected:
        /**
         * This method is protected to prevent slicing
         */
        Action( const Action& ) = default;

        virtual void print( std::ostream& out ) const;

    private:
        std::string_view name;
    };

    std::ostream& operator<<( std::ostream& out, const Action& action );
}
