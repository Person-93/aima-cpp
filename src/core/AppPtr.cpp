#include "AppPtr.hpp"
#include "Environment.hpp"
#include "App.hpp" // IWYU pragma: keep

using namespace aima::IntegratedRunner;
using detail::ControlBlock;

void ControlBlock::unPoint() noexcept {
    --ptrCount;
    tryErase();
}

void ControlBlock::removeWeak() noexcept {
    --weakPtrCount;
    tryErase();
}

void ControlBlock::notify( ) noexcept {
    heldByEnvironment = false;
    tryErase();
}

void ControlBlock::tryErase() noexcept {
    if ( ptrCount ) return;
    std::unique_lock lock( mutex, std::try_to_lock );
    if ( ptrCount ) return;


    if ( internalPtr ) {
        // need to release the environment before the app can be disposed of
        // can't dispose of the app right away because the environment might be busy
        if ( heldByEnvironment ) {
            lock.unlock();
            auto& env = internalPtr->environment();
            env->stop();
            env = nullptr;
            return;
        }

        delete internalPtr;
        internalPtr = nullptr;
    }

    if ( !weakPtrCount ) delete this;
}

detail::ControlBlock::ControlBlock( ControlBlock::App* ptr ) : internalPtr( ptr ) {
    ptr->environment()->addEnvironmentDestroyedView( *this );
}

void detail::ControlBlock::point() noexcept {
    std::scoped_lock lock( mutex );
    ++ptrCount;
}

void detail::ControlBlock::addWeak() noexcept {
    std::scoped_lock lock( mutex );
    ++weakPtrCount;
}

WeakAppPtr::WeakAppPtr( const AppPtr& ptr ) noexcept : pControlBlock( ptr.pControlBlock ) {
    if ( pControlBlock ) pControlBlock->addWeak();
}

AppPtr WeakAppPtr::lock() noexcept { return AppPtr( *this ); }

const AppPtr WeakAppPtr::lock() const noexcept { return AppPtr( *this ); }

WeakAppPtr& WeakAppPtr::operator=( const AppPtr& other ) noexcept {
    if ( pControlBlock == other.pControlBlock ) return *this;

    if ( pControlBlock ) pControlBlock->removeWeak();
    pControlBlock = other.pControlBlock;
    if ( pControlBlock ) pControlBlock->addWeak();
    return *this;
}

WeakAppPtr& WeakAppPtr::operator=( AppPtr&& other ) noexcept {
    if ( pControlBlock == other.pControlBlock ) return *this;

    if ( pControlBlock ) pControlBlock->removeWeak();
    pControlBlock = other.pControlBlock;
    if ( pControlBlock ) pControlBlock->addWeak();
    other.reset();
    return *this;
}

WeakAppPtr& WeakAppPtr::operator=( const WeakAppPtr& other ) noexcept {
    if ( pControlBlock == other.pControlBlock ) return *this;

    if ( pControlBlock ) pControlBlock->removeWeak();
    pControlBlock = other.pControlBlock;
    if ( pControlBlock ) pControlBlock->addWeak();
    return *this;
}

WeakAppPtr& WeakAppPtr::operator=( WeakAppPtr&& other ) noexcept {
    if ( pControlBlock == other.pControlBlock ) return *this;

    if ( pControlBlock ) pControlBlock->removeWeak();
    pControlBlock = other.pControlBlock;
    other.pControlBlock = nullptr;
    return *this;
}

WeakAppPtr& WeakAppPtr::operator=( std::nullptr_t ) noexcept {
    reset();
    return *this;
}

WeakAppPtr::~WeakAppPtr() {
    if ( pControlBlock ) pControlBlock->removeWeak();
}

WeakAppPtr::operator bool() {
    return pControlBlock && pControlBlock->isAlive();
}

void WeakAppPtr::reset() noexcept {
    if ( !pControlBlock ) return;
    pControlBlock->removeWeak();
    pControlBlock = nullptr;
}

WeakAppPtr::WeakAppPtr( const WeakAppPtr& other ) noexcept : pControlBlock( other.pControlBlock ) {
    if ( pControlBlock ) pControlBlock->addWeak();
}

WeakAppPtr::WeakAppPtr( WeakAppPtr&& other ) noexcept : pControlBlock( other.pControlBlock ) {
    other.pControlBlock = nullptr;
}

AppPtr::AppPtr( const WeakAppPtr& weakAppPtr ) noexcept : pControlBlock( weakAppPtr.pControlBlock ) {
    if ( pControlBlock ) {
        if ( pControlBlock->isAlive()) pControlBlock->point();
        else pControlBlock = nullptr;
    }
}

AppPtr::AppPtr( WeakAppPtr&& weakAppPtr ) noexcept : pControlBlock( weakAppPtr.pControlBlock ) {
    if ( pControlBlock ) {
        if ( pControlBlock->isAlive()) pControlBlock->point();
        else pControlBlock = nullptr;
    }
    weakAppPtr.reset();
}

AppPtr& AppPtr::operator=( const AppPtr& other ) noexcept {
    if ( pControlBlock == other.pControlBlock ) return *this;

    reset();
    if ( other.pControlBlock ) {
        pControlBlock = other.pControlBlock;
        pControlBlock->point();
    }
    return *this;
}

AppPtr& AppPtr::operator=( AppPtr&& other ) noexcept {
    if ( pControlBlock == other.pControlBlock ) return *this;

    reset();
    pControlBlock = other.pControlBlock;
    other.pControlBlock = nullptr;
    return *this;
}

AppPtr& AppPtr::operator=( const WeakAppPtr& weakAppPtr ) noexcept {
    if ( pControlBlock == weakAppPtr.pControlBlock ) return *this;

    reset();
    if ( weakAppPtr.pControlBlock && weakAppPtr.pControlBlock->isAlive()) {
        pControlBlock = weakAppPtr.pControlBlock;
        pControlBlock->point();
    }
    return *this;
}

AppPtr& AppPtr::operator=( WeakAppPtr&& weakAppPtr ) noexcept {
    if ( pControlBlock == weakAppPtr.pControlBlock ) return *this;

    reset();
    if ( weakAppPtr.pControlBlock && weakAppPtr.pControlBlock->isAlive()) {
        pControlBlock = weakAppPtr.pControlBlock;
        pControlBlock->point();
    }
    weakAppPtr.reset();
    return *this;
}

void AppPtr::reset() noexcept {
    if ( !pControlBlock ) return;
    pControlBlock->unPoint();
    pControlBlock = nullptr;
}

AppPtr& AppPtr::operator=( std::nullptr_t ) noexcept {
    reset();
    return *this;
}
