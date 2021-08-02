# AIMA in C++

C++ Implementations of some of the algorithms in [Norvig's](http://www.norvig.com/) [Artificial Intelligence - A Modern Approach 3rd Edition](http://aima.cs.berkeley.edu/).

## Building

Uses the following system libraries:
* Boost
* SQLite3
* OpenGL
* glfw
* clang std
* clang abi

Clone the repo recursively and run cmake. Build all or choose which executables you want.

MUST be built with clang. Currently setup for clang 7, to build with a later version, configure the project. This will build the cppcoro dependency. Check the name of the compiled library in the build tree and update vendor/CMakeLists.txt accordingly. This is necessary because cppcoro uses a differnt build system that renames the output depending on the compiler used.

## Running

There are two ways to run the algorithms:
1. Run the integrated app and choose individual applets or demos from the drop down menu.
2. Run the desired app or demo as an independent executable.
