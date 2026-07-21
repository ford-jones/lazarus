# Lazarus Engine CMAKE build 2026
# Author: Ford Jones
#
# This file is the successor of the previous build instructions which were system specific (makefile & build.bat).
# They are still available in the source tree for use in development but should not be used in releases as they
# require all dependencies to be manually installed ahead of time. Note that for Windows and MacOS the liblazarus
# artifact produced by the legacy build files is a static library instead of dynamically linked / shared.
#
# Usage: (For windows this guide uses the x64 Native Tools Command Prompt for VS 2022)
# 1. Generate the cmake build directories and cache with the following commands:
#	- windows: cmake -S . -G "Visual Studio 17 2022" -A x64 -B build
#	- unix: cmake -S . -B build
# 2. Compile the library and find (or download + install) the dependencies with:
#	cmake --build ./build --config Release
# 3. Install the project:
#	cmake --install ./build
# 4. Once installation is complete the following operations may need to be performed in order for consuming applications
#    to link to the library:
#	- windows: PATH=C:\Program Files\LazarusEngine\bin;%PATH%
#	- linux: sudo ldconfig
#	- MacOS: sudo update_dyld_shared_cache
# 5. Consuming applications can then link to lazarus in the following way:
#	- g++/gcc: g++ main.cpp -o example_project -llazarus
#	- MSVC: cl /Zc:__cplusplus main.cpp /I"C:/Program Files/LazarusEngine/include" /link msvcrt.lib user32.lib gdi32.lib shell32.lib /LIBPATH:"C:/Program Files/LazarusEngine/lib" lazarus.lib /out:example_project.exe /NODEFAULTLIB:libcmt
#
# Notes:
# 1. Windows users must use MSVC provided by Visual Studio 17 in order to generate the build files. Other generators
#    like MinGW are not supported.
# 2. This build must be run with elevated privelages. Run as administrator on windows, or use `sudo` on unix based
#    systems
# 3. Inclusion of audio tooling is optional because it requires the core api of FMOD sound subsystem, a dependency which
#    I dont have a liscence to distribute. To cull the audio management artifacts from the output, run the build with
#    `-D LAZARUS_EXCLUDE_AUDIO=1`. Otherwise, FMOD Core may be obtained from https://www.fmod.com/download#fmodengine.
#    Bare in mind that you're required to register an account in order to proceed with the download.
#
#
########### SETUP ###########
cmake_minimum_required(VERSION 3.6)
project(LazarusEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include(ExternalProject)
include(GNUInstallDirs)

add_library(lazarus SHARED)
set(lazarus PROPERTIES LINKER_LANGUAGE CXX)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_INSTALL_PREFIX}) # crazy that this is not default...

message("Target install directory: ${CMAKE_INSTALL_PREFIX}")

if(WIN32)
   set(EXT_DYNAMIC dll)
   set(EXT_STATIC lib)
elseif(APPLE)
   set(EXT_DYNAMIC dylib)
   set(EXT_STATIC a)
elseif(UNIX)
   set(EXT_DYNAMIC so)
   set(EXT_STATIC a)
endif()

########### PROJECT STRUCTURE ###########
# Directories
add_subdirectory(include)
add_subdirectory(src)
target_include_directories(lazarus PUBLIC ${CMAKE_INSTALL_PREFIX}/include)

# Source files
include(src/CMakeLists.txt)
foreach(FILENAME ${LAZARUS_SOURCES})
    message(src/${FILENAME})
    set(LazarusSourceFiles ${LazarusSourceFiles} src/${FILENAME})
endforeach()

# Header files
include(include/CMakeLists.txt)
set(LazarusEntryPoint include/${LAZARUS_ENTRY_POINT})
message(${LazarusEntryPoint})
foreach(FILENAME ${LAZARUS_HEADERS})
    message(include/${FILENAME})
    set(LazarusHeaderFiles ${LazarusHeaderFiles} include/${FILENAME})
endforeach()

########### DEPENDENCIES ###########
# TODO: Consider the following
# https://cmake.org/cmake/help/latest/module/FindOpenGL.html #macos-specific
# OpenGL
find_package(OpenGL REQUIRED)
if(${OPENGL_FOUND})
    message("Located OpenGL")
    target_link_libraries(lazarus PRIVATE OpenGL::GL)   # Uses OpenGL::OpenGL OR OpenGL::GLX (most portable non-EGL option)
