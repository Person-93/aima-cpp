#pragma once

#include "core/Exception.hpp"

namespace aima::util::AssetManager {
    using core::exception::Because;
    using TypeErasedAction = boost::error_info<struct type_erased_action, std::string>;

    class AssetManagerException : public virtual ::aima::core::exception::Exception {};
}