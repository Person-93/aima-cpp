#include "TypeErasedContainer.hpp"

using aima::util::AssetManager::detail::TypeErasedContainer;

DEFINE_LOGGER( TypeErasedContainer );

log4cplus::Logger& aima::util::AssetManager::detail::type_erased_container::GetLogger() {
    return ::GetLogger();
}

TypeErasedContainer::~TypeErasedContainer() {
    TRACE;
    containerDeleter( pContainer );
}

size_t TypeErasedContainer::insert() {
    return inserter( pContainer );
}

void* TypeErasedContainer::at( size_t index ) {
    return indexer( index, pContainer );
}

void TypeErasedContainer::destroy( size_t index ) noexcept {
    destructor( at( index ));
}

TypeErasedContainer::TypeErasedContainer( void* pContainer,
                                          Inserter inserter,
                                          Popper popper,
                                          Indexer indexer,
                                          Destructor destructor,
                                          Destructor containerDestructor ) :
        pContainer( pContainer ),
        popper( popper ),
        inserter( inserter ),
        indexer( indexer ),
        destructor( destructor ),
        containerDeleter( containerDestructor ) {}

TypeErasedContainer::TypeErasedContainer( TypeErasedContainer&& other ) noexcept :
        pContainer( other.pContainer ),
        popper( other.popper ),
        inserter( other.inserter ),
        indexer( other.indexer ),
        destructor( other.destructor ),
        containerDeleter( other.containerDeleter ) {
    other.pContainer = nullptr;
}

TypeErasedContainer& TypeErasedContainer::operator=( TypeErasedContainer&& other ) noexcept {
    containerDeleter( pContainer );
    pContainer       = other.pContainer;
    popper           = other.popper;
    inserter         = other.inserter;
    indexer          = other.indexer;
    destructor       = other.destructor;
    containerDeleter = other.containerDeleter;
    other.pContainer = nullptr;
    return *this;
}

void TypeErasedContainer::pop_back() {
    popper( pContainer );
}
