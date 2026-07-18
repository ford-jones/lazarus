cmake_minimum_required(VERSION 3.23)
project(LazarusEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# might not need this
# find_package(Filesystem REQUIRED)

add_library(lazarus SHARED)

add_subdirectory(include)
add_subdirectory(src)

# Load source files
include(src/CMakeLists.txt)
foreach(FILENAME ${LAZARUS_SOURCES})
    message(src/${FILENAME})
    set(LazarusSourceFiles ${LazarusSourceFiles} src/${FILENAME})
endforeach()

# Load header files
include(include/CMakeLists.txt)
set(LazarusEntryPoint include/${LAZARUS_ENTRY_POINT})
message(${LazarusEntryPoint})
foreach(FILENAME ${LAZARUS_HEADERS})
    message(include/${FILENAME})
    set(LazarusHeaderFiles ${LazarusHeaderFiles} include/${FILENAME})
endforeach()

target_include_directories(lazarus PRIVATE src)
target_include_directories(lazarus PRIVATE include)

target_compile_options(
    lazarus 
    PRIVATE 
    $<$<CXX_COMPILER_ID:MSVC>:/EHsc /Wall /Zc:__cplusplus /c>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fPIC -Wall>
)

# TODO:
# "Optimise" option (-O3) for MSVC

# Pass context specific build flags i.e. debug||optimised
if(D_LAZARUS_DEBUG_BUILD)
    target_compile_options(
        lazarus 
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/Zi /DEBUG:FULL>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-g>
    )
    target_compile_definitions(lazarus PRIVATE D_LAZARUS_DEBUG_BUILD)

elseif(D_LAZARUS_OPTIMISE_BUILD)
    target_compile_options(
        lazarus
        PRIVATE
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3>
    )
    target_compile_definitions(lazarus PRIVATE D_LAZARUS_OPTIMISE_BUILD)
endif()

# TODO:
# Get dependencies
# Linker/loader options

# Need to include std::filesystem for linux...
if(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
    target_link_libraries(lazarus PRIVATE stdc++fs)

elseif(${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
    target_link_libraries(
        lazarus 
        PRIVATE 
        -framework OpenGL
        -framework Cocoa
        -framework IOKit
    )
endif()

target_sources(
    lazarus
    PUBLIC
    ${LazarusEntryPoint}
    PRIVATE
    ${LazarusSourceFiles}
    ${LazarusHeaderFiles}
)

# TODO:
# have an install flag?
# install(TARGETS lazarus)
