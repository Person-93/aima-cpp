#include <sstream>
#include <string>
#include "Demo.hpp"
#include "core/Environment.hpp"
#include "core/Agent.hpp"
#include "util/CancellationToken.hpp"
#include "util/define_logger.hpp"

// IWYU pragma: no_include <type_traits>

using namespace aima::core;

DEFINE_LOGGER( Demo );

Demo::Demo( std::ostream& out ) : viewer( out ) { TRACE; }

void Demo::run( std::shared_ptr<const util::CancellationToken> cancellationToken ) {
    TRACE;

    auto& env = environment();
    env.addEnvironmentView( viewer );
    init();

    if ( cancellationToken )
        while ( !env.isDone() && !cancellationToken->isCancelled())
            env.step( 10 );
    else env.stepUntilDone();

    std::stringstream ss;
    ss << "Performance Measures";
    for ( const auto&[agentRef, performance] : env.getPerformanceMeasures()) {
        ss << "\t(Agent " << agentRef.get().id() << ": " << performance.dirtyRead() << ')';
    }
    viewer.notify( ss.str());

    runAtEnd();
}
