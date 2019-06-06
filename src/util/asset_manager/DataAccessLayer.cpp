#include "DataAccessLayer.hpp"
#include "AssetHandle.hpp"
#include "sqlite_modern_cpp.h"
#include "util/AssetManager.hpp"
#include "util/define_logger.hpp"
#include "util/StringBuilder.hpp"

namespace {
    log4cplus::Logger& GetLogger() {
        return aima::util::AssetManager::detail::GetLogger();
    }

    using namespace std::string_view_literals;

    using sqlite::database;
    using aima::util::AssetManager::DataAccessLayer::SqliteException;
    using aima::util::AssetManager::DataAccessLayer::SqlTransaction;


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
                AIMA_THROW_EXCEPTION( SqliteException( e ) << SqlTransaction( "Create database" ));
            }
        }();
        return db_;
    }

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
            AIMA_THROW_EXCEPTION( SqliteException( e ) << SqlTransaction( "Preparing statement" ));
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
                return instance >> std::forward<T>( t );
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
}

namespace aima::util::AssetManager::DataAccessLayer {
    template< typename T, typename ... Args >
    AssetInfo getAsset( Statement::Instance& instance, T&& t, Args&& ... args ) {
        instance << std::forward<T>( t );
        if constexpr ( sizeof...( Args ) > 0 ) {
            return getAsset( instance, args... );
        }
        else {
            try {
                AssetInfo info;
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
                AIMA_THROW_EXCEPTION( SqliteException( e ));
            }
        }
    }

    template< typename ... Args >
    AssetInfo getAsset( Statement& statement, Args&& ... args ) {
        auto instance = statement.instance();
        return getAsset( instance, std::forward<Args>( args )... );
    }

    AssetInfo getAsset( const aima::util::AssetManager::AssetHandle& handle ) try {
        static Statement statement = "SELECT id, idx, quantity, type_hash FROM assets WHERE id = ?"sv;
        return getAsset( statement, handle.id());
    }
    catch ( SqliteException& e ) {
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
        static Statement statement = R"(
SELECT a.id, a.idx, a.quantity, a.type_hash
FROM tags t
INNER JOIN assets a
  ON a.id = t.asset_id
WHERE a.type_hash = ?
  AND t.tag = ?
)"sv;
        return getAsset( statement, type.hash_code(), tag );
    }
    catch ( SqliteException& e ) {
        e << SqlTransaction( "Get asset by tag" );
        throw;
    }

    /**
     * Inserts a new asset record into the database
     * @param path
     * @param typeHash
     * @param index
     * @return The ID of the newly inserted asset
     */
    size_t insertAsset( const std::filesystem::path& path, size_t typeHash, size_t index ) try {
        static Statement statement = "INSERT INTO assets( path, type_hash, idx ) VALUES( ?, ?, ? )"sv;
        statement.instance() << path << typeHash << index << execute;
        return static_cast<size_t>(db().last_insert_rowid());
    }
    catch ( const sqlite::sqlite_exception& e ) {
        AIMA_THROW_EXCEPTION( SqliteException( e ) << SqlTransaction( "Insert new asset" ));
    }

    /**
     * If no record of this asset exists, one is created. Otherwise, it increments the
     * count of asset handles that point to this asset
     * @param id The id of the asset, if it is zero, it will be set to the id of the newly created asset
     * @param path
     */
    void incrementHandleCount( size_t id ) try {
        static Statement statement = "UPDATE assets SET quantity = quantity + 1 WHERE id = ?"sv;
        statement.instance() << id << execute;
    }
    catch ( const sqlite::sqlite_exception& e ) {
        AIMA_THROW_EXCEPTION( SqliteException( e ) << SqlTransaction( "Increment Handle Count" ));
    }

    /**
     * Decrements the handle count of the given asset
     * @param id
     * @return The count before decrementing
     */
    void decrementHandleCount( size_t id ) try {
        static Statement decrement = "UPDATE assets SET quantity = quantity - 1 WHERE id = ?"sv;
        decrement.instance() << id << execute;
    }
    catch ( const sqlite::sqlite_exception& e ) {
        AIMA_THROW_EXCEPTION( SqliteException( e ) << SqlTransaction( "Decrement Handle Count" ));
    }

    void insertTag( size_t id, std::string_view tag ) try {
        static Statement insertTag = "INSERT INTO tags( asset_id, tag ) VALUES(?,?)"sv;
        std::move( insertTag.instance() << id << tag ) << execute;
    }
    catch ( const sqlite::sqlite_exception& e ) {
        AIMA_THROW_EXCEPTION( SqliteException( e ) << SqlTransaction( "Insert tag" ));
    }
}

using namespace aima::util::AssetManager::DataAccessLayer;

SqliteException::SqliteException( const sqlite::sqlite_exception& e ) {
    using namespace aima::core::exception;

    *this << WrappedExceptionType( boost::core::demangle( typeid( e ).name()));
    *this << WrappedExceptionMessage( e.what());
    *this << SqlErrorCode( e.get_extended_code());
    *this << SqlCode( e.get_sql());
}

Transaction::Transaction() {
    db() << "BEGIN";
}

void Transaction::commit() {
    if ( completed ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because( "Cannot commit transaction that is already complete" ));
    }
    completed = true;
    db() << "COMMIT";
}

void Transaction::rollback() {
    if ( completed ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because( "Cannot rollback transaction that is already complete" ));
    }
    completed = true;
    db() << "ROLLBACK;";
}

Transaction::~Transaction() {
    if ( !completed ) rollback();
}


void ::aima::util::AssetManager::DataAccessLayer::freeAssetHandle( const AssetHandle& handle ) {
    TRACE;

    if ( handle.id() == 0 ) return;

    Transaction t;

    auto info = getAsset( handle );
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
        detail::storage().at( info.typeHash ).destroy( info.index );
}

void ::aima::util::AssetManager::DataAccessLayer::copyAssetHandle( const AssetHandle& handle ) {
    incrementHandleCount( handle.id());
}
