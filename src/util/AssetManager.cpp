#include "AssetManager.hpp"
#include <algorithm>
#include <string>
#include <memory>
#include "util/type_traits.hpp"
#include "asset_manager/DataAccessLayer.hpp"

using namespace std::string_view_literals;
using namespace aima::util::AssetManager;
using std::string_view;
using ::aima::util::StringBuilder;

log4cplus::Logger& detail::GetLogger() {
    static auto logger = log4cplus::Logger::getInstance( "AssetManager" );
    return logger;
}

void* detail::get( std::string_view tag, std::type_index type ) {
    auto info = DataAccessLayer::getAsset( tag, type );

    if ( !info.id ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION(
                AssetManagerException{}
                        << WrappedExceptionMessage( util::StringBuilder( 256 ) << "asset \"" << tag << "\" of type "
                                                                               << boost::core::demangle( type.name())
                                                                               << " not found in database" ));
    }

    if ( !info.quantity ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION(
                AssetManagerException{}
                        << Because( util::StringBuilder( 256 ) << "asset \"" << tag << "\" of type "
                                                               << boost::core::demangle( type.name())
                                                               << " was freed" ));
    }

    return storage().at( type.hash_code()).at( info.index );
}

detail::Storage& ::aima::util::AssetManager::detail::storage() {
    static Storage storage;
    return storage;
}
