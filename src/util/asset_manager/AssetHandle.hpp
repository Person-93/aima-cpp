#pragma once

#include <cstddef>
#include <filesystem>
#include <string_view>
#include <functional>

namespace aima::util::AssetManager {
    class AssetHandle {
    public:
        AssetHandle() : id_( 0 ) {}

        AssetHandle( const AssetHandle& other );

        AssetHandle( AssetHandle&& other ) noexcept;

        AssetHandle& operator=( const AssetHandle& other );

        AssetHandle& operator=( AssetHandle&& other ) noexcept;

        ~AssetHandle() noexcept( false );

        size_t id() const noexcept { return id_; }

        static AssetHandle make( std::filesystem::path path,
                                 std::string_view tag,
                                 size_t typeHash,
                                 std::function<void( std::filesystem::path, void* )> factory );

    private:
        explicit AssetHandle( size_t id ) : id_( id ) {}

        size_t id_;
    };
}
