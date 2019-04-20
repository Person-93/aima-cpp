#pragma once

#include <utility>
#include <string_view>
#include <memory>
#include <functional>
#include <typeinfo>
#include <typeindex>
#include <filesystem>
#include <boost/core/typeinfo.hpp>
#include "define_logger.hpp"
#include "StringBuilder.hpp"
#include "asset_manager/AssetManagerException.hpp"
#include "asset_manager/TypeErasedContainer.hpp"

namespace aima::util::AssetManager {
    class AssetHandle;

    namespace detail {
        log4cplus::Logger& GetLogger();

        AssetHandle add( std::filesystem::path path,
                         std::string_view tag,
                         size_t typeHash,
                         std::function<void( std::filesystem::path, void* )> factory );


        void* get( std::string_view tag, std::type_index type );

        void freeAssetHandle( const AssetHandle& handle );

        void copyAssetHandle( const AssetHandle& handle );

        using Storage = std::unordered_map<size_t, TypeErasedContainer>;

        Storage& storage();
    }

    void initialize( const std::filesystem::path& dbFile = std::filesystem::path{} );

    class AssetHandle {
    public:
        AssetHandle() : id_( 0 ) {}

        AssetHandle( const AssetHandle& other );

        AssetHandle( AssetHandle&& other ) noexcept;

        AssetHandle& operator=( const AssetHandle& other );

        AssetHandle& operator=( AssetHandle&& other ) noexcept;

        ~AssetHandle() noexcept( false );

        size_t id() const noexcept { return id_; }

    private:
        friend AssetHandle detail::add( std::filesystem::path path,
                                        std::string_view tag,
                                        size_t typeHash,
                                        std::function<void( std::filesystem::path, void* )> factory );

        explicit AssetHandle( size_t id ) : id_( id ) {}

        size_t id_;
    };

    template< typename T >
    std::function<void( const std::filesystem::path&, void* )> assetFactory() {
        static_assert( std::is_constructible_v<T, const std::filesystem::path&>,
                       "Types that cannot be constructed from a std::filesystem::path"
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
            storage().emplace( std::make_pair( typeHash, detail::TypeErasedContainer::make<T>()));

        return detail::add( path, tag, typeHash, std::move( assetFactory<T>()));
    }
    catch ( AssetManagerException& e ) {
        e << Because( util::StringBuilder( 256 ) << "Failed to add asset of type "
                                                 << boost::core::demangle( typeid( T ).name())
                                                 << " from \"" << path << "\" tagged \"" << tag << '"' );
        throw;
    }

    template< typename T >
    const T& get( std::string_view tag ) try {
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
