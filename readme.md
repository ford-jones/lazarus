# Lazarus Engine
#### *Version: 0.14.2*
## Table of contents:

- [Getting Started](#getting-started)
- [Lazarus by Example](#lazarus-by-example)
- [API Reference](#api-reference)
- [Resources](#resources)
- [Known Limitations](#known-caveats-and-limitations)

![OpenGL Logo](https://drive.google.com/thumbnail?id=1Wnm1g7fGtYi3vqHWuypVgbHMarHNyE1y&sz=w280)

![GLM Logo](https://drive.google.com/thumbnail?id=1J3yivgfIojpvRlFeGXl75RirZYOBts-z&sz=w200)

![FMOD Logo](https://drive.google.com/thumbnail?id=1KNnrRvWJHEOgRYyJ2W2M3MPjKc_StMOd&sz=w400)

![GLFW Logo](https://drive.google.com/thumbnail?id=1woJUTrAZ7Af7NZEmVmeBxVquGlZT-d0g&sz=w200)

------------------------------------------------------------------

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
See [here](#lazarus-by-example) for further usage guides.

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
------------------------------------------------------------------

# Lazarus by Example:
Welcome to the examples guide. While the following was written in a Linux environment, the snippets below have been tested on Windows 10 and MacOS. \
This guide covers the following:
1. [The application window](#window-context)
2. [Configuration](#global-settings)
3. [Loading mesh](#mesh-assets)
4. [Shader Workflow](#shaders)

## Application window:
### Creating a window:
Before we can start drawing any graphics, we need somewhere to draw to. Generally speaking, one of the first things you will want to do is create an application window. Lazarus makes this process simple. Consider the following:
```cpp
#include <lazarus.h>

int main()
{
    //  Instantiate the window object
    Lazarus::WindowManager window("Lazarus Tutorial", 800, 600);
    window.createWindow();

    //  Load user settings from global state
    window.loadConfig();

    // Invoke render loop
    window.open();
    
    while(window.isOpen)
    {
        std::cout << "Window is open!" << std::endl;
    };

    return 0;
}
```

Remember that at any point we can shutdown the program like so:
```cpp
bool gameOver = false;

while(window.isOpen)
{
    if(gameOver)
    {
        std::cout << "Exiting..." << std::endl;
        window.close();
    };
};
```

### Presenting a frame:
Our program should now compile and launch successfully. \
But this is just a blank screen? Lets try draw something! The simplest draw command we can make right now without any further setup is to change the backbuffer color of our window, so lets do that.

```cpp
bool gameOver = false;
float count = 0.0;

while(window.isOpen)
{
    count += 0.001;
    window.setBackgroundColor(count, count / 2.0, count / 5.0);

    if(gameOver)
    {
        std::cout << "Exiting..." << std::endl;
        window.close();
    };
};
```
But nothing's happening!? \
This is because Lazarus uses double buffering, meaning that while frame 'A' is being presented, frame 'B' is being drawn to. To ensure our frames are being swapped accordingly we need to call the following somewhere inside our render loop. Usually at the end of this cycle's operations.

```cpp
window.presentNextFrame();
```

At this point your application may start recieving messages from the OS such as the one in the image below. Don't worry - this is normal, as our application does not yet have anything to respond *to.* We'll remediate this in the next section which looks at how Lazarus manages events. \
![Error](https://drive.google.com/thumbnail?id=120qmRnMSWGpAxI9UAYFf1wxnL_DpFvTK&sz=w800)

### Managing events:
Events in Lazarus are intrinsically tied to the window context in which they were created and dispatched. The window object in question will inherit an `EventManager` interface, which uses callbacks to retrieve information from the programs event loop asynchronously without blocking the application's control flow. 

The lazarus callback functions are abstracted in userspace, taking the form of event-listeners such as the one below.
```cpp
//  Initialise the window's event manager
window.eventsInit();

while(window.isOpen)
{
    //  Listen for events
    window.monitorEvents();
    
    //  Retrieve and use window state
    float red = static_cast<window.mousePositionX> / 100.0;
    float blue = static_cast<window.mousePositionY> / 100.0;

    window.setBackgroundColor(red, 0.0, blue);

    if(gameOver)
    {
        std::cout << "Exiting..." << std::endl;
        window.close();
    }
    else
    {
        window.presentNextFrame();
    }
};
```

Your application should now blend from varying shades of pink, purple and blue depending on the cursor's screen position. \
Our problem from the window creation section should now be resolved, as our application successfully responds to defined inputs.

Next let's try listen for a key event and use it to close our window. We can do so by fulfilling our `gameOver` condition.
```cpp
while(window.isOpen)
{
    //  Listen for events
    window.monitorEvents();
    
    //  Retrieve and use window's cursor location
    float red = static_cast<window.mousePositionX> / 100.0;
    float blue = static_cast<window.mousePositionY> / 100.0;

    //  Check the window's keypress state
    if(window.keyEventCode > 0) gameOver = true;

    window.setBackgroundColor(red, 0.0, blue);

    if(gameOver)
    {
        std::cout << "Exiting..." << std::endl;
        window.close();
    }
    else
    {
        window.presentNextFrame();
    };
};
```

See the [Window API Reference](#windowmanager) for more information.

## Global Settings:
### Modifying the Lazarus state:
Lazarus keeps track of user settings via the `Lazarus::GlobalsManager` class. This class provides functionality to get and set the state of internally tracked and externally declared Lazarus configuration variables. User settings *must* be specified prior to calling `Lazarus::WindowManager::loadConfig()`. For example:
```cpp
Lazarus::GlobalsManager globals = Lazarus::GlobalsManager();

//  Correct usage: the program will render frames as quickly as possible despite the monitor's maximum refresh rate.
globals.setVsyncDisabled(true);
window.loadConfig();
//  Incorrect usage: The value returned by globals.getLaunchInFullScreen() will reflect your selection, but will be inefective.
globals.setLaunchInFullscreen(true);
```

Find a full list of settings functions [here](#globalsmanager).

## Mesh assets:
### Hello Quad!
Now that we have a window open, let's render some geometry. To view geometry on screen we need some place to view *from*, this is our camera. \
For now we'll use an orthographic camera which projects the scene into two dimensions devoid of perspective. The orthographic camera observes the scene relative to the pixel dimensions of the viewing plane, these are the dimensions of your application window x2. We're also going to link the Lazarus default shader program, don't worry too much about this right now as it will be covered later in [Shaders](#shaders).

```cpp
#include <lazarus.h>

int main()
{
    //  Engine state interface
    Lazarus::GlobalsManager globals = Lazarus::GlobalsManager();    

    //  Window
    Lazarus::WindowManager window("Lazarus Tutorial");
    window.createWindow();
    window.setBackgroundColor(1.0, 0.0, 0.0);
    window.loadConfig();

    //  Default shader
    Lazarus::Shader shader = Lazarus::Shader();
    int shaderID = -1;
    shader.compileShaders(shaderID);    //  shaderID assumes the index of the newly linked shader
    shader.setActiveShader(shaderID);   //  activate the shader found at shaderID

    //  Camera
    Lazarus::CameraManager cameraManager = Lazarus::CameraManager(shaderID);

    //  Configure camera settings
    Lazarus::CameraManager::CameraConfig cameraSettings = {};
    cameraSettings.aspectRatioX = globals.getDisplayWidth();
    cameraSettings.aspectRatioY = globals.getDisplayHeight();

    //  Generate camera
    Lazarus::CameraManager::Camera camera = {};
    cameraManager.createOrthoCam(camera, cameraSettings); //  upon success, camera is given a value
```

Now lets create our geometry:
```cpp
    //  Instantiate a new mesh manager (this can be stack'd or heap'd, here we use the stack).
    //  The ID of the shader that will be used to render this instance's resources must be provided
    Lazarus::MeshManager meshManager = Lazarus::MeshManager(shaderID);

    //  Configure the mesh asset
    //  Note: when orthographically viewed, sizing corresponds to pixel-width
    Lazarus::MeshManager::QuadConfig quadSettings = {};
    quadSettings.width = 500;
    quadSettings.height = 500;

    //  Generate asset
    Lazarus::MeshManager::Quad quad = {};
    meshManager.createQuad(quad, quadSettings); //  upon success, quad is given a value
```

The active shader has been set and our resources are prepared. From here we can begin our render loop, where per-frame we will load our scene's data onto the GPU and use our shader to render the output upon each draw call. Then we present the next frame to observe the outcome.
```cpp
    //  Set background color so quad is visible
    window.setBackgroundColor(1.0, 1.0, 1.0);
    window.open();
    window.eventsInit();

    while(window.isOpen)
    {
        window.monitorEvents();

        //  Load resources
        cameraManager.loadCamera(camera);
        meshManager.loadMesh(quad);

        //  Draw next frame
        lazarus_result engineStatus = meshManager.drawMesh(quad);

        //  Check errors
        //  Note: a lazarus_result shall be returned by most functions in the api
        if(engineStatus != LAZARUS_OK)
        {
            window.close();
        };

        window.presentNextFrame();
    }

    //  Exit
    return 0;
};
```

You should see something like this: congratulations - you've just drawn your first scene with Lazarus! \
![helloQuad1](https://drive.google.com/thumbnail?id=174WZ32_dxPwt21R8WW5k7vS-xvaukIdi&sz=w800)

### Transforming assets:
*But why is it all the way down there?* Because under observation of our orthographic camera, metrics are taken in pixel dimensions from the bottom-left corner of the viewport. Although it may be confusing in this context, it might make sense a little later if you are looking to create UI or HUD components. This is the same coordinate system that is used internally for the layout of text and glyphs. \
To fix the issue; lets center our quad using a transform.
```cpp
    meshManager.createQuad(quad, quadSettings);

    // After creating the quad, move it to the center of the screen
    Lazarus::Transform transformer = Lazarus::Transform();
    transformer.translateMeshAsset(
        quad, 
        static_cast<float>(globals.getDisplayWidth()) / 2.0, 
        static_cast<float>(globals.getDisplayHeight()) / 2.0, 
        0.0
    );
```
    
Here it is! You'll also observe that the geometry was partially clipped. With the quad centered we can now see it in full. \
![helloQuad2](https://drive.google.com/thumbnail?id=1HNdYXlDgjj1AOdB9BZNAjpgJGqTTICwh&sz=w800)

For more on this; see the [Mesh section of the API Reference](#meshmanager).

## Shaders:
### Getting started with shaders:
Shader programs executed on the graphics processor are deterministic of the render result as seen on-screen. Responsible for operations like interpretting geometric data relative to the viewport (vertex shader) to computing the final output color on a per-pixel basis (fragment shader). \
To draw anything to the screen at all, we need to upload our shader program to the GPU. For now we'll look at linking the Lazarus default shader program, how to use your own is covered in the next section.

### Using your own shaders:
Shaders submitted to and compiled by Lazarus should be written in GLSL at version **410**. \
When you submit your own fragment and / or vertex shaders with a call to `Lazarus::Shader::compileShaders()`, there are a number of layout positions, as well as; sampler and uniform names reserved by Lazarus. These variables are accessible in any of your own shader programs and are listed below.

#### Vertex shader inputs:
Note these inputs can be found at `LAZARUS_DEFAULT_VERT_LAYOUT`.
```c
layout(location = 0) in vec3 inVertex;      //  Input Vertex position
layout(location = 1) in vec3 inDiffuse;     //  Input Vertex color
layout(location = 2) in vec3 inNormal;      //  Input Vertex normal
layout(location = 3) in vec3 inTexCoord;    //  Input UV (S/T & stack-index)

uniform int usesPerspective;                //  Which projection type to use, 1 for perspective - otherwise orthographic
uniform mat4 modelMatrix;                   //  The render subject's modelmatrix
uniform mat4 viewMatrix;                    //  The camera's viewing matrix
uniform mat4 perspectiveProjectionMatrix;   //  A 3D projection matrix (if one is present)
uniform mat4 orthoProjectionMatrix;         //  A 2D projection matrix (if one is present)

out vec3 fragPosition;                      //  Output position
out vec3 diffuseColor;                      //  Output color data
out vec3 normalCoordinate;                  //  Output normal coordinates
out vec3 textureCoordinate;                 //  Output UV for render subject
out vec3 skyBoxTextureCoordinate;           //  Output for skybox UV

flat out int isUnderPerspective;            //  Output required by default program for rendering text / glyphs

vec3 _lazarusComputeWorldPosition();        //  Determine the output vertex position in worldspace coordinates and calculates the relevant clip-space coordinates
vec3 _lazarusComputeNormalDirection()       //  Determine the direction vector of the output vertex normals. Ensures preservation of the normal direction over non-uniform surfaces.
```

#### Pixel / Fragment shader inputs:
Note these inputs can be found at `LAZARUS_DEFAULT_FRAG_LAYOUT`. \
Anything not used from here will be optimised-out when compiled.
```c
    #define MAX_LIGHTS 150                          

    //  Texture storage types for comparisson with samplerType
    const int CUBEMAP   = 1;
    const int ATLAS     = 2;
    const int ARRAY     = 3;

    //  Light variants
    const int DIRECTIONAL_LIGHT = 1;
    const int POINT_LIGHT       = 2;

    in vec3 fragPosition;                                   //  Input 3D fragment position
    in vec3 diffuseColor;                                   //  Input fragment color
    in vec3 normalCoordinate;                               //  Input fragment normals
    in vec3 textureCoordinate;                              //  Input UV coordinates 
    in vec3 skyBoxTextureCoordinate;                        //  Input UV coords for skyboxes

    flat in int isUnderPerspective;                         //  1 if a perspective camera is being used to observe this fragment, otherwise 0

    uniform int lightCount;                                 //  The number of lights currently bound to lazarus
    uniform int lightTypes[MAX_LIGHTS];                     //  Contains the active light type (point-light/directional) located at [i]
    uniform vec3 lightDirections[MAX_LIGHTS];               //  A container of direction-vectors pertaining to lights of the DIRECTIONAL_LIGHT type
    uniform vec3 lightPositions[MAX_LIGHTS];                //  A container of 3D light positions pertaining to lights of type POINT_LIGHT in-order
    uniform vec3 lightColors[MAX_LIGHTS];                   //  A container of light color values in-order
    uniform float lightBrightness[MAX_LIGHTS];              //  A container of light brightness variables in-order

    uniform vec3 fogColor;                                  //  The color of environmental fog if it is present
    uniform vec3 fogViewpoint;                              //  Where the epicenter from which fog thickness attenuates out from
    uniform float fogMaxDist;                               //  The maximum distance at which environment fog effects are observable
    uniform float fogMinDist;                               //  The minimum distance at which environment fog effects are observable
    uniform float fogDensity;                               //  Thickness of environment fog if present

    uniform vec3 textColor;                                 //  The color of the text, if the fragment is part of an ascii glyph

    uniform int samplerType;                                //  In-storage variant for identification of texture samplers
    uniform int discardFrags;                               //  Whether or not fragments with a diffuse-alpha value of zero should be discarded.

    uniform sampler2D textureAtlas;                         //  Glyph atlas used for font bitmaps
    uniform sampler2DArray textureArray;                    //  Array sampler / testure stack
    uniform samplerCube textureCube;                        //  Cubemap sampler used for skyboxes

    out vec4 outFragment;                                   //  The output fragment color

    vec4 _lazarusComputeColor();                            //  Evaluate inputs and determine fragment's rgba values
    vec3 _lazarusComputeLambertianReflection(vec3 color);   //  Calculate the fragment's diffuse lighting
    float _lazarusComputeFogFactor();                       //  Calculate fog attenuation
```
**It should be noted that `_lazarusComputeColor()` will only return a value for a textured mesh when it is called from the program which the `MeshManager` was instantiated with.**

------------------------------------------------------------------

# API Reference:
This API reference contains brief descriptions of each of the Lazarus classes; their contructors, functions and members and \
destructors. Private functions and members are currently omitted from this guide.

Return parameters are omitted from this guide as each of the functions in the lazarus library return a `void` or a `lazarus_result` (a uint16 status code). All of the `lazarus_result` codes can be found [here](#members).

For a comprehensive usage guide, visit [Lazarus by Example](#lazarus-by-example)

## GlobalsManager:

### Functions:
#### void setMaxImageSize(int width, int height)
Sets the value of `LAZARUS_MAX_IMAGE_WIDTH` and `LAZARUS_MAX_IMAGE_HEIGHT`. Used to determine the rescale size for all images loaded with `FileReader::readFromImage()` from hereafter.

*Notes:* 
- *Has no effect if the value returned by `GlobalsManager::getEnforceImageSanity()`* is `true`.
- *The `width` and `height` parameters must have values higher than 0 and no higher that 2048.*
- *Also effects the cursor image. Load your cursors first!*

Params:
>**width:** *The width in pixels to be used for image resizing.*
>**height:** *The height in pixels to be used for image resizing.*

#### int getMaxImageWidth()
Returns the current value of `LAZARUS_MAX_IMAGE_WIDTH`.

#### int getMaxImageHeight()
Returns the current value of `LAZARUS_MAX_IMAGE_HEIGHT`.

#### void setEnforceImageSanity(bool shouldEnforce)
Sets the value of `LAZARUS_ENFORCE_IMAGE_SANITY` and allows calls to `GlobalsManager::setMaxImageSize()` to take effect. Also ensures all images are flipped vertically on load.

Params:
>**shouldEnforce:** *Whether or not vertical flipping and image resizing is enforced on all images.*

#### bool getEnforceImageSanity()
Returns the current value of `LAZARUS_ENFORCE_IMAGE_SANITY`.

#### void setCursorHidden(bool shouldHide)
Sets the value of `LAZARUS_DISABLE_CURSOR_VISIBILITY`. When `true` the cursor pointer will become transparent when hovered over the active game window.

*Notes:*
- *Must be set prior to creation of a window with `WindowManager::initialise()`*
- *Cursor X and Y coordinates continue to update following a call to `EventManager::monitorEvents()` as if it were opaque.*

params:
>**shouldHide:** *Whether or not cursor opacity should be set to 0.*

#### bool getCursorHidden()
Returns the current value of `LAZARUS_DISABLE_CURSOR_VISIBILITY`.

#### void setLaunchInFullScreen(bool shouldEnlarge)
Sets the value of `LAZARUS_LAUNCH_IN_FULLSCREEN`. When `true` the application will launch at the maximum height and width values of the primary monitor.

parms:
>**shouldEnlarge** *Whether or not the application should launch in fullscreen by default.*

#### bool getLaunchInFullScreen()
Returns the current value of `LAZARUS_DISABLE_CURSOR_VISIBILITY`.

#### void setVsyncDisabled(bool shouldDisable)
Sets the value of `LAZARUS_VSYNC_DISABLED`. If true, the render pipeline will be allowed to render at it's maximum framerate. When false the interval between rendering and processing is set to 1 frame.

#### bool getVsyncDisabled()
Returns the current value of `LAZARUS_VSYNC_DISABLED`.

#### void setBackFaceCulling(bool shouldCull)
Sets the value of `LAZARUS_CULL_BACK_FACES`. I don't reccomend disabling this optimisation but if you want to you can... Ensures that faces opposite to the camera aren't rendered. Front face culling is currently unsupported through lazarus but you can enable it yourself using OpenGL (prior to window creation) like so:

```cpp
glEnable(GL_CULL_FACE);
glCullFace(GL_FRONT);
```

params:
>**shouldCull:** *Whether or not to disable the rendering of faces that are currently out of sight.*

#### bool getBackFaceCulling()
Returns the current value of `LAZARUS_CULL_BACK_FACES`.

#### void setDepthTest(bool shouldTest)
Sets the value of `LAZARUS_DEPTH_TEST_FRAGS`. Again, I don't reccomend disabling this setting. Informs OpenGL that we want it to perform a depth test on the current fragment that is being drawn against the rest of the frame buffers contents. Determines what is in-front or behind. Turning this off can have a disastrous effect on the render result.

params:
>**shouldTest:** *Whether or not OpenGL should should check which fragments are in-front or behind of eachother.*

#### getDepthTest()
Returns the current value of `LAZARUS_DEPTH_TEST_FRAGS`.

#### void setNumberOfActiveLights(int count)
Sets the value of `LAZARUS_LIGHT_COUNT`. Updates the total number of lightsources known to the render context. Don't do this.

params:
>**count:** *The total number of lights.*

#### int getNumberOfActiveLights()
Returns the number of lights known accross all `LightManager` instances.

### Members:
> **lazarus_result**: Various execution status codes (*type:* `enum`)
> - **LAZARUS_OK** *The engines default state. No problems. (Code: 0)* 
> - **LAZARUS_FILE_NOT_FOUND** *The specified asset couldn't be found (Code: 101)* 
> - **LAZARUS_FILE_UNREADABLE** *The located file cannot be read. (Code: 102)* 
> - **LAZARUS_FILESTREAM_CLOSED** *The filestream input closed unexpectedly. (Code: 103)* 
> - **LAZARUS_IMAGE_LOAD_FAILURE** *STB was unable to load the contents of the given image file into a 8_8_8_8 (RGBA) buffer. (Code: 104)*
> - **LAZARUS_IMAGE_RESIZE_FAILURE** *STB was unable to resize the image to the height and width specified at `LAZARUS_MAX_IMAGE_WIDTH` / `LAZARUS_MAX_IMAGE_HEIGHT` (Code: 105)*
> - **LAZARUS_SHADER_ERROR** *OpenGL does not regard the output from shader compilation to be a valid shader program. (Code: 201)* 
> - **LAZARUS_VSHADER_COMPILE_FAILURE** *The vertex shader failed to compile. (Code: 202)*
> - **LAZARUS_FSHADER_COMPILE_FAILURE** *The fragment shader failed to compile. (Code: 203)
> - **LAZARUS_SHADER_LINKING_FAILURE** *OpenGL failed to link the shaders. (Code: 204)*
> - **LAZARUS_UNIFORM_NOT_FOUND** *Lazarus failed to perform a lookup on the desired uniform from the vertex or fragment shader. (Code: 205)*
> - **LAZARUS_MATRIX_LOCATION_ERROR** *Lazarus failed to perform a shader lookup on the desired modelview, projection or view-matrix required to render the target entity. (Code: 206)*
> - **LAZARUS_OPENGL_ERROR** *An error occured in the OpenGL graphics pipeline. (Code: 301)*
> - **LAZARUS_NO_CONTEXT** *Unable to find a window with an active OpenGL context. (Code: 302)*
> - **LAZARUS_WINDOW_ERROR** *An error occured in the window API. (Code: 303)*
> - **LAZARUS_EVENT_ERROR** *An error occured in the events API (Code: 304)*
> - **LAZARUS_GLFW_NOINIT** *GL framework wrangler failed to initialise. (Code: 305)*
> - **LAZARUS_WIN_EXCEEDS_MAX** *The requested window size is larger than the dimensions of the primary monitor. (Code: 306)*
> - **LAZARUS_TIME_ERROR** *Lazarus tried to perform a time operation but the windows running time was 0ms. (Code: 307)*
> - **LAZARUS_AUDIO_ERROR** *An error occured in the FMOD audio backend. (Code: 401)*
> - **LAZARUS_AUDIO_PLAYBACK_POSITION_ERROR** *Desired audio playback location was less than 0 seconds or more than AudioManager::Audio::length. (Code: 402)*
> - **LAZARUS_AUDIO_LOAD_ERROR** *Unable to load audio sample into a channel. (Code: 403)*
> - **LAZARUS_INVALID_RADIANS** *Lazarus recieved a rotational value which exceeds 360.0. (Code: 501)*
> - **LAZARUS_INVALID_CUBEMAP** *The images recieved to construct a cubemap texture are not all of equal height and width (Code: 502)*
> - **LAZARUS_INVALID_DIMENSIONS** *Lazarus recieved a mesh creation input with value(s) below 0.0 (Code: 504).*
> - **LAZARUS_INVALID_INTENSITY** *Lazarus recieved a asset-strength multiplier with a value below 0 (Code: 505).*
> - **LAZARUS_FEATURE_DISABLED** *A function was invoked that relies on settings which are disabled. (Code: 506)*
> - **LAZARUS_ASSET_LOAD_ERROR** *Failed to load a mesh asset (Code: 601)*
> - **LAZARUS_FT_INIT_FAILURE** *Lazarus failed to inititialise the freetype2 library. (Code: 701)*
> - **LAZARUS_FT_LOAD_FAILURE** *Freetype has indicated that it is unable to load the TrueType font from the desired location. (Code: 702)*
> - **LAZARUS_FT_RENDER_FAILURE** *Despite being able to load the target glyph's splines, freetype was unable to render them into a monochrome bitmap. (Code: 703)*

## WindowManager:
A class for making and managing the program's window(s). 

### Constructor:
#### WindowManager(const char *title, int width, int height)

Params:
> **title**: *The window's title* \
> **width**: *The width of the window. (default: `800`)* \
> **height**: *The height of the window. (default: `600`)* \

### Functions:
#### createWindow()
Initialises OpenGL and supplementary libraries. Creates a window and rendering context.

#### loadConfig()
Binds a shader program to the current active window's OpenGL Context and loads a render configuration based on values set in the global scope (see: `GlobalsManager`).

#### resize(int width, int height)
Sets the window to the specified `width` and `height`.

Params:
> **width:** *The desired window width.* \
> **height:** *The desired window height.*

#### toggleFullscreen()
Sets the viewport to the size of the monitor / display if it's currently in windowed-mode. If fullscreen is already active, converts the viewport to windowed-mode. When switching out of fullscreen the new frame will be what it was prior to resize. If the size was never specified (*e.g. because the application was launched with `GlobalsManager::setLaunchInFullscreen(true)`*) then the window will take up the full size of the monitor / display.

#### open()
Opens the active window.

#### close()
Closes the active window.

#### createCursor(int sizeX, int sizeY, int hotX, int hotY, std::string filepath)
Builds a unique cursor for the window from an image loaded in by the user.

Params:
> **sizeX:** *Cursor image width.* \
> **sizeY:** *Cursor image height.* \
> **hotX:** *The x-axis cursor hotspot.* \
> **hotY:** *The y-axis cursor hotspot.* \
> **filepath:** *The relative path to the desired cursor image. Ideally the image should be of 32x32 resolution.* 

#### snapCursor(int moveX, int moveY)
Programatically move the cursor to the specified pixel location.

Params:
> **moveX:** *The x-axis pixel coordinate from the bottom left corner of the display (Not the window).*
> **moveY:** *The y-axis pixel coordinate from the bottom left corner of the display (Not the window).*

#### setBackgroundColor(glm::vec3 color)
Sets the window's background color which is black by default. Can be called without reloading the window.

Params:
> **color:** *The desired background color in RGB.*

#### presentNextFrame()
Bring the back buffer to the front (into user view) and moves the front buffer to the back. \
Clears the back buffer's depth and color bits so that they can be given new values for the next draw.

#### monitorPixelOccupants()
Enables picking of the window's pixels for objects which have been rendered to the screen following a call to `MeshManager::drawMesh(...)`. The ID's of objects with items with `MeshManager::Mesh::isClickable` set to `true` now become searchable at their pixel coordinates via a call to `CameraManager::getPixelOccupant(...)`.

### Members:
> **isOpen:** *Whether or not the active window is open. See also: `GlobalsManager::getContextWindowOpen()`. (type: `bool`, default: `false`)* \

## WindowManager::EventManager
A class for tracking, storing and managing window events as well as their values.

### Functions:
#### eventsInit()
Locates the programs active window and loads the following event-handler callbacks into it's context's event loop: 
- keydown 
- mousedown 
- mousemove 
- scroll

#### monitorEvents()
Polls GLFW for the head of the active window's event queue and then updates the values of the event managers members.

#### getLatestKey(int &outCode, int &outScan)
Retrieves the most recently recorded key change, inclusive of key-release which holds a value of `0`.

params:
> **outCode:** *A code number which represents the most recently pressed key.* \
> **outScan:** *Similar to the `outCode`, but may be system specific. Should contain a value in the event that `outCode` does not.*

#### getLatestMouseMove(int &outX, int &outY)
Retrieves the most recently recorded change in mouse / cursor position.

params:
> **outX:** *The x-axis position of the cursor.* \
> **outY:** *The y-axis position of the cursor.*

#### getLatestClick(int &out)
Retrieves the most recently recorded change in the mouse button state, inclusive of button-release which holds a value of `0`.

params:
> **out:** *The most recent click behaviour.*

#### getLatestScroll(int &out)
Retrieves the most recently recorded change in the scrollwheel state.

params:
> **out:** *The most recent scroll behaviour.*

### Members:
> **eventQueue:** *All meaningful changes that occured in event state since the last call to `monitorEvents()`. (type: `std::vector<WindowManager::EventManager::Event>`)* \
> **EventType:** *Different event varieties. (type: `enum`)* 
>	- **KEY_PRESS:** *A keyboard button, pressed or released.* 
>	- **CLICK:** *A mouse button, pressed or released.* 
>	- **MOUSE_MOVE:** *The cursor position on either the x or y axis (or both).*  
>   - **SCROLL:** *The scroll / mouse wheel, up, down or neutral.*
> **Event:** *Properties used to quantify and measure event statuses (type: `struct`)* 
>   - **type:** *What event variant this struct represents (type: `EventType`)*
>   - **key:** *Holds the keyboard state.* (type: `int`)
>   - **keyVariant:** *Holds the alternate keyboard state which may be system specific.* (type: `int`)
>   - **click:** *Holds the mousebutton state.* (type: `int`)
>   - **scroll:** *Holds the scrollwheel state.* (type: `int`)
>   - **mousePositionX:** *Holds the cursor x-axis position.* (type: `int`)
>   - **mousePositionY:** *Holds the cursor x-axis position.* (type: `int`)

## WindowManager::Time
Interface for managing and monitoring internal engine time.

### Functions:
#### monitorFPS()
Calculates the number of frame presentations occuring per second. \
Access the result(s) via `WindowManager::Time::framesPerSecond`.

#### monitorTimeDelta()
Calculate the time taken in milliseconds to draw and present a single frame. \
Access the result(s) via `WindowManager::Time::timeDelta`.

#### monitorElapsedTime()
Begin active monitoring of the total number of seconds passed since the time of calling. \
Access the result(s) via `WindowManager::Time::elapsedTime`.

### Members:
> **framesPerSecond:** *Current number of frames being drawn per second. (type: `int`)* \
> **timeDelta:** *The duration of time taken in milliseconds to draw a single frame. (type: `float`)* 
> **elapsedTime:** *The amount of seconds passed since `WindowManager::Time::monitorElapsedTime()` was first called. (type: `float`)*

## Shader:
A class for the lazarus default shader program which, simply maps vertex positions to their coordinates and draws the fragments; following the lambertian lighting model.

### Constructor:
#### Shader()
Default-initialises this classes members.

### Functions:
#### compileShaders(std::string fragmentShader, std::string vertexShader)
Invokes the parsing, compiling, attatching and linking of the specified shaders. If none are specified, `LAZARUS_DEFAULT_VERT_SHADER` and `LAZARUS_DEFAULT_FRAG_SHADER` are used. \
Returns the ID of the shader program which can then be passed to the various constructors which need it.

Params:
> **vertexShader:** *The relative path to a glsl vertex shader program. (optional)* \
> **fragmentShader:** *The relative path to a glsl fragment shader program. (optional)* \

*For notes on shader guidelines, layout and default variables see: [Using your own shaders](#using-your-own-shaders) in the examples.*

#### setActiveShader(int program)
Set the shader id in state that should be used to render subsequent draw calls. 

Params:
> **program:** *The ID of a shader program returned from `Shader::compileShaders()`*

#### uploadUniform(std::string identifier, void *data)
Uploads a value to the to the most recently activated shader program. If no uniform by the name of `identifier` is present, the execution state will be set to `LAZARUS_SHADER_ERROR`. 

Params:
> **identifier:** *The uniform variable's name within the shader program.* \
> **data:** *The value to be uploaded to the GPU. Supported types are: `float`, `int`, `unsigned int`, `vec2`, `vec3` and `vec4`.*

## FileReader:
A utility class for locating files and reading their contents.

### Constructor:
#### FileReader()
Default-initialises this classes members.

### Functions:
#### std::string relativePathToAbsolute(std::string filepath, std::string &out)
Finds the absolute path (from root) to the `filepath` and returns it as `std::string`.

Params:
> **filepath:** The relative path to the file you would like to find the absolute path of.
> **out:** Out parameter which stores the result

#### loadText(std::string filepath, std::string &out)
Reads a file who's contents are expressed in utf-8 ascii and stores its contents in the out buffer.

Params:
> **filepath:** The relative path to the file you would like to read from.
> **out:** Out parameter to store the file's contents.

#### readFromImage(FileReader::Image &out, const char *filepath, const unsigned char *raw, int size)
Reads and parses the contents of an image located at `filepath` or directly from memory when passed to `raw`. Supported filetypes are .png, .jpg, .tga, .pic; view `stb_image` documentation for the full list. Images must be formatted into 8-bit per channel RGBA.

Params:
> **out:** An out parameter to store the resultant image.
> **filepath:** The relative path to the file you would like to read from. (optional, default `NULL`)
> **raw:** Raw image bytes to be loaded from memory (optional, default: `NULL`)
> **size:** The bytesize of `raw` if specified. (optional, default: `0`)

### Members:
> **Image:** *The properties of an image returned from stb_image. (type: `struct`)* 
>	- **width:** *The images pixel-width.. (type: `int`)* 
>	- **height:** *The images pixel-height. (type: `int`)* 
>	- **pixelData:** *The actual image data / texels tightly packed in RGBA order. (type: `unsigned char *`)* 

## Transform:
A class built to handle transformations of different world assets such as mesh, cameras and lights.

### Functions:
#### translateMeshAsset(MeshManager::Mesh &mesh, float x, float y, float z, int matrixId)
Applies a translation transformation (movement) to a mesh asset along the x, y and z axis from an offset of 0.0. \
Updates the `locationX`, `locationY` and `locationZ` properties of a `MeshManager::Mesh` object in real time. 

Params:
> **mesh:** *The mesh asset to be acted upon.* \
> **x:** *A floating point number used to increment / decrement the x-axis locative value of the mesh.* \
> **y:** *A floating point number used to increment / decrement the y-axis locative value of the mesh.* \
> **z:** *A floating point number used to increment / decrement the z-axis locative value of the mesh.* \
> **matrixId:** *The index position / map key of the `MeshManager::Mesh::Instance` to be acted upon. (default: `0`)*

#### rotateMeshAsset(MeshManager::Mesh &mesh, float x, float y, float z, int matrixId)
Applies a rotation transformation to a mesh asset on it's x, y and z axis from an offset of 0.0. \
This rotation affects the yaw, pitch and roll of the mesh. Not to be confused with an orbital rotation. 

Params:
> **mesh:** *The mesh asset to be acted upon.* \
> **x:** *A floating point number used to increment / decrement the x-axis (yaw) rotational value of the mesh.* \
> **y:** *A floating point number used to increment / decrement the y-axis (pitch) rotational value of the mesh.* \
> **z:** *A floating point number used to increment / decrement the z-axis (roll) rotational value of the mesh.* \
> **matrixId:** *The index position / map key of the `MeshManager::Mesh::Instance` to be acted upon. (default: `0`)*

#### scaleMeshAsset(MeshManager::Mesh &mesh, float x, float y, float z, int matrixId)
Applies a scaling transformation to a mesh asset on it's x, y, and z axis from and offset of 1.0. \
Will update the value returned by `GlobalsManager::getExecutionStatus()` to `LAZARUS_INVALID_DIMENSIONS` if any of the values recieved are below `0.0`.

Params:
> **mesh:** *The mesh asset to be acted upon.* \
> **x:** *A floating point number used to increment / decrement the x-axis size value of the mesh.* \
> **y:** *A floating point number used to increment / decrement the y-axis size value of the mesh.* \
> **z:** *A floating point number used to increment / decrement the z-axis size value of the mesh. \
> **matrixId:** *The index position / map key of the `MeshManager::Mesh::Instance` to be acted upon. (default: `0`)*

#### translateCameraAsset(CameraManager::Camera &camera, float x, float y, float z, float velocity)
Applies a translation transformation (movement) to a camera asset along the x, y and z axis from an offset of 0.0. \
Updates the `locationX`, `locationY` and `locationZ` properties of a `CameraManager::Camera` object in real time. 

Params:
> **camera:** *The camera asset to be acted upon.* \
> **x:** *A floating point number used to increment / decrement the x-axis locative value of the camera.* \
> **y:** *A floating point number used to increment / decrement the y-axis locative value of the camera.* \
> **z:** *A floating point number used to increment / decrement the z-axis locative value of the camera.* \
> **velocity:** *The speed at which the camera should translate through space per update. (default: `0.1`)*

#### rotateCameraAsset(CameraManager::Camera &camera, float x, float y, float z)
Applies a rotation transformation to a camera asset on it's x, y and z axis from an offset of 0.0. \
This rotation affects the yaw, pitch and roll of the camera. Not to be confused with an orbital rotation. \
Will update the value returned by `GlobalsManager::getExecutionStatus()` to `LAZARUS_INVALID_RADIANS` if any of the values recieved are below `-360.0` or above `+360.0`.

Params:
> **camera:** *The camera asset to be acted upon.* \
> **x:** *A floating point number used to increment / decrement the x-axis locative value of the camera.* \
> **y:** *A floating point number used to increment / decrement the y-axis locative value of the camera.* \
> **z:** *A floating point number used to increment / decrement the z-axis locative value of the camera.*

#### orbitCameraAsset(CameraManager::Camera &camera, float azimuth, float elevation, float radius, float tarX, float tarY, float tarZ)
Apply arcball / orbital camera transformations to a camera asset; i.e. unit-sphere translation and rotation around a target.

Params:
> **camera:** *The camera asset to be acted upon.* \
> **azimuth:** *Horizontal degrees of rotation around the origin.* \
> **elevation:** *Vertical degreees of rotation around the origin.* \
> **radius:** *Distance from the target / radius of the unit-sphere.* \
> **tarX:** *The x-axis worldspace coordinate of the target location. (default: `0.0`)* \
> **tarY:** *The y-axis worldspace coordinate of the target location. (default: `0.0`)* \
> **tarZ:** *The z-axis worldspace coordinate of the target location. (default: `0.0`)*

#### translateLightAsset(LightManager::Light &light, float x, float y, float z)
Applies a translation transformation (movement) to a light asset along the x, y and z axis from an offset of 0.0. \
Updates the `locationX`, `locationY` and `locationZ` properties of a `LightManager::Light` object in real time. 

Params:
> **light:** *The light asset to be acted upon.* \
> **x:** *A floating point number used to increment / decrement the x-axis locative value of the light.* \
> **y:** *A floating point number used to increment / decrement the y-axis locative value of the light.* \
> **z:** *A floating point number used to increment / decrement the z-axis locative value of the light.*

## MeshManager:
A management class for mesh assets and their properties.

### Constructor:
#### MeshManager(GLuint shader, TextureLoader::StorageType textureType)

Params:
> **shader:** *The id of the shader program used to render this mesh. Acquired from the out-parameter of `Shader::compileShaders()`*
> **textureType:** *Which variant of sampler storage should mesh texture's loaded by this instance be written to. (default: `TextureLoader::StorageType::ARRAY`)*

### Functions:
#### create3DAsset(MeshManager::Mesh &out, MeshManager::AssetConfig options)
Finds and loads the contents of an `.obj` or `.glb` file located at `options.meshPath`. \
Initialises a `Mesh`.

Params:
> **out:** *An out parameter where load results are stored.* \
> **options:** *Load settings indicating how the asset should be loaded.*

#### createQuad(MeshManager::Quad &out, MeshManager::QuadConfig options)
Creates a quad (2D plane) to the size of the specified height and width \
with a texture as specified by `options.texturePath`

Params:
> **out:** *An out parameter where load results are stored.* \
> **options:** *Load settings indicating how the asset should be loaded.*

#### createCube(MeshManager::Cube &out, MeshManager::CubeConfig options)
Creates a cube (equal height, width and depth) of size `options.scale`. Note that without specification of a relative path to a texture asset, this function will assume the cube is to be used for a skybox which; is likely to cause problems in your program without manually setting the required texture data for the cubemap texture.

Params:
> **out:** *An out parameter where load results are stored.* \
> **options:** *Load settings indicating how the asset should be loaded.*

#### loadMesh(MeshManager::Mesh &meshIn)
Loads a mesh object's buffer data into their correct GPU uniform positions located inside the shader program that was referenced in the class constructor.

Params:
> **meshIn:** *The mesh object who's buffer data you wish to pass into the shader program.*

#### drawMesh(MeshManager::Mesh &meshIn)
Draws the mesh object contents of the shader program's uniforms onto the render loops back buffer (see: `WindowManager::presetNextFrame()`). \
Be sure to bring the back buffer forward to see the draw result.

> Params: \
> **meshIn:** *The mesh object you wish to draw.*

#### copyMesh(MeshManager::Mesh &dest, MeshManager::Mesh src)
Creates a duplicate of `src` at the location of `dest` with updated unique ID for the asset and all its child instances.

> Params: \
> **dest:** *The pre-allocated destination where the copy should be stored.* \
> **src:** *The asset to be copied.*

#### setDiscardFragments(MeshManager::Mesh &meshIn, bool shouldDiscard)
Toggle for removing the areas of a face prior to rendering where the meshes texture's alpha value is zero. Used for rendering sprites.

> Params: \
> **meshIn:** *The mesh object you wish to draw.* \
> **shouldDiscard:** *The desired value for the option (T/F).* 

### clearMeshStorage()
Flushes out the internal state(s) of the manager, including it's list of children; freeing ID's of assets and textures for future use and invalidating any asset handles created prior to calling this function.

### Members:
> **Mesh:** *A collection of properties which make up a mesh entity. (type: `struct`)* 
>   - **id:** *The meshes serialised ID. Unique only to the manager instance which created it. (type: `int`)*
>   - **type:** *Which type of asset this mesh is. (type: `MeshManager::MeshType`)*
>   - **materials:** *The materials used by this mesh. (type: `std::vector<MeshManager::Material>`)*
>	- **numOfFaces:** *The number of faces that make up the mesh. (type: `int`)* 
>	- **numOfVertices:** *The number of vertices that make up the mesh. (type: `int`)* 
>	- **meshFilepath:** *The absolute path (from system root) to the wavefront file containing this mesh's vertex data. (type: `std::string`)*
>	- **materialFilepath:** *The absolute path (from system root) to the wavefront file containing this mesh's material data. (type: `std::string*`)*
>   - **instances:** *A map containing information used for applying transformations to a mesh copy. (type: `std::map<int, MeshManager::Mesh::Instance>`)*

> **Mesh::Instance** *A collection of properties which make up a GPU copy of a mesh entity. (type: `struct`)*
>	- **position:** *Where the instance is positioned in world space. (type: `glm::vec3`)*
>	- **direction:** *The instance's forward-vector. Where the instance's local coordinate system's z+ is in relation to world space. (type: `glm::vec3`)*
>	- **scale:** *The size of the instance. (type: `glm::vec3`)*
>   - **modelMatrix:** *A 4x4 matrice used to perform transformations on the instance. You will need this if you're intending you write your own transformations instead of using the `Transform` class. (type: `glm::mat4`)*
>   - **isClickable:** *Whether or not this assets id can be looked up via pixel coordinate when it is occupying screenspace. (type: `bool`)*
>   - **isVisible:** *Whether or not this instance should be rendered to the screen during fragment processing or discarded. (type: `bool`)*

> **MeshType:** *Different varieties of meshes (type: `enum`)*
>   - **LOADED_GLB:** *A mesh which has been loaded from a glb file.*
>   - **LOADED_WAVEFRONT:** *A mesh which has been loaded from a wavefront file.*
>   - **PLANE:** *A quad, I.e. Two-dimensional, four sides.*
>   - **CUBE:** *A cuboid / rectangular prism.*

> **AssetConfig:** *Creation function input-settings. (type: `struct`)*
>   - **name:** *What to call this asset. (type: `std::string`)*
>   - **meshPath:** *The relative path to the `.obj` or `.glb` asset to be loaded. (type: `std::string`)*
>   - **materialPath:** *The relative path to the asset's material-file (`.mtl`) if `meshPath` is directed toward an `.obj` file. Leave blank otherwise. (type: `std::string`)*
>   - **selectable:** *Whether to assign a stencil ID to this asset for cursor-picking while visible in-frame. (type: `bool`)*
>   - **instanceCount:** *The number of copies of this mesh to be produced. (type: `int`, default: `1`)*

> **QuadConfig:** *Creation function input-settings. (type: `struct`)*
>   - **name:** *What to call this asset. (type: `std::string`, default: `"QUAD_" + n`)*
>   - **texturePath:** *The relative path to a texture image used to render to the quad's surface. (type: `std::string`)*
>   - **width:** *The quad's horizontal span. (type: `float`)*
>   - **height:** *The quad's vertical span. (type: `float`)*
>   - **selectable:** *Whether to assign a stencil ID to this asset for cursor-picking while visible in-frame. (type: `bool`)*
>   - **uvXL:** *The left-most extremity of the x-axis UV / ST coordinates. (type: `float`, optional)*
>   - **uvXR:** *The right-most extremity of the x-axis UV / ST coordinates. (type: `float`, optional)*
>   - **uvYU:** *The upper-most extremity of the y-axis UV / ST coordinates. (type: `float`, optional)*
>   - **uvYD:** *The lower-most extremity of the y-axis UV / ST coordinates. (type: `float`, optional)*
>   - **instanceCount:** *The number of copies of this mesh to be produced. (type: `int`, default: `1`)*

> **CubeConfig:** *Creation function input-settings. (type: `struct`)*
>   - **name:** *What to call this asset. (type: `std::string`, default: `"CUBE_" + n`)*
>   - **texturePath:** *The relative path to a texture image used to render to the quad's surface. (type: `std::string`)*
>   - **selectable:** *Whether to assign a stencil ID to this asset for cursor-picking while visible in-frame. (type: `bool`)*
>   - **scale:** *The multiplier by which to increase the size of the cube by. (type: `float`, default: `1.0f`)*
>   - **instanceCount:** *The number of copies of this mesh to be produced. (type: `int`, default: `1`)*

> **Material:** *The properties which constitute the material that is rendered to a surface. (type: `struct`)*
>   - **id:** *A serialised ID unique to the material's parent `MeshManager::Mesh`. (type: `int`)*
>   - **type:** *Which type of material this is. (type: `MeshManager::MaterialType`)*
>   - **texture:** *If `type` is `MeshManager::MaterialType::IMAGE_TEXTURE`, this struct contains the materials texture data, i.e. width, height and raw bytes. (type: `FileLoader::Image`)*
>   - **diffuse:** *If `type` is `MeshManager::MaterialType::BASE_COLOR`, this struct contains the materials color value.*
>   - **discardsAlphaZero:** *If the contents of `texture.pixelData` contain an `RGBA` value with an alpha of zero, discard the fragment.*

> **MaterialType:** *Different varieties of materials (type: `enum`)*
>   - **IMAGE_TEXTURE:** *Faces using this material are the rendered using a sample from an image.*
>   - **BASE_COLOR:** *Faces using this material are rendered using their base diffuse coloring.*

## CameraManager:
### Constructor:
Default initialises this class's members.

#### CameraManager(GLuint shader)

Params:
> **shader:** *The id of the shader program used to render this camera. Acquired from the out-parameter of `Shader::compileShaders()`*

### Functions: 
#### createPerspectiveCam(CameraManager::Camera &out, CameraManager::CameraConfig options)
Creates a new `Camera` object with a perspective projection matrix located at the scenes origin (x: 0.0, y: 0.0, z: 0.0) which looks directly down the +X axis.

Params:
> **out:** *An out parameter where load results are stored.* \
> **options:** *Load settings indicating how the asset should be loaded.*

### Functions: 
#### createOrthoCam(CameraManager::Camera &out, CameraManager::CameraConfig options)
Creates a new instance of a `Camera`, with an orthographic projection matrix. 

Params:
> **out:** *An out parameter where load results are stored.* \
> **options:** *Load settings indicating how the asset should be loaded.*

#### loadCamera(CameraManager::Camera cameraData)
Passes the camera's projection matrix and view matrix into the shader program's corresponding uniform locations.

Params:
> **cameraData:** *The camera asset you would like to render.*

#### getPixelOccupant(int positionX, int positionY, int &out)
Retrieves the ID of a pixel occupant in view which has `MeshManager::Mesh::isClickable` set to `true`.

> **positionX:** *The x-axis pixel coordinate from the bottom left corner of the display (Not the window).*
> **positionY:** *The y-axis pixel coordinate from the bottom left corner of the display (Not the window).*
> **out:** *Where to store the occupant result.*

### Members:
> **Camera:** *A collection of properties which make up a camera entity. (type: `struct`)* 
>	- **id:** *This camera's unique id. (type: `int`)* 
>	- **position:** *The x, y, z location of the camera. (type: `glm::vec3`)*
>	- **direction:** *Which direction the camera is facing. (type: `glm::vec3`)*
>	- **upVector:** *Which way is considered up. (type: `glm::vec3`)*
>   - **config:** *Object settings. (type: `CameraManager::CameraConfig`)*
>	- **viewMatrix:** *The view matrix to be passed into the shader program at the uniform location of `viewLocation`. (type: `glm::mat4`)*
>	- **projectionMatrix:** *The projection matrix to be passed into the shader program at the uniform location of `projectionLocation`. (type: `glm::mat4`)*

> **CameraConfig:** *Creation function input-settings. (type: `struct`)*
>   - **name:** *What to call this asset. (type: `std::string`, default: `"CAMERA_" + Camera.id`)*
>   - **aspectRatioX:** *The x-axis aspect ratio / width of the viewport. (Default: `LAZARUS_PRIMARY_DISPLAY_WIDTH`)* \
>   - **aspectRatioY:** *The y-axis aspect ratio / height of the viewport. (Default: `LAZARUS_PRIMARY_DISPLAY_HEIGHT`)* 

## LightManager:
A management class for light assets and their properties. 

### Constructor:
#### LightManager(int shader)

Params:
> **shader:** *The id of the shader program used to render this light. Acquired from the out-parameter of `Shader::compileShaders()`*

### Functions: 
#### createLightSource(LightManager::Light &out, LightManager::LightConfig options)
Creates a new instance of an `Light`, initialises the values of its properties.

Params:
> **out:** *An out parameter where load results are stored.* \
> **options:** *Load settings indicating how the asset should be loaded.*

#### loadLightSource(LightManager::Light &lightData, int shader)
Passes the light object's locative (x,y,z) values into the vertex shader and its' colour (r,g,b) values into the fragment shader.

Params:
> **lightData:** *A reference to the light asset you would like to render.*
> **shader:** *Alternative shader destination to upload light properties rather than that which this `LightManager` was instantiated with. Note that usage of this option has performance expenses. (optional)*

### Members:
> **Light:** *A collection of properties which make up a light entity. (type: `struct`)*
>	- **id:** *This light's unique id. (type: `int`)*
>   - **config:** *Object settings. (type: `LightManager::LightConfig`)*

> **LightType:** *Diffrent varieties of lights. (type: `enum`)*
>   - **DIRECTIONAL:** *Luminence from a far away point such as the sun. Is treated as constant accross the surface of an object.*
>   - **POINT:** *Light which shines at all angles from a point in space with range-based attenuation, like a lightbulb.*

> **LightConfig:** *Creation function input-settings. (type: `struct`)*
>   - **name:** *What to call this asset. (type: `std::string`, default: `"LIGHT_" + n`)*
>   - **direction:** *The x, y, z direction of the light. Ignored when `type` is set to ``LightType::POINT`. (type: `glm::vec3`, default: `(1.0, 0.0, 0.0)`)*
>   - **position:** *The x, y, z location of the light. Ignored when `type` is set to ``LightType::DIRECTIONAL`. (type: `glm::vec3`, default: `(0.0, 0.0, 0.0)`)*
>   - **color:** *The light's shade of RGB color. (type: `glm::vec3`, default: `(1.0, 1.0, 1.0)`)*
>   - **brightness:** *The light's luminescent intensity. (type: `float`, default: `1.0f`)*
>   - **type:** *Which variant of light this is. (type: `LightType`, default: `LightType::DIRECTIONAL`)*

## AudioManager:
A management class using an `FMOD` backend for loading audio, as well as handling audio locations and listeners. 

### Constructor:
#### AudioManager()

### Functions:
#### initialise()
Initialises `FMOD` core and instantiates a new system for interfacing with up to 512 unique audio channels. 

#### createAudio(AudioManager::Audio &out, AudioManager::AudioConfig options)
Creates a new instance of `AudioManager::Audio`, initialises the values of its properties and returns it.

Params:
> **out:** *An out parameter where load results are stored.* \
> **options:** *Load settings indicating how the asset should be loaded.*

#### loadAudio(AudioManager::Audio audioIn)
Loads a `AudioManager::Audio` object and prepares it for playback with `FMOD`.

Params:
> **audioIn:** *The audio object you want to load.*

#### playAudio(AudioManager::Audio audioIn)
Plays a `AudioManager::Audio` object which has been previously loaded with `AudioManager::loadAudio()`.

Params:
> **audioIn:** *The audio object you want to start playing.*

#### pauseAudio(AudioManager::Audio audioIn)
Pauses an `AudioManager::Audio` object which has been previously played with `AudioManager::playAudio()`.

Params:
> **audioIn:** *The audio object you want to pause / stop playing.*

#### setPlaybackCursor(AudioManager::Audio &audioIn, int milliseconds)
Set the playback position of the target audio in milliseconds.

Params:
> **audioIn** *A reference to the target audio sample* \
> **milliseconds** *Target number of elapsed milliseconds since the audios beginning (0) to playback from.*

#### updateSourceLocation(AudioManager::Audio audioIn, glm::vec3 location)
Updates the location in 3D of a `AudioManager::Audio` source; using `FMOD` to calculate the sound's doppler, relative to the listener's current positioning (*see*: `AudioManager::listenerLocationX`, `AudioManager::listenerLocationY` and `AudioManager::listenerLocationZ`).

Params:
> **audioIn**: *The audio sample to be updated.* \
> **location**: *The desired audio source location in worldspace.* \

#### updateListenerLocation(glm::vec3 location)
Updates the location in 3D space of the audio's listener; using `FMOD` to calculate the doppler level, relative to the `AudioManager::Audio` current positioning. 

Params:
> **location**: *The desired audio listener location on in worldspace.* 

### Members:
> **Audio:** *A collection of properties which make up a sound asset. (type: `struct`)*
>	- **id:** *This audio's unique id. (type: `int`)*
>	- **sourceLocation:** *The audio's position in world space. (type: `glm::vec3`)*
>   - **isPaused:** *Signifies whether the sound is currently playing or not (type: `bool`)*
>   - **audioIndex:** *Used internally to identify the audio object's location in the audio vector. (type: `int`)* 
>   - **config:** *Object settings. (type: `AudioManager::AudioConfig`)*

> **AudioConfig:** *Creation function input-settings. (type: `struct`)*
>   - **name:** *What to call this asset. (type: `std::string`, default: `"SAMPLE_" + n`)*
>   - **filepath:** *The relative path to the sound file. (type: `std::string`)*
>   - **is3D:** *Whether the sound should be treated as spatial. (type: `bool`, default: `false`)*
>   - **loopCount:** *How many times the sample should loop. Use -1 to loop infinitely. (type: `int`, default: `0`)*

## TextManager:
A management class for rendering and laying out text on the screen.

### Constructor:
#### TextManager(GLuint shader)

Params:
> **shader:** *The id of the shader program used to render this camera. Acquired from the out-parameter of `Shader::compileShaders()`*

### Functions:
#### extendFontStack(int &fontId, std::string filepath, int ptSize)
Loads a new TrueType font into a texture atlas stored in VRAM. \

Returns the font index used for font selection when loading text.

params:
> **fontId:** *An out-parameter to store the index of the new font.*
> **filepath:** *The relative path to the TrueType `.ttf` font file.* \
> **ptSize:** *The desired character pt size. (default: `12`)*

#### createText(TextManager::Text &out, TextManager::TextConfig options)
Loads the desired text using glyphs from the selected font. Sets the text's colour, position on the screen and letterspacing. It's worth noting \
here that a space (`' '`) is equal to `letterSpacing * 8`.

Inserted as an entry in the layout.

Params:
> **out:** *An out parameter where load results are stored.* \
> **options:** *Load settings indicating how the asset should be loaded.*

#### TextManager::loadText(TextManager::Text textIn)
Updates the layout with any changes from `textIn` and prepares values in memory for drawing.

Params:
> **textIn:** *A utf-8 encoded string to be drawn orthographically to the screen.*

#### drawText(TextManager::Text textIn)
Draws text that has been loaded into the layout at the location of `text.layoutIndex` onto the screen.

Params:
> **textIn:** *A text object previously loaded by `TextManager::loadText`.*

### Members:
> **Text:** *A collection of properties which make up a string of text.*
>   - **layoutIndex:** *An identifier for performing lookup on strings loaded into the layout container. (type: `int`)*
>   - **config:** *Object settings. (type: `TextManager::TextConfig`)*

> **TextConfig:** *Creation function input-settings. (type: `struct`)*
>   - **fontIndex:** *The unsigned integer value returned in the out-parameter of `TextManager::extendFontStack`, i.e. a font previously loaded into memory. (type: `int`)*
>   - **letterSpacing:** *The number of pixels to leave between each rendered glyph / character. (type: `int`, default: `1`)*
>   - **location:** *The position of the text (in pixels) measured from the bottom-left corner of the window. (type: `glm::vec2`, default: `(0.0, 0.0)`)*
>   - **targetString:** *The text you wish to render on-screen. (type: `std::string`)*
>   - **color:** *The RGB color of the `targetString` when rendered. (type: `glm::vec3`, default: `(0.0, 0.0, 0.0)`)*

## WorldFX:
A management class for handling environmental effects such as skyboxes and soon; particles.

### Constructor:
#### WorldFX(GLuint shader)

Params:
> **shader:** *The id of the shader program used to render the subject's of this classes tooling. Acquired from the out-parameter of `Shader::compileShaders()`*

### Functions:

#### createSkyBox(WorldFX::SkyBox &out, std::string rightPath, std::string leftPath, std::string downPath, std::string upPath, std::string frontPath, std::string backPath)
Creates and initialises a skybox object using textures loaded from the functions path params.

Params:
> **out:** *The out-parameter to store the value of the newly generated skybox.* \
> **rightPath** *The relative path to the image used for the cube's `+X axis` face.* \
> **leftPath** *The relative path to the image used for the cube's `-X axis` face.* \
> **downPath** *The relative path to the image used for the cube's `-Y axis` face.* \
> **upPath** *The relative path to the image used for the cube's `+Y axis` face.* \
> **frontPath** *The relative path to the image used for the cube's `+Z axis` face.* \
> **backPath** *The relative path to the image used for the cube's `-Z axis` face.*

#### drawSkyBox(WorldFX::SkyBox sky, CameraManager::Camera camera)
Draws texels from the specified skybox into the scene at the location of the camera's direction vector.

Params:
> **sky** The target skybox object to be drawn. \
> **camera** The camera object used to determine the target uvw coordinate of the texel to the sampled from the bound cubemap image texture.

#### createFog(WorldFX::Fog &out, float minDistance, float maxDistance, float thickness, glm::vec3 color, glm::vec3 position)
Creates and a initialises a cloud of fog into the scene with radial visibility bounds. I.e. spherical area of non-fog.

Params: 
> **out:** *The out-parameter to store the value of the newly generated fog object.*
> **minDistance:** *The radius from xyz wherein fog's attenuation / opacity begins taking effect.*
> **maxDistance:** *The radius from xyz wherein fog reaches full opacity.*
> **thickness:** *The fog's density.*
> **color:** *The fog's RGB color values.*
> **position:** *The worldspace coordinates of the viewpoint. (default: `{0.0, 0.0, 0.0} (origin)`).*

#### loadFog(WorldFx::Fog &fogIn, int shader)
Load fog properties to the GPU to be used in subsequent draw calls.

Params:
> **fogIn:** *The fog to be uploaded.* \
> **shader:** *The destination shader program which fog properties should be uploaded to instead of that which was specified during instantiation. Note that usage of this function has performance expenses. (optional)*

### Members:
> **SkyBox**: *A collection of properties which make up a skybox object. (type: `struct`.)*
>   - **paths:** *The absolute filepaths to the 6 image textures used to construct the skymap. (type: `std::vector<std::string>`)*
>   - **cubeMap:** *The image data for each of the cubes 6 faces. (type: `std::vector<FileReader::Image>`)*
>   - **cube:** *The skybox's mesh. (type: `MeshManager::Mesh`)*
> **Fog** *A collection of properties which quanitify world fog and it's visibility. (type: `struct`).* 
>   - **color:** *The fog's RGB color values. (type: `glm::vec3`).*
>   - **viewpoint:** *The epicenter of the fog's sphere of visibility in worldspace. Defines where the fog can be seen from. (type: `glm::vec3`).*
>   - **minDistance:** *From the viewpoint, at which distance the fog's attenuation / opacity begins taking effect. (type: `float`).*
>   - **maxDistance:** *From the viewpoint, at which distance the fog reaches full opacity / coverage. (type: `float`).*
>   - **density:** *The fog's thickness. Note that this only acts as a modifier of the other visibility values. The fog will still eventually reach full opacity. (type: `float`).*

------------------------------------------------------------------

# Resources:
1. G++ GNU compiler. \
**make sure you are installing version 11.3.0** \
Downloads: \
mac (intel): http://cs.millersville.edu/~gzoppetti/InstallingGccMac.html \
mac (silicone / M1): https://trinhminhchien.com/install-gcc-g-on-macos-monterey-apple-m1/ \
windows: https://www3.cs.stonybrook.edu/~alee/g++/g++.html 

2. OpenGL resources: \
Official Documentation: https://www.opengl.org/Documentation/Documentation.html \
Downloads: https://developer.nvidia.com/opengl-driver \
Wiki: https://www.khronos.org/opengl/wiki/ \
Tutorials: \
https://learnopengl.com/ \
https://docs.gl/es2/glBufferData 

3. GLFW resources: \
Official Documentation: https://www.glfw.org/documentation.html \
Downloads: https://www.glfw.org/download.html 

4. GLEW resources: \
Downloads / Official Documentation: https://glew.sourceforge.net/

5. GLM resources: \
Official Documentation: https://glm.g-truc.net/0.9.9/api/index.html \
Download: https://sourceforge.net/projects/glm.mirror/

6. nothings stb: \
stb_image source (RAW): https://raw.githubusercontent.com/nothings/stb/master/stb_image.h \
stb_image_resize source (RAW): https://raw.githubusercontent.com/nothings/obbg/refs/heads/master/stb/stb_image_resize.h

7. FMOD: \
Audio engine documentation: https://www.fmod.com/docs/2.03/api/welcome.html \
Downloads: https://www.fmod.com/download#fmodengine \
Sign up: https://www.fmod.com/profile/register 

8. FreeType2: \
Official Documentation: https://freetype.org/index.html
Download: https://sourceforge.net/projects/freetype/files/freetype2/

9. Valgrind resources: \
Official Documentation: https://valgrind.org/docs/ \
Downloads: https://valgrind.org/downloads/ \
Install instructions: https://stackoverflow.com/questions/24935217/how-to-install-valgrind-properly

10. gdb resources: \
Official Documentation: https://ftp.gnu.org/old-gnu/Manuals/gdb/html_chapter/gdb_toc.html \
Download / installation: https://www.gdbtutorial.com/tutorial/how-install-gdb

11. MSVC resources: \
Official Documentation: https://learn.microsoft.com/en-us/cpp/build/reference/compiler-options?view=msvc-170

12. C++ resources: \
Core guidelines: https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-functions

13. Wavefront file format: \
Appendix: https://www.loc.gov/preservation/digital/formats/fdd/fdd000507.shtml

14. GLTF/glb file format: \
Official documentation: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html \
Basics: https://www.khronos.org/files/gltf20-reference-guide.pdf

15. Misc:
RGB decimal colours: https://www.tug.org/pracjourn/2007-4/walden/color.pdf
Unicode UTF-8 characters: https://www.charset.org/utf-8
Free cubemaps: https://www.humus.name/index.php?page=Textures

------------------------------------------------------------------

# Known caveats and limitations:
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
9. For linux systems using wayland graphical sessions, running in windowed mode will throw an error due to window repositioning (the window is centered so it doesn't just appear in a random place). Run the application in fullscreen with `GlobalsManager::setLaunchInFullscreen(true)`.