#pragma once

#include <log4cplus/loggingmacros.h>
#include "util/StringBuilder.hpp"
#include "core/Exception.hpp"


namespace aima::gui::detail {
    log4cplus::Logger& GetLogger();
}

#define IM_ASSERT( x )                         \
do {                                           \
    if ( !(x) ) {                              \
        using namespace aima::core::exception; \
        using aima::util::StringBuilder;       \
        using aima::gui::detail::GetLogger;    \
        AIMA_THROW_EXCEPTION(                  \
             Exception{} << Because( StringBuilder( 512 ) << "ImGui assertion failed: " << #x )); \
    }                                          \
} while ( false )
