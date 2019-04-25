#include <utility>
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <chrono>
#include "util/AssetManager.hpp"

struct TestAsset {
    explicit TestAsset( std::filesystem::path path ) : path( std::move( path )) {
        ++constructorCounter;
    }

    TestAsset( const TestAsset& ) = delete;

    TestAsset( TestAsset&& ) = delete;

    TestAsset& operator=( const TestAsset& ) = delete;

    TestAsset& operator=( TestAsset&& ) = delete;

    ~TestAsset() {
        ++destructorCounter;
    }

    static size_t constructorCounter;
    static size_t destructorCounter;

    std::filesystem::path path;
};

size_t TestAsset::constructorCounter = 0;
size_t TestAsset::destructorCounter  = 0;

BOOST_AUTO_TEST_CASE( single_asset ) { // NOLINT(cert-err58-cpp)
    const std::filesystem::path path = "/this/is/the/path";
    const std::string_view      tag  = "A tag";
    const std::string_view      tag1 = "Another tag";

    TestAsset::constructorCounter = 0;
    TestAsset::destructorCounter  = 0;
    namespace AssetManager = aima::util::AssetManager;
    {
        auto handle = AssetManager::add<TestAsset>( path, tag );
        BOOST_REQUIRE_EQUAL( TestAsset::constructorCounter, 1 );
        BOOST_REQUIRE_EQUAL( TestAsset::destructorCounter, 0 );

        auto& asset = AssetManager::get<TestAsset>( tag );
        BOOST_REQUIRE_EQUAL( asset.path, path );

        {
            auto handle1 = AssetManager::add<TestAsset>( path, tag );
            BOOST_REQUIRE_EQUAL( TestAsset::constructorCounter, 1 );
        }
        BOOST_REQUIRE_EQUAL( TestAsset::destructorCounter, 0 );

        auto handle2 = AssetManager::add<TestAsset>( path, tag1 );
        BOOST_REQUIRE_EQUAL( TestAsset::constructorCounter, 1 );
        BOOST_REQUIRE_EQUAL( TestAsset::destructorCounter, 0 );

        auto& asset1 = AssetManager::get<TestAsset>( tag1 );
        BOOST_REQUIRE_EQUAL( &asset, &asset1 );
    }
    BOOST_REQUIRE_EQUAL( TestAsset::constructorCounter, 1 );
    BOOST_REQUIRE_EQUAL( TestAsset::destructorCounter, 1 );
}
