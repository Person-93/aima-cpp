#include "AssetManager.hpp"
#include <algorithm>
#include <string>
#include <memory>
#include "sqlite_modern_cpp.h"
#include "util/StringBuilder.hpp"
#include "util/Finally.hpp"
#include "util/type_traits.hpp"


using namespace std::string_view_literals;
using namespace aima::util::AssetManager;
using std::string_view;
using sqlite::database;
using detail::GetLogger;
using detail::TypeErasedContainer;
using detail::storage;
using ::aima::util::StringBuilder;

log4cplus::Logger& detail::GetLogger() {
    static auto logger = log4cplus::Logger::getInstance( "AssetManager" );
    return logger;
}

AssetHandle::~AssetHandle() noexcept( false ) {
    auto eCount = std::uncaught_exceptions();
    try {
        detail::freeAssetHandle( *this );
    }
    catch ( ... ) {
        if ( eCount == 0 ) throw;
    }
}

AssetHandle::AssetHandle( const AssetHandle& other ) : id_( other.id_ ) {
    detail::copyAssetHandle( *this );
}

AssetHandle& AssetHandle::operator=( const AssetHandle& other ) {
    detail::freeAssetHandle( *this );
    id_ = other.id_;
    detail::copyAssetHandle( *this );
    return *this;
}

namespace {
    using SqlErrorCode   = boost::error_info<struct sql_error_code, int>;
    using SqlCode        = boost::error_info<struct sql_code, std::string>;
    using SqlTransaction = boost::error_info<struct sql_transaction, std::string>;

    class SqliteException : public virtual aima::util::AssetManager::AssetManagerException {
    public:
        explicit SqliteException( const sqlite::sqlite_exception& e ) {
            *this << aima::core::exception::WrappedExceptionType( boost::core::demangle( typeid( e ).name()));
            *this << aima::core::exception::WrappedExceptionMessage( e.what());
            *this << SqlErrorCode( e.get_extended_code());
            *this << SqlCode( e.get_sql());
        }
    };

    std::filesystem::path assetDir = ""; // NOLINT(cert-err58-cpp)

    database db() {
        static database db_ = []() {
            try {
                database db__( ":memory:" );

                ( db__ << R"(
CREATE TABLE assets (
    id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    idx INTEGER DEFAULT 0,
    type_hash INTEGER NOT NULL,
    path TEXT NOT NULL UNIQUE,
    quantity INTEGER DEFAULT 1
)
)" ).execute();

                ( db__ << R"(
CREATE TABLE tags (
    asset_id INTEGER NOT NULL,
    tag TEXT NOT NULL
)
)" ).execute();
                return db__;
            }
            catch ( const sqlite::sqlite_exception& e ) {
                AIMA_THROW_EXCEPTION( SqliteException(e) << SqlTransaction("Create database"));
            }
        }();
        return db_;
    }

    class Transaction {
    public:
        Transaction() {
            db() << "BEGIN";
        }

        void commit() {
            if ( completed ) {
                using namespace aima::core::exception;
                AIMA_THROW_EXCEPTION( Exception{} << Because( "Cannot commit transaction that is already complete" ));
            }
            completed = true;
            db() << "COMMIT";
        }

        void rollback() {
            if ( completed ) {
                using namespace aima::core::exception;
                AIMA_THROW_EXCEPTION( Exception{} << Because( "Cannot rollback transaction that is already complete" ));
            }
            completed = true;
            db() << "ROLLBACK;";
        }

        ~Transaction() {
            if ( !completed ) rollback();
        }

    private:
        bool completed = false;
    };

    struct AssetInfo {
        /**  ID of the asset, defaults to 0 */
        size_t id       = 0;
        /** Index of the asset in the asset storage, defaults to std::numeric_limits<>::max */
        size_t index    = std::numeric_limits<size_t>::max();
        /** Number of asset handles that point to this asset */
        size_t quantity = 0;
        /** Hash of asset type, defaults to 0 */
        size_t typeHash = 0;
    };

    sqlite::database_binder& operator<<( sqlite::database_binder& binder, std::string_view stringView ) {
        return binder << std::string( stringView );
    }

