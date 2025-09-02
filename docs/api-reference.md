# API Reference:
This API reference contains brief descriptions of each of the Lazarus classes; their contructors, functions and members and \
destructors. Private functions and members are currently omitted from this guide.

All of the Lazarus status codes can be found at the bottom of this document.

For a comprehensive usage guide, visit [Lazarus by Example](./lazarus-by-example.md)

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

#### void setExecutionState(int state)
This will eventually be made private but right now it isn't which is why I'm documenting it here. Sets the value of `LAZARUS_EXECUTION_STATUS` which; in an error-free program should always(!) be 0.

*(Note: Seriously, no point in touching this. you're just shooting yourself in the foot).*

params:
>**state:** *The desired execution-status (status code).*

#### int getExecutionState()
Returns the current value of `LAZARUS_EXECUTION_STATUS`. Any errors that occur inside the engine should update this value to be one of any lazarus status code.

#### void setNumberOfActiveLights(int count)
Sets the value of `LAZARUS_LIGHT_COUNT`. Updates the total number of lightsources known to the render context. Don't do this.

params:
>**count:** *The total number of lights.*

#### int getNumberOfActiveLights()
Returns the number of lights known accross all `LightManager` instances.

### Members:
> **StatusCode**: Various execution status codes (*type:* `enum`)
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
#### int createWindow()
Initialises OpenGL and supplementary libraries. Creates a window and rendering context.

#### int loadConfig()
Binds a shader program to the current active window's OpenGL Context and loads a render configuration based on values set in the global scope (see: `GlobalsManager`).

#### int resize(int width, int height)
Sets the window to the specified `width` and `height`.

Params:
> **width:** *The desired window width.* \
> **height:** *The desired window height.*

#### int toggleFullscreen()
Sets the viewport to the size of the monitor / display if it's currently in windowed-mode. If fullscreen is already active, converts the viewport to windowed-mode. When switching out of fullscreen the new frame will be what it was prior to resize. If the size was never specified (*e.g. because the application was launched with `GlobalsManager::setLaunchInFullscreen(true)`*) then the window will take up the full size of the monitor / display.

#### int open()
Opens the active window.

#### int close()
Closes the active window.

#### int createCursor(int sizeX, int sizeY, int hotX, int hotY, std::string filepath)
Builds a unique cursor for the window from an image loaded in by the user.

Params:
> **sizeX:** *Cursor image width.* \
> **sizeY:** *Cursor image height.* \
> **hotX:** *The x-axis cursor hotspot.* \
> **hotY:** *The y-axis cursor hotspot.* \
> **filepath:** *The relative path to the desired cursor image. Ideally the image should be of 32x32 resolution.* 

#### int snapCursor(int moveX, int moveY)
Programatically move the cursor to the specified pixel location.

Params:
> **moveX:** *The x-axis pixel coordinate from the bottom left corner of the display (Not the window).*
> **moveY:** *The y-axis pixel coordinate from the bottom left corner of the display (Not the window).*

#### int setBackgroundColor(glm::vec3 color)
Sets the window's background color which is black by default. Can be called without reloading the window.

Params:
> **color:** *The desired background color in RGB.*

#### int presentNextFrame()
Bring the back buffer to the front (into user view) and moves the front buffer to the back. \
Clears the back buffer's depth and color bits so that they can be given new values for the next draw.

#### int monitorPixelOccupants()
Enables picking of the window's pixels for objects which have been rendered to the screen following a call to `MeshManager::drawMesh(...)`. The ID's of objects with items with `MeshManager::Mesh::isClickable` set to `true` now become searchable at their pixel coordinates via a call to `CameraManager::getPixelOccupant(...)`.

### Members:
> **isOpen:** *Whether or not the active window is open. See also: `GlobalsManager::getContextWindowOpen()`. (type: `bool`, default: `false`)* \

## WindowManager::Events
A class for tracking, storing and managing window events as well as their values.

### Functions:
#### void eventsInit()
Locates the programs active window and loads the following event-handler callbacks into it's context's event loop: 
- keydown 
- mousedown 
- mousemove 
- scroll

#### void monitorEvents()
Polls GLFW for the head of the active window's event queue and then updates the values of the event managers members.

### Members:
> **keyEventString:** *The key currently being pressed, expressed as a string. (type: `std::string`)* \
> **keyEventCode:** *The openGL code of the string currently being pressed. (type: `int`)* \
> **keyEventOsCode:** *The operating-system specific "scancode" of the key currently being pressed. (type `int`)* \
> **mouseEventCode:** *An integer in the range of 0 to 8 expressing which mouse button is currently being clicked / pressed. (type: `int`)* \
> **mousePositionX:** *The current x-axis coordinate of the cursor. (type: `int`)* \
> **mousePositionY:** *The current y-axis coordinate of the cursor. (type: `int`)* \
> **scrollEventCode:** *An integer in the range of -1 to 1 expressing the scrollwheel's y-offset.* 

## WindowManager::Time
Interface for managing and monitoring internal engine time.

### Functions:
#### void monitorFPS()
Calculates the number of frame presentations occuring per second. \
Access the result(s) via `WindowManager::Time::framesPerSecond`.

#### void monitorTimeDelta()
Calculate the time taken in milliseconds to draw and present a single frame. \
Access the result(s) via `WindowManager::Time::timeDelta`.

#### void monitorElapsedTime()
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
#### int compileShaders(std::string fragmentShader, std::string vertexShader)
Invokes the parsing, compiling, attatching and linking of the specified shaders. If none are specified, `LAZARUS_DEFAULT_VERT_SHADER` and `LAZARUS_DEFAULT_FRAG_SHADER` are used. \
Returns the ID of the shader program which can then be passed to the various constructors which need it.

Params:
> **vertexShader:** *The relative path to a glsl vertex shader program. (optional)* \
> **fragmentShader:** *The relative path to a glsl fragment shader program. (optional)* \

*For notes on shader guidelines, layout and default variables see: [Using your own shaders](./lazarus-by-example.md#using-your-own-shaders) in the examples.*

#### void setActiveShader(int program)
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
#### std::string relativePathToAbsolute(std::string filepath)
Finds the absolute path (from root) to the `filepath` and returns it as `std::string`.

Params:
> **filepath**: The relative path to the file you would like to find the absolute path of.

#### const char *readFromText(std::string filepath)
Reads a file who's contents are expressed in ascii. Stores the files contents in a variable and returns it.

Params:
> **filepath**: The relative path to the file you would like to read from.

#### FileReader::Image readFromImage(std::string filename)
Reads and parses the contents of an image file (.png, .jpg, .tga, .pic; view `stb_image` documentation for the full list). Images must be formatted into 8-bit per channel RGBA.
Returns the image data in the form of an `unsigned char*`.

Params:
> **filename**: The relative path to the file you would like to read from.

#### void resizeImagesOnLoad(bool shouldResize)
Updates the `LAZARUS_ENFORCE_IMAGE_SANITY` global flag *(default: `false`)*. Informs the image loader that it should resize all images to the dimensions of `LAZARUS_MAX_IMAGE_WIDTH`x`LAZARUS_MAX_IMAGE_HEIGHT` prior to adding the image to the texture stack.

Params:
>**shouldResize:** *If true use max image height & width global values, otherwise use image raw size.*

#### void setMaxImageSize(int width, int height)
Sets values for the global `LAZARUS_MAX_IMAGE_WIDTH` and `LAZARUS_MAX_IMAGE_HEIGHT` flags.

Params:
>**height:** *The desired height to enforce accross all images, must be higher than 0 when `LAZARUS_ENFORCE_IMAGE_SANITY` is set. Cant be set beyond 2048.* \
>**width:** *The desired width to enforce accross all images, must be higher than 0 when `LAZARUS_ENFORCE_IMAGE_SANITY` is set. Cant be set beyond 2048.*

### Members:
> **Image:** *The properties of an image returned from stb_image. (type: `struct`)* 
>	- **width:** *The images pixel-width.. (type: `int`)* 
>	- **height:** *The images pixel-height. (type: `int`)* 
>	- **pixelData:** *The actual image data / texels tightly packed in RGBA order. (type: `unsigned char *`)* 

## Transform:
A class built to handle transformations of different world assets such as mesh, cameras and lights.

### Functions:
#### void translateMeshAsset(MeshManager::Mesh &mesh, float x, float y, float z)
Applies a translation transformation (movement) to a mesh asset along the x, y and z axis from an offset of 0.0. \
Updates the `locationX`, `locationY` and `locationZ` properties of a `MeshManager::Mesh` object in real time. 

Params:
> **mesh:** *The mesh asset to be acted upon.* \
> **x:** *A floating point number used to increment / decrement the x-axis locative value of the mesh.* \
> **y:** *A floating point number used to increment / decrement the y-axis locative value of the mesh.* \
> **z:** *A floating point number used to increment / decrement the z-axis locative value of the mesh.*

#### void rotateMeshAsset(MeshManager::Mesh &mesh, float x, float y, float z)
Applies a rotation transformation to a mesh asset on it's x, y and z axis from an offset of 0.0. \
This rotation affects the yaw, pitch and roll of the mesh. Not to be confused with an orbital rotation. 

Params:
> **mesh:** *The mesh asset to be acted upon.* \
> **x:** *A floating point number used to increment / decrement the x-axis (yaw) rotational value of the mesh.* \
> **y:** *A floating point number used to increment / decrement the y-axis (pitch) rotational value of the mesh.* \
> **z:** *A floating point number used to increment / decrement the z-axis (roll) rotational value of the mesh.*

#### void scaleMeshAsset(MeshManager::Mesh &mesh, float x, float y, float z)
Applies a scaling transformation to a mesh asset on it's x, y, and z axis from and offset of 1.0. \
Will update the value returned by `GlobalsManager::getExecutionStatus()` to `LAZARUS_INVALID_DIMENSIONS` if any of the values recieved are below `0.0`.

#### void translateCameraAsset(CameraManager::Camera &camera, float x, float y, float z, float velocity)
Applies a translation transformation (movement) to a camera asset along the x, y and z axis from an offset of 0.0. \
Updates the `locationX`, `locationY` and `locationZ` properties of a `CameraManager::Camera` object in real time. 

Params:
> **camera:** *The camera asset to be acted upon.* \
> **x:** *A floating point number used to increment / decrement the x-axis locative value of the camera.* \
> **y:** *A floating point number used to increment / decrement the y-axis locative value of the camera.* \
> **z:** *A floating point number used to increment / decrement the z-axis locative value of the camera.* \
> **velocity:** *The speed at which the camera should translate through space per update. (default: `0.1`)*

#### void rotateCameraAsset(CameraManager::Camera &camera, float x, float y, float z)
Applies a rotation transformation to a camera asset on it's x, y and z axis from an offset of 0.0. \
This rotation affects the yaw, pitch and roll of the camera. Not to be confused with an orbital rotation. \
Will update the value returned by `GlobalsManager::getExecutionStatus()` to `LAZARUS_INVALID_RADIANS` if any of the values recieved are below `-360.0` or above `+360.0`.

Params:
> **camera:** *The camera asset to be acted upon.* \
> **x:** *A floating point number used to increment / decrement the x-axis locative value of the camera.* \
> **y:** *A floating point number used to increment / decrement the y-axis locative value of the camera.* \
> **z:** *A floating point number used to increment / decrement the z-axis locative value of the camera.*

#### void orbitCameraAsset(CameraManager::Camera &camera, float azimuth, float elevation, float radius, float tarX, float tarY, float tarZ)
Apply arcball / orbital camera transformations to a camera asset; i.e. unit-sphere translation and rotation around a target.

Params:
> **camera:** *The camera asset to be acted upon.* \
> **azimuth:** *Horizontal degrees of rotation around the origin.* \
> **elevation:** *Vertical degreees of rotation around the origin.* \
> **radius:** *Distance from the target / radius of the unit-sphere.* \
> **tarX:** *The x-axis worldspace coordinate of the target location. (default: `0.0`)* \
> **tarY:** *The y-axis worldspace coordinate of the target location. (default: `0.0`)* \
> **tarZ:** *The z-axis worldspace coordinate of the target location. (default: `0.0`)*

#### void translateLightAsset(LightManager::Light &light, float x, float y, float z)
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
#### MeshManager(GLuint shader)

Params:
> **shader:** *The id of the shader program used to render this mesh. Acquired from the return value of `Shader::initialiseShader()`*

### Functions:
#### MeshManager::Mesh create3DAsset(std::string meshPath, std::string materialPath, bool selectable)
Finds and reads a the `.obj` or `.glb` file located at `meshPath`. \
Creates a new instance of a `Mesh`, initialises the values of its properties and returns it. 

Returns a new mesh entity.

Params:
> **meshPath:** *The relative path to the wavefront mesh asset you wish to render.* 
> **materialPath:** *The relative path to the wavefront material asset you wish to render. (default: `LAZARUS_TEXTURED_MESH`)*
> **selectable:** *Whether or not this assets id can be looked up via pixel coordinate when it is occupying screenspace. (default: `false`)*

#### MeshManager::Mesh createQuad(float width, float height, std::string texturePath, float uvXL, float uvXR, float uvY, bool selectable)
Creates a quad (2D plane) to the size of the specified height and width. \
Textures loaded into a quad have their fragments discarded where the texture opacity is 0.0 - used for sprites.

Returns a new mesh entity.

Params:
> **width:** *The relative path to the wavefront mesh asset you wish to render.* 
> **height:** *The relative path to the wavefront material asset you wish to render.*
> **texturePath:** *The relative path to the texture image. (optional)*
> **uvXL:** *The normalised x-axis coordinate of the UV's left-side. Used for text rendering. (optional)*
> **uvXR:** *The normalised x-axis coordinate of the UV's right-side. Used for text rendering. (optional)*
> **uvYU:** *The normalised y-axis coordinate of the UV's top edge. Used for text rendering. (optional)*
> **uvYD:** *The normalised y-axis coordinate of the UV's bottom edge. Used for text rendering. (optional)*
> **selectable:** *Whether or not this assets id can be looked up via pixel coordinate when it is occupying screenspace. (optional)*

#### MeshManager::Mesh createCube(float scale, std::string texturePath, bool selectable)
Creates a cube (equal height, width and depth) of size `scale`. Note that without specification of a relative path to a texture asset, this function will assume the cube is to be used for a skybox which; is likely to cause problems in your program without manually setting the required texture data for the cubemap texture.

Returns a new mesh entity.

Params:
> **scale:** *The desired height, width and depth of the cube. Values of 0 or less will set the engines execution state to `LAZARUS_INVALID_DIMENSIONS`.*
> **texturePath:** *The relative path to an image texture asset. (optional)*
> **selectable:** *Whether or not this assets id can be looked up via pixel coordinate when it is occupying screenspace. (optional)*

#### MeshManager::Mesh loadMesh(MeshManager::Mesh &meshIn)
Loads a mesh object's buffer data into their correct GPU uniform positions located inside the shader program that was referenced in the class constructor.

Params:
> **meshIn:** *The mesh object who's buffer data you wish to pass into the shader program.*

#### MeshManager::Mesh drawMesh(MeshManager::Mesh &meshIn)
Draws the mesh object contents of the shader program's uniforms onto the render loops back buffer (see: `WindowManager::handleBuffers()`). \
Be sure to bring the back buffer forward to see the draw result.

> Params:
> **meshIn:** *The mesh object you wish to draw.*

#### void setDiscardFragments(MeshManager::Mesh &meshIn, bool shouldDiscard)
Toggle for removing the areas of a face prior to rendering where the meshes texture's alpha value is zero. Used for rendering sprites.

> Params: \
> **meshIn:** *The mesh object you wish to draw.* \
> **shouldDiscard:** *The desired value for the option (T/F).*

### Members:
> **Mesh:** *A collection of properties which make up a mesh entity. (type: `struct`)* 
>   - **id:** *The meshes serialised ID. Unique only to the manager instance which created it. (type: `int`)*
>   - **type:** *Which type of asset this mesh is. (type: `MeshManager::MeshType`)*
>   - **materials:** *The materials used by this mesh. (type: `std::vector<MeshManager::Material>`)*
>	- **numOfFaces:** *The number of faces that make up the mesh. (type: `int`)* 
>	- **numOfVertices:** *The number of vertices that make up the mesh. (type: `int`)* 
>	- **meshFilepath:** *The absolute path (from system root) to the wavefront file containing this mesh's vertex data. (type: `std::string`)*
>	- **materialFilepath:** *The absolute path (from system root) to the wavefront file containing this mesh's material data. (type: `std::string*`)*
>	- **position:** *Where the mesh is position in world space. (type: `glm::vec3`)*
>	- **direction:** *The mesh's forward-vector. Where the mesh's local coordinate system's z+ is in relation to world space. (type: `glm::vec3`)*
>	- **scale:** *The size of the mesh. (type: `glm::vec3`)*
>   - **modelMatrix:** *A 4x4 matrice used to perform transformations on the mesh. You will need this if you're intending you write your own transformations instead of using the `Transform` class. (type: `glm::mat4`)*
>   - **isClickable** *Whether or not this assets id can be looked up via pixel coordinate when it is occupying screenspace. (type: `bool`)*

> **MeshType:** *Different varieties of meshes (type: `enum`)*
>   - **LOADED_GLB:** *A mesh which has been loaded from a glb file.*
>   - **LOADED_WAVEFRONT:** *A mesh which has been loaded from a wavefront file.*
>   - **PLANE:** *A quad, I.e. Two-dimensional, four sides.*
>   - **CUBE:** *A cuboid / rectangular prism.*

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
> **shader:** *The id of the shader program used to render this camera. Acquired from the return value of `Shader::initialiseShader()`*

### Functions: 
#### CameraManager::Camera createPerspectiveCam(int aspectRatioX, int aspectRatioY)
Creates a new `Camera` object located at the scenes origin (x: 0.0, y: 0.0, z: 0.0) which looks directly down the +X axis. 

Returns a `Camera` object.

Params:
> **aspectRatioX:** *The x-axis aspect ratio / width of the viewport. (Default: `LAZARUS_PRIMARY_DISPLAY_WIDTH`)* \
> **aspectRatioY:** *The y-axis aspect ratio / height of the viewport. (Default: `LAZARUS_PRIMARY_DISPLAY_HEIGHT`)* 

### Functions: 
#### CameraManager::Camera createOrthoCam(int arX, int arY)
Creates a new instance of a `Camera`, with an orthographic projection matrix. 

Returns a new camera entity.

Params:
> **aspectRatioX:** *The x-axis aspect ratio / width of the viewport. (Default: `LAZARUS_PRIMARY_DISPLAY_WIDTH`)* \
> **aspectRatioY:** *The y-axis aspect ratio / height of the viewport. (Default: `LAZARUS_PRIMARY_DISPLAY_HEIGHT`)* 

#### CameraManager::Camera loadCamera(CameraManager::Camera cameraData)
Passes the camera's projection matrix and view matrix into the shader program's corresponding uniform locations.

Returns a new camera entity.

Params:
> **cameraData:** *The camera asset you would like to render.*

#### int getPixelOccupant(int positionX, int positionY)
Retrieves the ID of a pixel occupant in view which has `MeshManager::Mesh::isClickable` set to `true`.

> **positionX:** *The x-axis pixel coordinate from the bottom left corner of the display (Not the window).*
> **positionY:** *The y-axis pixel coordinate from the bottom left corner of the display (Not the window).*

### Members:
> **Camera:** *A collection of properties which make up a camera entity. (type: `struct`)* 
>	- **id:** *This camera's unique id. (type: `int`)* 
>	- **position:** *The x, y, z location of the camera. (type: `glm::vec3`)*
>	- **direction:** *Which direction the camera is facing. (type: `glm::vec3`)*
>	- **upVector:** *Which way is considered up. (type: `glm::vec3`)*
>	- **aspectRatio:** *The camera's aspect ratio. (type: `float`)*
>	- **viewMatrix:** *The view matrix to be passed into the shader program at the uniform location of `viewLocation`. (type: `glm::mat4`)*
>	- **projectionMatrix:** *The projection matrix to be passed into the shader program at the uniform location of `projectionLocation`. (type: `glm::mat4`)*

## LightManager:
A management class for light assets and their properties. 

### Constructor:
#### LightManager(GLuint shader)

Params:
> **shader:** *The id of the shader program used to render this light. Acquired from the return value of `Shader::initialiseShader()`*

### Functions: 
#### LightManager::Light createLightSource(glm::vec3 position, glm::vec3 color, float brightness)
Creates a new instance of an `Light`, initialises the values of its properties and returns it.

Returns a new light entity.

Params:
> **position:** *The desired world-space position of the light.* \
> **color:** *The desired RGB color of the light.*
> **brightness:** *The light's intensity / luminesence.*

#### void loadLightSource(LightManager::Light &lightData)
Passes the light object's locative (x,y,z) values into the vertex shader and its' colour (r,g,b) values into the fragment shader.

Params:
> **lightData:** *A reference to the light asset you would like to render.*

### Members:
> **Light:** *A collection of properties which make up a light entity. (type: `struct`)*
>	- **id:** *This light's unique id. (type: `int`)*
>   - **brightness**: *The intensity of the light.*
>	- **lightPosition:** *The x, y, z location of the light. (type: glm::vec3)*
>	- **lightColor:** *The r, g, b color values of the light. (type: glm::vec3)*

## AudioManager:
A management class using an `FMOD` backend for loading audio, as well as handling audio locations and listeners. 

### Constructor:
#### AudioManager()

### Functions:
#### void initialise()
Initialises `FMOD` core and instantiates a new system for interfacing with up to 512 unique audio channels. 

#### AudioManager::Audio createAudio(string filepath, bool is3D, int loopCount)
Creates a new instance of `AudioManager::Audio`, initialises the values of its properties and returns it.

Returns a new audio object.

Params:
> **filepath:** *The relative path to the audio (mp3 / wav) asset.* \
> **is3D:** *Indicates whether the sound has locative properties or it plays ambiently.* \
> **loopCount:** *The number of times the audio should loop. Use -1 to loop infinitely. (default: `0`)*

#### AudioManager::Audio loadAudio(AudioManager::Audio audioIn)
Loads a `AudioManager::Audio` object and prepares it for playback with `FMOD`.

Params:
> **audioIn:** *The audio object you want to load.*

#### AudioManager::Audio playAudio(AudioManager::Audio audioIn)
Plays a `AudioManager::Audio` object which has been previously loaded with `AudioManager::loadAudio()`.

Params:
> **audioIn:** *The audio object you want to start playing.*

#### AudioManager::Audio pauseAudio(AudioManager::Audio audioIn)
Pauses an `AudioManager::Audio` object which has been previously played with `AudioManager::playAudio()`.

Params:
> **audioIn:** *The audio object you want to pause / stop playing.*

#### AudioManager::Audio setPlaybackCursor(AudioManager::Audio &audioIn, int milliseconds)
Set the playback position of the target audio in milliseconds.

Params:
> **audioIn** *A reference to the target audio sample* \
> **milliseconds** *Target number of elapsed milliseconds since the audios beginning (0) to playback from.*

#### AudioManager::Audio updateSourceLocation(AudioManager::Audio audioIn, glm::vec3 location)
Updates the location in 3D of a `AudioManager::Audio` source; using `FMOD` to calculate the sound's doppler, relative to the listener's current positioning (*see*: `AudioManager::listenerLocationX`, `AudioManager::listenerLocationY` and `AudioManager::listenerLocationZ`).

Params:
> **audioIn**: *The audio sample to be updated.* \
> **location**: *The desired audio source location in worldspace.* \

#### void updateListenerLocation(glm::vec3 location)
Updates the location in 3D space of the audio's listener; using `FMOD` to calculate the doppler level, relative to the `AudioManager::Audio` current positioning. 

Params:
> **location**: *The desired audio listener location on in worldspace.* 

### Members:
> **Audio:** *A collection of properties which make up a sound asset. (type: `struct`)*
>	- **id:** *This audio's unique id. (type: `int`)*
>	- **sourceLocation:** *The audio's position in world space. (type: `glm::vec3`)*
>   - **is3D:** *Signifies whether the sound should be played in relation to the space or ambiently. (type: `bool`)*
>   - **isPaused:** *Signifies whether the sound is currently playing or not (type: `bool`)*
>   - **loopCount:** *The number of times that this audio sample should loop. Use -1 to loop indefinitely. (type: `int`), (default: `0`)*
>   - **audioIndex:** *Used internally to identify the audio object's location in the audio vector. (type: `int`)* 
> 

## TextManager:
A management class for rendering and laying out text on the screen.

### Constructor:
#### TextManager(GLuint shader)

Params:
> **shader:** *The id of the shader program used to render this camera. Acquired from the return value of `Shader::initialiseShader()`*

### Functions:
#### int extendFontStack(std::string filepath, int ptSize)
Loads a new TrueType font into a texture atlas stored in VRAM. \

Returns the font index used for font selection when loading text.

params:
> **filepath:** *The relative path to the TrueType `.ttf` font file.* \
> **ptSize:** *The desired character pt size. (default: `12`)*

#### TextManager::Text loadText(std::string targetText, int fontId, glm::vec2 location, glm::vec3 color, int letterSpacing, TextManager::Text textIn)
Loads the desired text using glyphs from the selected font. Sets the text's colour, position on the screen and letterspacing. It's worth noting \
here that a space `' '` is equal to `letterSpacing * 8`.

Pushes the text string into *this* `TextManager`' layout container. Optionally overwrite / update the contents of an existing piece of text by specifying an existing `layoutID` / layout index.

Returns a new `TextManager::Text` object.

Params:
> **targetText:** *The desired string to load to memory.* \
> **fontId:** *The index of the font the string should be rendered with.* \
> **location:** *The x and y-axis screen space coordinates of where the upper-left-most point of the first character is positioned.*
> **color:** *The desired RGB text color. (default: `{0.0, 0.0, 0.0} (black)`)* \
> **letterSpacing:** *How much spacing (in pixels) to put between each character. Word spacing is equal to this value * 8. (default: `1`)* \
> **textIn** *An existing text string in the layout container to update/replace upon successful load. (optional)*

#### void drawText(TextManager::Text text)
Draws text that has been loaded into the layout at the location of `text.layoutIndex` onto the screen.

Params:
> **text:** *A text object previously loaded by `TextManager::loadText`.*

### Members:
> **Text:** *A collection of properties which make up a string of text.*
>   - **layoutIndex:** *An identifier for performing lookup on strings loaded into the layout container. (type: `int`)*
>   - **locationX:** *The x-axis coordinate of the bottom left bounds of the string's first character. (type: `int`)*
>   - **locationY:** *The y-axis coordinate of the bottom left bounds of the string's first character. (type: `int`)*
>   - **targetString:** *The UTF-8 Ascii string literal to be rendered. (type: `std::string`)*
>   - **color:** *The text color. (type: `glm::vec3`)*

## WorldFX:
A management class for handling environmental effects such as skyboxes and soon; particles.

### Constructor:
#### WorldFX(GLuint shader)

Params:
> **shader:** *The id of the shader program used to render the subject's of this classes tooling. Acquired from the return value of `Shader::initialiseShader()`*

### Functions:

#### WorldFX::SkyBox createSkyBox(std::string rightPath, std::string leftPath, std::string downPath, std::string upPath, std::string frontPath, std::string backPath)
Creates and initialises a skybox object using textures loaded from the functions path params.

Params:
> **rightPath** *The relative path to the image used for the cube's `+X axis` face.* \
> **leftPath** *The relative path to the image used for the cube's `-X axis` face.* \
> **downPath** *The relative path to the image used for the cube's `-Y axis` face.* \
> **upPath** *The relative path to the image used for the cube's `+Y axis` face.* \
> **frontPath** *The relative path to the image used for the cube's `+Z axis` face.* \
> **backPath** *The relative path to the image used for the cube's `-Z axis` face.*

#### void drawSkyBox(WorldFX::SkyBox sky, CameraManager::Camera camera)
Draws texels from the specified skybox into the scene at the location of the camera's direction vector.

Params:
> **sky** The target skybox object to be drawn. \
> **camera** The camera object used to determine the target uvw coordinate of the texel to the sampled from the bound cubemap image texture.

#### WorldFX::Fog createFog(float minDistance, float maxDistance, float thickness, glm::vec3 color, glm::vec3 position)
Creates and a initialises a cloud of fog into the scene with radial visibility bounds. I.e. spherical area of non-fog.

Params: 
> **minDistance:** *The radius from xyz wherein fog's attenuation / opacity begins taking effect.*
> **maxDistance:** *The radius from xyz wherein fog reaches full opacity.*
> **thickness:** *The fog's density.*
> **color:** *The fog's RGB color values.*
> **position:** *The worldspace coordinates of the viewpoint. (default: `{0.0, 0.0, 0.0} (origin)`).*

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