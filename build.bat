cl /EHsc /std:c++17 /c src/*.cpp /link glew32.lib glfw3.lib freetype.lib fmod_vc.lib

lib *.obj /out:liblazarus.lib

mkdir build
mkdir lib

move *.obj build
move *.lib lib