#include <utility>
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <chrono>
#include "util/AssetManager.hpp"

struct AssetManagerTestFixture {
    AssetManagerTestFixture() {
        std::ostringstream ss;
        ss << "db-" << std::chrono::steady_clock::now().time_since_epoch().count() << ".sqlite3";
        aima::util::AssetManager::initialize( ss.str());
        BOOST_TEST_MESSAGE( "Using database file: " << ss.str());
    }
};

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

BOOST_FIXTURE_TEST_CASE( single_asset, AssetManagerTestFixture ) { // NOLINT(cert-err58-cpp)
    const std::filesystem::path path = "/this/is/the/path";
    const std::string_view      tag  = "A tag";
    const std::string_view      tag1 = "Another tag";

    TestAsset::constructorCounter = 0;
    TestAsset::destructorCounter  = 0;
    namespace AssetManager = aima::util::AssetManager;
    AssetManager::initialize();
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
