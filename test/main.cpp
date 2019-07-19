#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#define BOOST_TEST_MODULE Tests
#define BOOST_TEST_ALTERNATIVE_INIT_API
#define BOOST_TEST_NO_MAIN
#pragma clang diagnostic pop

#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test_monitor.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/test/unit_test_suite.hpp>
#include "log4cplus/logger.h"
#include "log4cplus/loglevel.h"

int main( int argc, char* argv[] ) {
    log4cplus::Logger::getRoot().setLogLevel( log4cplus::OFF_LOG_LEVEL );
    boost::unit_test::unit_test_monitor.register_exception_translator<boost::exception>(
            []( const boost::exception& e ) {
                BOOST_TEST_FAIL( boost::diagnostic_information( e, true ));
            } );
    return boost::unit_test::unit_test_main( &init_unit_test, argc, argv );
}
