#pragma once

#include <boost/container/stable_vector.hpp>
#include "util/define_logger.hpp"
#include "util/StringBuilder.hpp"
#include "util/asset_manager/AssetManagerException.hpp"

namespace aima::util::AssetManager {
    namespace type_erased_container {
        log4cplus::Logger& GetLogger();
    }
    class TypeErasedContainer {
    public:
        TypeErasedContainer( const TypeErasedContainer& ) = delete;

        TypeErasedContainer( TypeErasedContainer&& other ) noexcept;

        TypeErasedContainer& operator=( const TypeErasedContainer& ) = delete;

        TypeErasedContainer& operator=( TypeErasedContainer&& other ) noexcept;

        template< typename T >
        static TypeErasedContainer make() {
            using type_erased_container::GetLogger;
            using Vector = boost::container::stable_vector<std::aligned_storage_t<sizeof( T )>>;
            TRACE;
            LOG4CPLUS_DEBUG( GetLogger(),
                             "Constructing new TypeErasedContainer for type: "
                                     << boost::core::demangle( typeid( T ).name()));

            return TypeErasedContainer(
                    // container
                    new Vector(),

                    // inserter
                    []( void* p ) -> size_t {
                        auto pVector = static_cast<Vector*>(p);
                        pVector->emplace_back();
                        return pVector->size() - 1;
                    },

                    // popper
                    []( void* p ) -> void {
                        auto pVector = static_cast<Vector*>(p);
                        pVector->pop_back();
                    },

                    // indexer
                    []( size_t i, void* p ) -> void* {
                        auto        pVector     = static_cast<Vector*>(p);
                        if ( size_t desiredSize = i + 1; pVector->size() < desiredSize ) {
                            AIMA_THROW_EXCEPTION(
                                    AssetManagerException{}
                                            << TypeErasedAction(
                                                    util::StringBuilder( 256 )
                                                            << "In asset of type "
                                                            << boost::core::demangle( typeid( T ).name())
                                                            << ": requested asset at index " << i
                                                            << "size is " << pVector->size()));
                        }
                        return &(( *pVector )[ i ] );
                    },

                    // destructor
                    []( const void* p ) noexcept -> void {
                        static_cast<const T*>(p)->~T();
                    },

                    // container destructor
                    []( const void* p ) noexcept -> void {
                        auto pVector = static_cast<const Vector*>(p);
                        delete pVector;
                    } );
        }

        ~TypeErasedContainer();

        size_t insert();

        void* at( size_t index );

        void destroy( size_t index ) noexcept;

        void pop_back();

    private:
        using Inserter   = size_t( * )( void* );
        using Popper     = void ( * )( void* );
        using Indexer    = void* ( * )( size_t, void* );
        using Destructor = void ( * )( const void* ) noexcept;

        TypeErasedContainer( void* pContainer,
                             Inserter inserter,
                             Popper popper,
                             Indexer indexer,
                             Destructor destructor,
                             Destructor containerDestructor );


        void* pContainer;
        Inserter   inserter;
        Popper     popper;
        Indexer    indexer;
        Destructor destructor;
        Destructor containerDeleter;
    };
}
