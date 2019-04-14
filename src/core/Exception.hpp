#pragma once

#include <stdexcept>                 // IWYU pragma: export
#include <string>
#include <string_view>
#include <boost/exception/info.hpp>  // IWYU pragma: export
#include <boost/throw_exception.hpp> // IWYU pragma: export
#include <boost/exception/get_error_info.hpp> // IWYU pragma: export
#include <log4cplus/loggingmacros.h> // IWYU pragma: export


namespace aima::core::exception {
    using AgentType               = boost::error_info<struct agent, std::string_view>;
    using EnvironmentType         = boost::error_info<struct environment, std::string_view>;
    using EnvironmentViewType     = boost::error_info<struct environment_view, std::string_view>;
    using DemoType                = boost::error_info<struct demo, std::string_view>;
    using AppType                 = boost::error_info<struct app, std::string_view>;
    using Because                 = boost::error_info<struct cause, std::string>;
    using PerceptInfo             = boost::error_info<struct percept, std::string>;
    using WrappedExceptionType    = boost::error_info<struct wrapped_exception_type, std::string>;
    using WrappedExceptionMessage = boost::error_info<struct wrapped_exception_message, std::string>;

    class Exception : public virtual std::exception, public virtual boost::exception {
    public:
        const char* what() const noexcept override {
            const std::string* const info = boost::get_error_info<Because>( *this );
            return info ? info->c_str() : std::exception::what();
        }
    };
}

#define AIMA_THROW_EXCEPTION( exception ) do { \
    auto y = exception;                        \
    LOG4CPLUS_ERROR( GetLogger(), "Exception thrown: " << y.what()); \
    BOOST_THROW_EXCEPTION(y);                  \
} while(false)
