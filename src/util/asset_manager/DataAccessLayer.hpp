#pragma once

#include "AssetManagerException.hpp"
#include <filesystem>
#include <typeindex>
#include <string_view>

namespace aima::util::AssetManager {
    class AssetHandle;
}

namespace sqlite {
    class sqlite_exception;
}

namespace aima::util::AssetManager {
    namespace DataAccessLayer {
        using SqlErrorCode   = boost::error_info<struct sql_error_code, int>;
        using SqlCode        = boost::error_info<struct sql_code, std::string>;
        using SqlTransaction = boost::error_info<struct sql_transaction, std::string>;

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

        class Transaction {
        public:
            Transaction();

            void commit();

            void rollback();

            ~Transaction();

        private:
            bool completed = false;
        };

        class SqliteException : public virtual aima::util::AssetManager::AssetManagerException {
        public:
            explicit SqliteException( const sqlite::sqlite_exception& e );
        };

        AssetInfo getAsset( const AssetHandle& handle );

        AssetInfo getAsset( const std::filesystem::path& path );

        AssetInfo getAsset( std::string_view tag, std::type_index type );

        size_t insertAsset( const std::filesystem::path& path, size_t typeHash, size_t index );

        void incrementHandleCount( size_t id );

        void decrementHandleCount( size_t id );

        void insertTag( size_t id, std::string_view tag );

        void freeAssetHandle( const AssetHandle& handle );

        void copyAssetHandle( const AssetHandle& handle );
    }
}
