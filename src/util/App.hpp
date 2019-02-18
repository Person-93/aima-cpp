#pragma once

namespace aima::util {
    /**
     * An App that can be run as a standalone or as part of an integrated app
     */
    class App {
    public:
        /**
         * Renders the app
         */
        virtual void render() = 0;
    };
}
