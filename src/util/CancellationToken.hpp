#pragma once

#include <atomic>
#include <memory>


namespace aima::util {
    /**
     * A token to be used for cooperative cancellation.
     *
     * The thread that will issue the cancellation should hold a <code>std::shared_ptr&lt;CancellationToken&gt;</code>,
     * the thread that will receive the cancellation should hold a
     * <code>std::shared_ptr&lt;const CancellationToken&gt;</code>.
     */
    class CancellationToken {
    public:
        /**
         * Create a cancellation token
         * @param cancelled
         * @return A shared_ptr to a cancellation token
         */
        static std::shared_ptr<CancellationToken> create( bool cancelled = false ) {
            struct CancellationTokenHelper : public CancellationToken {
                explicit CancellationTokenHelper( bool cancelled ) : CancellationToken( cancelled ) {}
            };

            return std::make_shared<CancellationTokenHelper>( cancelled );
        }

        CancellationToken( CancellationToken& ) = delete;

        CancellationToken( CancellationToken&& ) = delete;

        CancellationToken& operator=( const CancellationToken& ) = delete;

        CancellationToken& operator=( CancellationToken&& ) = delete;

        /**
         * Issue a cancellation request
         */
        void cancel() noexcept { cancelled = true; }

        /**
         * Check if a cancellation request was issued
         * @return <code>true</code> if a cancellation request was issued,
         *         <code>false</code> otherwise.
         */
        bool isCancelled() const noexcept { return cancelled; }

    private:
        explicit constexpr CancellationToken( bool cancelled ) : cancelled( cancelled ) {}

        std::atomic_bool cancelled;
    };
}