    sqlite::database_binder& operator<<( sqlite::database_binder& binder, const char* cString ) {
        return binder << std::string( cString );
    }

    struct execute_t {};

    execute_t execute;

    class Statement {
    public:
        Statement( std::string_view query ) try : // NOLINT(google-explicit-constructor)
                Statement( db() << query.data()) {}
        catch ( const sqlite::sqlite_exception& e ) {
            AIMA_THROW_EXCEPTION( SqliteException(e) << SqlTransaction("Preparing statement"));
        }

        Statement( sqlite::database_binder&& binder ) : // NOLINT(google-explicit-constructor)
                binder( std::move( binder )) {
        }

        ~Statement() {
            binder.used( true );
        }

        class Instance {
        public:
            explicit Instance( Statement& statement ) : binder( statement.binder ) {}

            Instance( const Instance& ) = delete;

            Instance( Instance&& other ) = delete;

            Instance& operator=( const Instance& ) = delete;

            Instance& operator=( Instance&& ) = delete;

            ~Instance() {
                binder.reset();
            }

            template< typename T >
            friend Instance& operator<<( Statement::Instance& instance, T&& t ) {
                instance.binder << std::forward<T>( t );
                return instance;
            }

            template< typename T >
            friend Instance& operator<<( Statement::Instance&& instance, T&& t ) {
                return instance << std::forward<T>( t );
            }

            template< typename T >
            friend Instance& operator>>( Statement::Instance& instance, T&& t ) {
                instance.binder >> std::forward<T>( t );
                return instance;
            }

            template< typename T >
            friend Instance& operator>>( Statement::Instance&& instance, T&& t ) {
                return instance << std::forward<T>( t );
            }

            void operator<<( execute_t ) {
                execute();
            }

            void execute() {
                binder.execute();
            }

        private:
            sqlite::database_binder& binder;
        };

        Instance instance() {
            return Instance( *this );
        }

    private:
        sqlite::database_binder binder;
    };

    template< typename T, typename ... Args >
    AssetInfo getAsset( Statement::Instance& instance, T&& t, Args&& ... args ) {
        instance << std::forward<T>( t );
        if constexpr ( sizeof...( Args ) > 0 ) {
            return getAsset( instance, args... );
        }
        else {
            try {
                AssetInfo info{};
                instance >> [ &info ]( size_t id, size_t index, size_t quantity, size_t typeHash ) {
                    info = {
                            .id       = id,
                            .index    = index,
                            .quantity = quantity,
                            .typeHash = typeHash,
                    };
                };
                return info;
            }
            catch ( sqlite::sqlite_exception& e ) {
                AIMA_THROW_EXCEPTION( SqliteException(e));
            }
        }
    }

    template< typename ... Args >
    AssetInfo getAsset( Statement& statement, Args&& ... args ) {
        auto instance = statement.instance();
        return getAsset( instance, std::forward<Args>( args )... );
    }

    AssetInfo getAsset( const AssetHandle& handle ) try {
        static Statement statement = "SELECT id, idx, quantity, type_hash FROM assets WHERE id = ?"sv;
        return getAsset( statement, handle.id());
    }
    catch ( SqliteException& e ) {
        e << SqlTransaction( "Get asset by handle" );
        e << SqlTransaction( "Get asset by handle" );
        throw;
    }

    AssetInfo getAsset( const std::filesystem::path& path ) try {
        static Statement statement = "SELECT id, idx, quantity, type_hash FROM assets WHERE path = ?"sv;
        return getAsset( statement, path );
    }
    catch ( SqliteException& e ) {
        e << SqlTransaction( "Get asset by path" );
        throw;
    }

