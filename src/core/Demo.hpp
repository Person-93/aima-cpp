#pragma once

#include <iosfwd>
#include <memory>
#include "views/SimpleViewer.hpp"

namespace aima::util { class CancellationToken; }

namespace aima::core {
    class Environment;

    /**
     * Abstract class to hold all of the entities necessary for running a single demo
     *
     * It has two abstract methods, init() and environment()
     * The init method will be called after the environment view is set up, so any initialization that needs to be
     * visible to the viewer should be put in that method instead of the constructor
     */
    class Demo {
    public:
        explicit Demo( std::ostream& out );

        Demo( const Demo& ) = delete;

        Demo( Demo&& ) = delete;

        Demo& operator=( const Demo& ) = delete;

        Demo& operator=( Demo&& ) = delete;

        virtual ~Demo() = default;

        /**
         * This method runs the demo.
         * Optionally takes a cancellation token that the caller can use to cancel the demo if it is no longer needed
         * @param cancellationToken
         */
        void run( std::shared_ptr<const util::CancellationToken> cancellationToken = nullptr );

    protected:
        /**
         * This method should contain any initialization that needs to happen before the demo is run, it will be called
         * automatically from the run method
         */
        virtual void init() = 0;

        /**
         * This method should return the environment in which the demo is to be run
         * @return
         */
        virtual Environment& environment() noexcept = 0;

    private:
        viewer::SimpleViewer viewer;
    };
}