else()
    message("FATAL: OpenGL is missing... ?")
endif()

# GLEW (GL Extension Wrangler)
find_package(GLEW)
if(${GLEW_FOUND})
    message("Located GLEW")
    target_link_libraries(lazarus PRIVATE GLEW::GLEW)   # Uses GLEW::glew OR GLEW::glew_s depending on what's available

else()
# TODO: 
# MacOS
    message("Preparing GLEW download")
    if(WIN32) # glew provides prebuilt binaries for windows
        ExternalProject_Add(
            libglew
            PREFIX ${CMAKE_BINARY_DIR}/glew
            URL https://github.com/nigels-com/glew/releases/download/glew-2.3.1/glew-2.3.1-win32.zip
            URL_HASH MD5=7c728cec5c9c3de95f1fc528a81570b7
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
			    ${CMAKE_BINARY_DIR}/glew/src/libglew/include/GL ${CMAKE_INSTALL_PREFIX}/include/GL &&
			    ${CMAKE_COMMAND} -E copy_directory
		            ${CMAKE_BINARY_DIR}/glew/src/libglew/lib/Release/x64 ${CMAKE_INSTALL_PREFIX}/lib &&
			    ${CMAKE_COMMAND} -E copy
			    ${CMAKE_BINARY_DIR}/glew/src/libglew/bin/Release/x64/glew32.${EXT_DYNAMIC} ${CMAKE_INSTALL_PREFIX}/bin/glew32.${EXT_DYNAMIC} 
        )
    else()
        ExternalProject_Add(
            libglew
            PREFIX ${CMAKE_BINARY_DIR}/glew
            URL https://github.com/nigels-com/glew/releases/download/glew-2.3.1/glew-2.3.1.zip
            URL_HASH MD5=b3cfe0ea0747e523daf15d39462e53c0
            CONFIGURE_COMMAND ""
            BUILD_COMMAND make -C <SOURCE_DIR>
            INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory # Manually install because `make install` doesnt seem to install the glew shared/static libraries, only the headers?
                            ${CMAKE_BINARY_DIR}/glew/src/libglew/include/GL ${CMAKE_INSTALL_PREFIX}/include/GL &&
                            ${CMAKE_COMMAND} -E copy_directory 
                            ${CMAKE_BINARY_DIR}/glew/src/libglew/lib ${CMAKE_INSTALL_PREFIX}/lib
        )
    endif()

    add_dependencies(lazarus libglew)

    set(GLEW_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/GL)
    set(GLEW_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib) 

    add_library(GLEW SHARED IMPORTED)
    if(WIN32)
       set(GLEW_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
       set_target_properties(GLEW PROPERTIES IMPORTED_LOCATION ${GLEW_BIN_DIR}/glew32.${EXT_DYNAMIC})
       set_target_properties(GLEW PROPERTIES IMPORTED_IMPLIB ${GLEW_LIB_DIR}/glew32.${EXT_STATIC}) # Seems to be a windows-only requirement ?
    else()
       set_target_properties(GLEW PROPERTIES IMPORTED_LOCATION ${GLEW_LIB_DIR}/libGLEW.${EXT_DYNAMIC})
    endif()
    
    include_directories(GLEW_INCLUDE_DIR)
    target_link_libraries(lazarus PRIVATE GLEW)

endif()

# GLFW
find_library(GLFW3_LIBRARY NAMES glfw3 glfw glfwdll glfw_mt)
if(${GLFW3_LIBRARY} STREQUAL GLFW3_LIBRARY-NOTFOUND)
# TODO:
# MacOS
    message("Preparing GLFW download")
    if(WIN32) # glfw provides prebuilt binaries for windows
	ExternalProject_Add(
	    libglfw
	    PREFIX ${CMAKE_BINARY_DIR}/glfw
            URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip
	    URL_HASH MD5=725c211378d00f3eee5df5acc8108742
            CONFIGURE_COMMAND ""
	    BUILD_COMMAND ""
	    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory 
			    ${CMAKE_BINARY_DIR}/glfw/src/libglfw/include/GLFW ${CMAKE_INSTALL_PREFIX}/include/GLFW &&
			    ${CMAKE_COMMAND} -E copy
			    ${CMAKE_BINARY_DIR}/glfw/src/libglfw/lib-vc2022/glfw3.${EXT_DYNAMIC} ${CMAKE_INSTALL_PREFIX}/bin &&
			    ${CMAKE_COMMAND} -E copy
			    ${CMAKE_BINARY_DIR}/glfw/src/libglfw/lib-vc2022/glfw3.${EXT_STATIC} 
			    ${CMAKE_BINARY_DIR}/glfw/src/libglfw/lib-vc2022/glfw3dll.${EXT_STATIC} 
			    ${CMAKE_BINARY_DIR}/glfw/src/libglfw/lib-vc2022/glfw3_mt.${EXT_STATIC} 
			    ${CMAKE_INSTALL_PREFIX}/lib/
	)

    else()
        ExternalProject_Add(
            libglfw
            PREFIX ${CMAKE_BINARY_DIR}/glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG 3.4
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ${CMAKE_COMMAND} -S <SOURCE_DIR> -B <SOURCE_DIR>/build && ${CMAKE_COMMAND} --build <SOURCE_DIR>/build --config Release
            INSTALL_COMMAND make -C <SOURCE_DIR>/build install
            GIT_PROGRESS ON
        )
    endif()
    add_dependencies(lazarus libglfw)

    set(GLFW_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/GLFW)
    set(GLFW_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)

    if(WIN32)
         add_library(glfw3 SHARED IMPORTED)
         set(GLFW_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
         set_target_properties(glfw3 PROPERTIES IMPORTED_LOCATION ${GLFW_BIN_DIR}/glfw3.${EXT_DYNAMIC})
         set_target_properties(glfw3 PROPERTIES IMPORTED_IMPLIB ${GLFW_LIB_DIR}/glfw3.${EXT_STATIC})
    else()
         add_library(glfw3 STATIC IMPORTED)
         set_target_properties(glfw3 PROPERTIES IMPORTED_LOCATION ${GLFW_LIB_DIR}/libglfw3.${EXT_STATIC})   # As far as I'm aware, glfw is only available as a static lib on unix (?)
    endif()
    
    include_directories(GLFW_INCLUDE_DIR)
    target_link_libraries(lazarus PRIVATE glfw3)

else()
    message("Located GLFW")
    target_link_libraries(lazarus PRIVATE ${GLFW3_LIBRARY})
endif()

# FreeType2
find_package(Freetype 2.14.3)
if(${Freetype_FOUND})
    message("Located FreeType2")
    target_link_libraries(lazarus PRIVATE Freetype::Freetype)
else()
    # Annoyingly: Freetype installs its' headers incorrectly acrross all platforms I've tested.
    # [make install OR cmake --install] both install all headers under a directory named freetype2
    # e.g. freetype2/freetype/*.h and freetype2/ftbuild.h -- whereas freetype's own implementation always 
    # includes it's own headers with the following scheme "#include <freetype/*.h>" -- i.e. without the
    # freetype2 prefix.... the repective INSTALL_COMMAND's here aim to correct this.

    message("Preparing FreeType2 download")
    if(WIN32)
       ExternalProject_Add(
            libfreetype
            PREFIX ${CMAKE_BINARY_DIR}/freetype2
            URL https://sourceforge.net/projects/freetype/files/latest/download
            URL_HASH MD5=b1243d490ba113e9bead49597072520c
            CONFIGURE_COMMAND ""
	    BUILD_COMMAND ${CMAKE_COMMAND} -S <SOURCE_DIR> -B <SOURCE_DIR>/build -D BUILD_SHARED_LIBS=true -D CMAKE_BUILD_TYPE=Release && ${CMAKE_COMMAND} --build <SOURCE_DIR>/build --config Release
            INSTALL_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/freetype2/src/libfreetype/build/Release/freetype.${EXT_STATIC}  ${CMAKE_INSTALL_PREFIX}/lib/freetype.${EXT_STATIC} &&
                            ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/freetype2/src/libfreetype/build/Release/freetype.${EXT_DYNAMIC}  ${CMAKE_INSTALL_PREFIX}/bin/freetype.${EXT_DYNAMIC} &&
                            ${CMAKE_COMMAND} -E copy  ${CMAKE_BINARY_DIR}/freetype2/src/libfreetype/include/ft2build.h ${CMAKE_INSTALL_PREFIX}/include/ft2build.h &&
                            ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/freetype2/src/libfreetype/include/freetype ${CMAKE_INSTALL_PREFIX}/include/freetype
        )
    else()
	ExternalProject_Add(
	   libfreetype
	   PREFIX ${CMAKE_BINARY_DIR}/freetype2
           URL https://sourceforge.net/projects/freetype/files/latest/download
           URL_HASH MD5=b1243d490ba113e9bead49597072520c
           CONFIGURE_COMMAND ""
	   BUILD_COMMAND ${CMAKE_COMMAND} -S <SOURCE_DIR> -B <SOURCE_DIR>/build -D BUILD_SHARED_LIBS=true -D CMAKE_BUILD_TYPE=Release && make -C <SOURCE_DIR>/build
           INSTALL_COMMAND make -C <SOURCE_DIR>/build install && 
                           ${CMAKE_COMMAND} -E copy_directory ${CMAKE_INSTALL_PREFIX}/include/freetype2 ${CMAKE_INSTALL_PREFIX}/include &&
                           ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/include/freetype2/ft2build.h ${CMAKE_INSTALL_PREFIX}/include/ft2build.h &&
                           ${CMAKE_COMMAND} -E rm -R ${CMAKE_INSTALL_PREFIX}/include/freetype2
	)
    endif()
    add_dependencies(lazarus libfreetype)
 
    set(FT_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
    set(FT_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)

    add_library(Freetype SHARED IMPORTED)

    if(WIN32)
       set(FT_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
       set_target_properties(Freetype PROPERTIES IMPORTED_LOCATION ${FT_BIN_DIR}/freetype.${EXT_DYNAMIC})
       set_target_properties(Freetype PROPERTIES IMPORTED_IMPLIB ${FT_LIB_DIR}/freetype.${EXT_STATIC}) # Seems to be a windows-only requirement ?
    else()
       set_target_properties(Freetype PROPERTIES IMPORTED_LOCATION ${FT_LIB_DIR}/libfreetype.${EXT_DYNAMIC})
    endif()

    include_directories(FT_INCLUDE_DIR)
    target_link_libraries(lazarus PRIVATE Freetype)

endif()

# GLM
# Note: Header-only library
message("Preparing glm download")
ExternalProject_Add(
    glm
    PREFIX ${CMAKE_BINARY_DIR}/glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_BINARY_DIR}/glm/src/glm/glm ${CMAKE_INSTALL_PREFIX}/include/glm
    GIT_PROGRESS ON
)
add_dependencies(lazarus glm)
set(GLM_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/glm)
include_directories(GLM_INCLUDE_DIR)

# FMOD
if(NOT LAZARUS_EXCLUDE_AUDIO)
    if(WIN32)
	    # This is the default install target for the windows fmod installation wizard
	    set(FMOD_CORE_PATH "C:/Program Files/FMOD Studio API Windows/api/core/")
        find_library(
	        FMOD_LIBRARY 
	        NAMES fmod fmodL libfmod fmod_vc fmodL_vc 
	        PATHS ${FMOD_CORE_PATH}/lib/x64
        )

        if(FMOD_LIBRARY STREQUAL FMOD_LIBRARY-NOTFOUND)
	        message("Cannot find FMOD !")
        else()
	        message("Located FMOD")
            add_library(FMOD::Core SHARED IMPORTED)
	        set_target_properties(FMOD::Core PROPERTIES IMPORTED_LOCATION ${FMOD_CORE_PATH}/lib/x64/fmod.${EXT_DYNAMIC})
	        set_target_properties(FMOD::Core PROPERTIES IMPORTED_IMPLIB ${FMOD_CORE_PATH}/lib/x64/fmod_vc.${EXT_STATIC})
	        include_directories(${FMOD_CORE_PATH}/inc/)
	        target_link_libraries(lazarus PRIVATE FMOD::Core)
        endif()

    else()
        # TODO:
        # Would be really awesome if could refactor this to just be a single find_library or find_package call -- this is pretty terrible for Xplatform 
        set(FMOD_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
        set(FMOD_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)

        add_library(fmod SHARED IMPORTED)
        set_target_properties(fmod PROPERTIES IMPORTED_LOCATION ${FMOD_LIB_DIR}/libfmod.${EXT_DYNAMIC})
        include_directories(FMOD_INCLUDE_DIR)
        target_link_libraries(lazarus PRIVATE fmod)
    endif()

    set(LazarusSourceFiles ${LazarusSourceFiles} src/${LAZARUS_AUDIO_SRC})
    set(LazarusHeaderFiles ${LazarusHeaderFiles} include/${LAZARUS_AUDIO_HDR})

endif()

# Sean Barrett's stb
# Note: Header-only library
message("Downloading stb_image.h")
file(
    DOWNLOAD 
    https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h
    ${CMAKE_BINARY_DIR}/stb/stb_image.h
    SHOW_PROGRESS
)
file(INSTALL ${CMAKE_BINARY_DIR}/stb/stb_image.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
message("Downloading stb_image_resize.h")
file(
    DOWNLOAD
    https://raw.githubusercontent.com/nothings/obbg/refs/heads/master/stb/stb_image_resize.h
    ${CMAKE_BINARY_DIR}/stb/stb_image_resize.h
    SHOW_PROGRESS
)
file(INSTALL ${CMAKE_BINARY_DIR}/stb/stb_image_resize.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

# std::filesystem (linux only...)
if(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
    message("Linking filesystem")
    target_link_libraries(lazarus PRIVATE stdc++fs)

# cpp frameworks (MacOS only)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
    find_library(COCOA_FRAMEWORK Cocoa) # Required for usage of OOP patterns
    if(${COCOA_FRAMEWORK} STREQUAL COCOA_FRAMEWORK-NOTFOUND)
        message("FATAL: Cocoa OOP framework is missing...")
    else()
        message("Located Cocoa OOP framework")
        target_link_libraries(lazarus PRIVATE ${COCOA_FRAMEWORK})
    endif()

    find_library(IOKIT_FRAMEWORK IOKit) # Required for standard IO usage
    if(${IOKIT_FRAMEWORK} STREQUAL IOKIT_FRAMEWORK-NOTFOUND)
        message("FATAL: IOKit framework is missing...")
    else()
        message("Located IOKit framework")
        target_link_libraries(lazarus PRIVATE ${IOKIT_FRAMEWORK})
    endif()
endif()

########### COMPILER OPTIONS ###########
# Common
target_compile_options(
    lazarus 
    PRIVATE 
    $<$<CXX_COMPILER_ID:MSVC>: /EHsc /Zc:__cplusplus /c>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>: -fPIC>
)

# Normal vs debug vs optimised
if(_LAZARUS_DEBUG_BUILD)
    target_compile_options(
        lazarus 
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>: /Zi /Wall /DEBUG:FULL>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>: -g -Wall>
    )
    target_compile_definitions(lazarus PRIVATE _LAZARUS_DEBUG_BUILD)

elseif(_LAZARUS_OPTIMISE_BUILD)
    target_compile_options(
        lazarus
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>: /O2 /Oi >
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>: -O3>
    )
    target_compile_definitions(lazarus PRIVATE _LAZARUS_OPTIMISE_BUILD)
endif()

########### INSTALLATION SCOPE ###########
target_sources(
    lazarus
    PUBLIC
    ${LazarusEntryPoint}
    PRIVATE
    ${LazarusSourceFiles}
    ${LazarusHeaderFiles}
)

install(TARGETS lazarus)
install(
    FILES ${LazarusHeaderFiles} ${LazarusEntryPoint} 
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)

# compile windows: cl /EHsc /std:c++17 /Zc:__cplusplus main.cpp game.cpp /I"C:/Program Files/LazarusEngine/include" /I"C:/Program Files/FMOD Studio API Windows/api/core/inc" /link msvcrt.lib user32.lib gdi32.lib shell32.lib lazarus.lib /out:run.exe /NODEFAULTLIB:libcmt