    AssetInfo getAsset( std::string_view tag, std::type_index type ) try {
        static Statement statement = db() << R"(
SELECT a.id, a.idx, a.quantity, a.type_hash
FROM tags t
INNER JOIN assets a
  ON a.id = t.asset_id
WHERE a.type_hash = ?
  AND t.tag = ?
)";
        return getAsset( statement, type.hash_code(), tag );
    }
    catch ( SqliteException& e ) {
        e << SqlTransaction( "Get asset by tag" );
        throw;
    }

    /**
     * Inserts a new asset record into the databse
     * @param path
     * @param typeHash
     * @param index
     * @return The ID of the newly inserted asset
     */
    size_t insertAsset( const std::filesystem::path& path, size_t typeHash, size_t index ) try {
        static Statement statement = db() << "INSERT INTO assets( path, type_hash, idx ) VALUES( ?, ?, ? )";
        statement.instance() << path.c_str() << typeHash << index << execute;
        return static_cast<size_t>(db().last_insert_rowid());
    }
    catch ( const sqlite::sqlite_exception& e ) {
        AIMA_THROW_EXCEPTION( SqliteException(e) << SqlTransaction("Insert new asset"));
    }

    /**
     * If no record of this asset exists, one is created. Otherwise, it increments the
     * count of asset handles that point to this asset
     * @param id The id of the asset, if it is zero, it will be set to the id of the newly created asset
     * @param path
     */
    void incrementHandleCount( size_t id ) try {
        static Statement statement = db() << "UPDATE assets SET quantity = quantity + 1 WHERE id = ?";
        statement.instance() << id << execute;
    }
    catch ( const sqlite::sqlite_exception& e ) {
        AIMA_THROW_EXCEPTION( SqliteException(e) << SqlTransaction("Increment Handle Count"));
    }

    /**
     * Decrements the handle count of the given asset
     * @param id
     * @return The count before decrementing
     */
    void decrementHandleCount( size_t id ) try {
        static Statement decrement = db() << "UPDATE assets SET quantity = quantity - 1 WHERE id = ?";
        decrement.instance() << id << execute;
    }
    catch ( const sqlite::sqlite_exception& e ) {
        AIMA_THROW_EXCEPTION( SqliteException(e) << SqlTransaction("Decrement Handle Count"));
    }

    void insertTag( size_t id, std::string_view tag ) try {
        static Statement insertTag = db() << "INSERT INTO tags( asset_id, tag ) VALUES(?,?)";
        std::move( insertTag.instance() << id << tag ) << execute;
    }
    catch ( const sqlite::sqlite_exception& e ) {
        AIMA_THROW_EXCEPTION( SqliteException(e) << SqlTransaction("Insert tag"));
    }

}

namespace {
    bool isDirectory( const std::filesystem::path& path ) {
        using namespace std::filesystem;
        if ( is_symlink( path )) return isDirectory( read_symlink( path ));
        return is_directory( path );
    }
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

AssetHandle detail::add( std::filesystem::path path,
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

AssetHandle::AssetHandle( AssetHandle&& other ) noexcept {
    id_ = other.id_;
    other.id_ = 0;
}

AssetHandle& AssetHandle::operator=( AssetHandle&& other ) noexcept {
    using std::swap;
    swap( id_, other.id_ );
    return *this;
}

void* detail::get( std::string_view tag, std::type_index type ) {
    AssetInfo info = getAsset( tag, type );

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

void detail::freeAssetHandle( const AssetHandle& handle ) {
    TRACE;

    if ( handle.id() == 0 ) return;

    Transaction t;

    AssetInfo info = getAsset( handle );
    if ( info.id == 0 ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION(
                AssetManagerException{}
                        << Because( StringBuilder( 64 ) << "Tried to release handle to non-existent asset: "
                                                        << handle.id()));
    }

    if ( info.quantity == 0 ) {
        using namespace aima::core::exception;
        using aima::util::StringBuilder;
        AIMA_THROW_EXCEPTION( AssetManagerException{}
                                      << Because( StringBuilder( 64 ) << "Tried to free already freed asset, id: "
                                                                      << handle.id()));

    }

    decrementHandleCount( info.id );
    t.commit();

    if ( info.quantity == 1 )
        storage().at( info.typeHash ).destroy( info.index );
}

detail::Storage& ::aima::util::AssetManager::detail::storage() {
    static Storage storage;
    return storage;
}

void ::aima::util::AssetManager::detail::copyAssetHandle( const AssetHandle& handle ) {
    incrementHandleCount( handle.id());
}
