# Getting Started:
Hello and welcome to the Lazarus Project. \

Before jumping into the project, make sure the following libraries / tools are accessible on your machine:
- GCC / G++ (See: [Resources](./resources.md)) or [MSVC](https://visualstudio.microsoft.com/downloads/)
- [OpenGL](https://www.khronos.org/opengl/wiki/Getting_Started#Downloading_OpenGL) (Note: Most modern OS's ship with OpenGL).
- [GLFW](https://www.glfw.org/download.html)
- [GLM](https://sourceforge.net/projects/glm.mirror/)
- [GLEW](https://glew.sourceforge.net/)
- stb ([stb_image.h](https://raw.githubusercontent.com/nothings/stb/master/stb_image.h), [stb_image_resize.h](https://raw.githubusercontent.com/nothings/obbg/refs/heads/master/stb/stb_image_resize.h))
- [FreeType2](https://sourceforge.net/projects/freetype/files/freetype2/)
- [FMOD Core](https://www.fmod.com/download#fmodengine)

If any of these are unavailable to you, downloads can be found in the resources section. \
*Note: gcc, glfw, glm & glew are available for macOS using* `homebrew` *and should be installed from there.*

## Compiling the application:

### Unix (Linux / Mac):
Compile lazarus using the makefile: 
```
make
```
*(Note: Use `make optimise` to enable level O3 compiler optimisations. `make debug` will reveal debugging symbols required by `gdb` and `valgrind`.)*

Followed by:
```
make install
```

To uninstall the library and all associated files:
```
make uninstall
```

At any time, if you want to cleanup the project's build files locally:
```
make clean
```

### Windows:
If using windows you will need to install Microsoft Visual Studio and use it's compiler. This hasn't been tested on versions below 2022. \
In the project root use Powershell or the Developer Command Prompt for VS 2022 to run the batchfile:
```
build.bat
```
*(Note: Use `build.bat --debug` to enable debugging symbols in the build output and create a `.pdb` file in the current working directory.)*

Now copy the header files located in include to msvc's include folder. Mine's located at:
```
c:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.42.34433\include
```
Move the lazarus library (`lib/liblazarus.lib`) to msvc's lib folder:
```
c:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.42.34433\lib
```

## Usage:
You should now be able to use Lazarus with your project like so:
```cpp
#include <lazarus.h>

int main()
{
    Lazarus::WindowManager window = Lazarus::WindowManager("Game Window", nullptr, nullptr);
    window.initialise();

    return 0;
}
```
See [here](./lazarus-by-example.md) for further usage guides.

When compiling your project you will need to pass the following linker flags:
#### Unix (Linux / Mac):
```
g++ main.cpp -o run -lGL -lGLEW -lglfw -lfmod -freetype -llazarus
```

#### Windows:
```
cl /EHsc /std:c++17 /Zc:__cplusplus main.cpp /link fmod_vc.lib freetype.lib glfw3.lib glew32.lib opengl32.lib liblazarus.lib msvcrt.lib user32.lib gdi32.lib shell32.lib /out:run.exe /NODEFAULTLIB:libcmt
```

## Installation Notes:
For generalised notes on how to install a system library, see [here.](./contribution.md#file-structure)

1. gcc / g++: \
If you are installing gcc / g++ on mac using homebrew, make sure to create a new symlink in your `$PATH`. It needs to occur *before* your `clang` compiler which is used as the compiler for macOS systems by default. Once your install is complete, this can be done like so; 

Mac (intel):
```
sudo ln -sf /usr/local/bin/gcc-11 /usr/local/bin/gcc
```

Mac (silicone): 
```
sudo ln -s /opt/homebrew/bin/gcc-11 /usr/local/bin/gcc
```

If you are using a silicone-based chip (M1 / M2), you may need to log out of your shell by running: 
```
exit
```
Once thats' done, restart your terminal for the change to take effect.

2. stb_image : \
`stb_image` is a public domain single header library for parsing and loading image files. \

Get the source from github. (*If you don't have `curl` visit the url and download it manually.*):
```
curl -o ./stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
```

Move the file into your include folder:
```
sudo mv ./stb_image.h /usr/local/include/
```

3. FMOD core API: \
`FMOD` is an audio engine built by Firelight Technologies Pty Ltd. The software operates under multiple distribution liscences dependant on the project's use-case, take a look at https://www.fmod.com/licensing/ to determine which liscence you need for your project.

Before `FMOD` can be used, you need to register an account on their website here https://www.fmod.com/profile/register.

Once that's done, navigate to https://www.fmod.com/download#fmodengine and download version `2.02.21` for your target OS.

Once the download is complete, extract the packages from the `.zip`.

`FMOD` libraries are available for a number of different architectures. Check which architecture your OS is running with:
```
lscpu
```
The first line should print the system's architecture, for me it's `x86_64`.

Move the *contents* of the FMOD architecture sub-directory to your `lib` folder, e.g:
```
sudo mv ./fmodstudioapi2022<OS>/api/core/lib/<your_architecture>/* /usr/local/lib
```

Do the same for the `FMOD` headers:
```
sudo mv ./fmodstudioapi2022<OS>/api/core/inc/* /usr/local/include
```

Finally, if you're running linux you will need to update your loaders' scoped libraries with:
```
sudo ldconfig
```

**If you're installing FMOD on macOS and encounter the following message:**

*“libfmod.dylib” can’t be opened because Apple cannot check it for malicious software.*

Run the following commands:
```
xattr -d com.apple.quarantine /usr/local/lib/libfmod.dylib
xattr -d com.apple.quarantine /usr/local/lib/libfmodL.dylib
```

## Known caveats and limitations:
1. Regarding Wavefront file format: \
1.A: Your assets must be **triangulated**, this can be done prior to or on export. \
1.B: Paths must be *stripped*, i.e. exports should include supplementary filenames *only* - with pathing truncated. \
1.C: Texture files (Those specified by an `.mtl` file's `Map_Kd` property) should be stored in the same directory as the `.mtl` file. \
1.D: Materials that appear in `.mtl` material files **must** appear in order they were created. A safe way to ensure this is to number any *named* materials during the modeling process (e.g. `1_metal`). This is because named materials are often exported in alphabetical order by modeling software which can lead to undesired loading behaviour.
2. This project version locks OpenGL to version 4.1 for compatibility with MacOS.
3. Texture images used in any scene should all have the same pixel width and height. The scene *should* still render but you can expect to find holes in your textures. Use `GlobalsManager::enforceImageSanity` to ensure images are resized on load.
4. The xy coordinate system of the orthographic camera created by `Camera::createOrthoCam` has `0.0` mapped to the top left corner of the window, while the perspective camera `Camera::createPerspectiveCam` uses the bottom left.
5. There is no kerning or centering of TrueType fonts loaded by the `TextManager` class. If you need to render perfectly aligned text it may be better to render it directly to a quad as a texture (See: `Mesh::createQuad`). It could then be rendered along with the rest of the text by loading in an orthographic camera (See: `Camera::createOrthoCam`).
6. The maximum number of lights permitted in any one scene while using the `LAZARUS_DEFAULT_VERT_SHADER` and/or `LAZARUS_DEFAULT_FRAG_SHADER` is limitted to a maximum size of 150.
7. The maximum number of entities in any one scene who can be picked or looked up using a pixel with `CameraManager::getPixelOccupant` is limmited to a maximum size of 255.
8. The gdb loader does not yet support animation.