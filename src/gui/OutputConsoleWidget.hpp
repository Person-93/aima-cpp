#pragma once

#include <string>
#include <sstream>
#include "util/ThreadSafeWrapper.hpp"


namespace aima::gui {
    namespace detail {
        class OutputConsoleBuffer : public std::stringbuf {
        public:
            OutputConsoleBuffer( util::ThreadSafeBool& outputPastDisplay, util::ThreadSafeString& display );

            OutputConsoleBuffer( const OutputConsoleBuffer& ) = delete;

            OutputConsoleBuffer( OutputConsoleBuffer&& ) noexcept( false ) = default;

            OutputConsoleBuffer& operator=( const OutputConsoleBuffer& ) = delete;

            OutputConsoleBuffer& operator=( OutputConsoleBuffer&& ) noexcept( false ) = default;

            void clear();

        protected:
            int sync() override;

        private:
            util::ThreadSafeString* display;
            util::ThreadSafeBool  * outputPastDisplay;
        };
    }

    struct ImGuiWrapper;
    struct ChildWindowConfig;

    /**
     * Represents a console-like widget that can be used for output.
     *
     * This class extends std::ostream, stream insertion can be used to display information in the widget.
     */
    class OutputConsoleWidget : public std::ostream {
    public:
        OutputConsoleWidget();

        OutputConsoleWidget( OutputConsoleWidget& ) = delete;

        OutputConsoleWidget( OutputConsoleWidget&& other ) noexcept( false );

        OutputConsoleWidget& operator=( OutputConsoleWidget& ) = delete;

        OutputConsoleWidget& operator=( OutputConsoleWidget&& other ) noexcept( false );

        /**
         * Renders the console.
         * @param gui Reference to ImGuiWrapper
         * @param config The window configuration
         * @param scrollEnabled Should the console auto-scroll if text was added?
         * @return
         */
        bool render( ImGuiWrapper& gui, ChildWindowConfig& config, bool scrollEnabled );

        /**
         * Erase all the text from the console
         */
        void clear();

    private:
        util::ThreadSafeString      display;
        util::ThreadSafeBool        outputPastDisplay;
        detail::OutputConsoleBuffer buffer;
    };
}
