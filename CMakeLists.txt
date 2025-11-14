cmake_minimum_required(VERSION 3.20)
project(PrimeCalculator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable address sanitizer for debugging
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

add_executable(prime_calculator
    src/main.cpp
    src/prime_calculator.cpp
    include/prime_calculator.hpp
    src/thread_pool.cpp
    include/thread_pool.hpp
)

target_include_directories(prime_calculator PRIVATE include)

target_compile_features(prime_calculator PRIVATE cxx_std_20)

# Add threading library
find_package(Threads REQUIRED)
target_link_libraries(prime_calculator PRIVATE Threads::Threads)

# Enable more warnings
if(MSVC)
    target_compile_options(prime_calculator PRIVATE /W4 /WX)
else()
    target_compile_options(prime_calculator PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
