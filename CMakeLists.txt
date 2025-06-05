cmake_minimum_required(VERSION 3.10)

# set the project name
project(mcp-server)

set(CMAKE_CXX_STANDARD 20)

if (NOTIDY)
else ()
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=-*,clang-*,cppcoreguidelines-*,-cppcoreguidelines-avoid-magic-numbers,performance-*,-cppcoreguidelines-pro-bounds-array-to-pointer-decay,llvm-*,modernize-*,-modernize-use-trailing-return-type,cert-*,-performance-avoid-endl")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()


# add the executable
add_executable(mcp-server main.cpp) 


