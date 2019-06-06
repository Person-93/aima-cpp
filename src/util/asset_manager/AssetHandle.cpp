#include "AssetHandle.hpp"
#include "DataAccessLayer.hpp"
#include "util/AssetManager.hpp"
#include "util/Finally.hpp"

using namespace aima::util::AssetManager;

inline log4cplus::Logger& GetLogger() {
    return detail::GetLogger();
}

AssetHandle::~AssetHandle() noexcept( false ) {
    auto eCount = std::uncaught_exceptions();
    try {
        DataAccessLayer::freeAssetHandle( *this );
    }
    catch ( ... ) {
        if ( eCount == 0 ) throw;
    }
}

AssetHandle::AssetHandle( const AssetHandle& other ) : id_( other.id_ ) {
    DataAccessLayer::copyAssetHandle( *this );
}

AssetHandle::AssetHandle( AssetHandle&& other ) noexcept {
    id_ = other.id_;
    other.id_ = 0;
}

AssetHandle& AssetHandle::operator=( AssetHandle&& other ) noexcept {
    using std::swap;
    swap( id_, other.id_ );
    return *this;
}

AssetHandle& AssetHandle::operator=( const AssetHandle& other ) {
    DataAccessLayer::freeAssetHandle( *this );
    id_ = other.id_;
    DataAccessLayer::copyAssetHandle( *this );
    return *this;
}

namespace {
    std::filesystem::path assetDir = ""; // NOLINT(cert-err58-cpp)

    /**
     * Checks if a filepath is a directory or a symlink to a directory
     * @param path
     * @return
     */
    bool isDirectory( const std::filesystem::path& path ) {
        using namespace std::filesystem;
        if ( is_symlink( path )) return isDirectory( read_symlink( path ));
        return is_directory( path );
    }
}

AssetHandle AssetHandle::make( std::filesystem::path path,
                               std::string_view tag,
                               size_t typeHash,
                               std::function<void( std::filesystem::path, void* )> factory ) {
    if ( path.is_relative()) {
        if ( assetDir == "" ) {
            using namespace aima::core::exception;
            AIMA_THROW_EXCEPTION(
                    Exception{} << Because( "Asset path must be absolute if setAssetDir() was not called" ));
        }

        path = assetDir / path;
    }

    using namespace DataAccessLayer;
    using detail::storage;

    Transaction t;

    AssetInfo info = getAsset( path );
    TypeErasedContainer* container = nullptr;

    // the container guard frees the allocated memory in case of exception
    aima::util::Finally containerGuard( [ &container ]() noexcept {
        try {
            if ( container ) container->pop_back();
        }
        catch ( ... ) {}
    } );

    if ( info.id == 0 ) {
        container = &storage().at( typeHash );
        info.index = container->insert();
        info.id    = insertAsset( path, typeHash, info.index );
    }
    else {
        incrementHandleCount( info.id );
    }

    insertTag( info.id, tag );

    if ( info.quantity == 0 ) { // means that this is the first asset handle for this asset
        if ( !container ) container = &storage().at( typeHash );
        factory( path, container->at( info.index ));
    }

    t.commit();
    containerGuard.dismiss();
    return AssetHandle( info.id );
}


void ::aima::util::AssetManager::setAssetDir( std::filesystem::path path ) {
    if ( !std::filesystem::exists( path )) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because(
                util::StringBuilder( 256 ) << "Asset directory " << path << " does not exist" ));
    }

    if ( !isDirectory( path )) {
        using namespace aima::core::exception;
        using aima::util::StringBuilder;
        AIMA_THROW_EXCEPTION(
                Exception{} << Because( StringBuilder( 256 ) << "Asset directory must be a directory, got:" << path ));

    }

    assetDir = std::move( path );
}
