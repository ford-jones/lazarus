::	Cleanup previous build output
del build
del lib
rmdir build
rmdir lib

::	Check user input for debug flag
::	Compile from source
IF "%~1"=="" (
	cl /EHsc /std:c++17 /Zc:__cplusplus /c src/*.cpp /link glew32.lib glfw3.lib freetype.lib fmod_vc.lib
) ELSE IF "%~1"=="--debug" (
	cl /EHsc /std:c++17 /Zc:__cplusplus /Zi /DEBUG:FULL /D_LAZARUS_DEBUG_BUILD /c src/*.cpp /link glew32.lib glfw3.lib freetype.lib fmod_vc.lib
) ELSE (
	echo "Exiting: Invalid argument."
	exit /B
)

::	Build library
lib *.obj /out:liblazarus.lib

::	Move to out dir(s)
mkdir build
mkdir lib
move *.obj build
move *.lib lib