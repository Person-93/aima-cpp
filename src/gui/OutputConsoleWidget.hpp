#pragma once

#include <string>
#include <sstream>

#include "util/ThreadSafeWrapper.hpp"
#include "gui/ImGuiWrapper.hpp"

namespace aima::gui {
    namespace detail {
        using ThreadSafeString = util::ThreadSafeWrapper<std::string>;
        using ThreadSafeBool = util::ThreadSafeWrapper<bool>;

        class OutputConsoleBuffer : public std::stringbuf {
        public:
            explicit OutputConsoleBuffer( ThreadSafeBool& outputPastDisplay, ThreadSafeString& display );

            void clear();

        protected:
            int sync() override;

        private:
            ThreadSafeString& display;
            ThreadSafeBool  & outputPastDisplay;
        };
    }

    class OutputConsoleWidget : public std::ostream {
    public:
        explicit OutputConsoleWidget( util::ThreadSafeWrapper<bool>& outputPastDisplay );

        bool render( ImGuiWrapper& gui, ImGuiWrapper::ChildWindowConfig& config, bool scrollEnabled );

        void clear();

    private:
        detail::ThreadSafeString    display;
        detail::OutputConsoleBuffer buffer;
        util::ThreadSafeWrapper<bool>& outputPastDisplay;
    };
}
