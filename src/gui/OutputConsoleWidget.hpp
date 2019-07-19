#pragma once

#include <string>
#include <sstream>
#include <type_traits>
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
    class OutputConsoleWidget final : public std::ostream {
    public:
        OutputConsoleWidget();

        OutputConsoleWidget( const OutputConsoleWidget& ) = delete;

        OutputConsoleWidget( OutputConsoleWidget&& other ) noexcept;

        OutputConsoleWidget& operator=( const OutputConsoleWidget& ) = delete;

        OutputConsoleWidget& operator=( OutputConsoleWidget&& ) noexcept;

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
        detail::OutputConsoleBuffer& buffer() {
            return *reinterpret_cast<detail::OutputConsoleBuffer*>(&bufferStorage);
        }

        util::ThreadSafeString                                         display;
        util::ThreadSafeBool                                           outputPastDisplay;
        std::aligned_storage_t<sizeof( detail::OutputConsoleBuffer ),
                               alignof( detail::OutputConsoleBuffer )> bufferStorage;
    };
}
