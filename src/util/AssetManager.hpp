#pragma once

#include <utility>
#include <string_view>
#include <typeindex>
#include <filesystem>
#include <boost/core/typeinfo.hpp>
#include "define_logger.hpp"
#include "StringBuilder.hpp"
#include "asset_manager/AssetManagerException.hpp"
#include "TypeErasedContainer.hpp"
#include "asset_manager/AssetHandle.hpp"

namespace aima::util::AssetManager {
    namespace detail {
        log4cplus::Logger& GetLogger();

        void* get( std::string_view tag, std::type_index type );

        using Storage = std::unordered_map<size_t, TypeErasedContainer>;

        Storage& storage();
    }

    void setAssetDir( std::filesystem::path path );

    template< typename T >
    std::function<void( const std::filesystem::path&, void* )> assetFactory() {
        static_assert( std::is_constructible_v<T, std::filesystem::path>,
                       "Types that cannot be constructed from a std::filesystem::path "
                       "need to specialize the assetFactory method" );
        return []( const std::filesystem::path& path, void* p ) {
            new( p ) T( path );
        };
    }

    template< typename T >
    AssetHandle add( const std::filesystem::path& path, std::string_view tag ) try {
        static_assert( std::is_invocable_v<decltype( assetFactory<T>()), std::filesystem::path, void*> );

        using detail::GetLogger;
        using detail::storage;

        TRACE;
        LOG4CPLUS_DEBUG( GetLogger(),
                         "Adding asset \"" << tag << "\" type " << boost::core::demangle( typeid( T ).name())
                                           << " from " << path );

        size_t typeHash = typeid( T ).hash_code();
        if ( storage().find( typeHash ) == storage().end())
            storage().emplace( std::make_pair( typeHash, TypeErasedContainer::make<T>()));

        return AssetHandle::make( path, tag, typeHash, std::move( assetFactory<T>()));
    }
    catch ( AssetManagerException& e ) {
        e << Because( util::StringBuilder( 256 ) << "Failed to add asset of type "
                                                 << boost::core::demangle( typeid( T ).name())
                                                 << " from " << path << " tagged \"" << tag << '"' );
        throw;
    }

    template< typename T >
    T& get( std::string_view tag ) try {
        using detail::GetLogger;
        TRACE;
        LOG4CPLUS_DEBUG( GetLogger(), "Getting asset \"" << tag << '"' );

        return *static_cast<T*>(detail::get( tag, typeid( T )));
    }
    catch ( AssetManagerException& e ) {
        e << Because( util::StringBuilder( 256 ) << "Failed to get asset of type "
                                                 << boost::core::demangle( typeid( T ).name())
                                                 << " tagged \"" << tag << '"' );
        throw;
    }


}
