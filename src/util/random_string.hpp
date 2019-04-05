#pragma once

#include <string>

namespace aima::util {
    /**
     * Returns a random string
     * @link https://stackoverflow.com/a/24586587/2036673
     * @param length
     * @return
     */
    std::string random_string( std::string::size_type length );
}
