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

## WindowManager:
A class for making and managing the program's window(s). 

### Constructor:
#### WindowManager(const char *title, int width, int height)

Params:
> **title**: *The window's title* \
> **width**: *The width of the window. (default: `800`)* \
> **height**: *The height of the window. (default: `600`)* \

### Functions:
#### int initialise()
Creates a new instance of a window, initialises OpenGL and creates a GL context.

#### int loadConfig(GLuint shader)
Binds a shader program to the current active window's OpenGL Context and loads a render configuration based on values set in the global scope (see: `GlobalsManager`).

Params:
> **shader:** *The id of the engine's shader program. This can be acquired from a call to `Shader::initialiseShader()` or by compiling your own shader program. (default: `0`)* 


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

#### int handleBuffers()
Bring the back buffer to the front (into user view) and moves the front buffer to the back. \
Clears the back buffer's depth and color bits so that they can be given new values for the next draw.

### Members:
> **isOpen:** *Whether or not the active window is open. See also: `GlobalsManager::getContextWindowOpen()`. (type: `bool`, default: `false`)* \

## EventManager
A class for tracking, storing and managing window events as well as their values.

### Functions:
#### void initialise()
Locates the programs active window and loads the following event-handler callbacks into it's context's event loop: 
- keydown 
- mousedown 
- mousemove 
- scroll

#### void listen()
Polls GLFW for the head of the active window's event queue and then updates the values of the event managers members.

### Members:
> **keyString:** *The key currently being pressed, expressed as a string. (type: `std::string`)* \
> **keyCode:** *The openGL code of the string currently being pressed. (type: `int`)* \
> **osCode:** *The operating-system specific "scancode" of the key currently being pressed. (type `int`)* \
> **mouseCode:** *An integer in the range of 0 to 8 expressing which mouse button is currently being clicked / pressed. (type: `int`)* \
> **mouseX:** *The current x-axis coordinate of the cursor. (type: `int`)* \
> **mouseY:** *The current y-axis coordinate of the cursor. (type: `int`)* \
> **scrollCode:** *An integer in the range of -1 to 1 expressing the scrollwheel's y-offset.* 

## Shader:
A class for the lazarus default shader program which, simply maps vertex positions to their coordinates and draws the fragments; following the lambertian lighting model.

### Constructor:
#### Shader()
Default-initialises this classes members.

### Functions:
#### GLuint initialiseShader()
Invokes the parsing, compiling, attatching and linking of the Lazarus vert and frag shaders. \
Returns the Id (OpenGL unsigned integer) of the shader program which can then be passed to the various constructors which need it.

*Note:* \
*Failure to pass the return value of this function as an argument into `WindowManager::loadConfig()` will cause the program to use the GL default shader program.* \
*While this may still render graphics in some limitted capacity, it is not reccomended to do so.* \
*Loading your own program should work fine, so long as you note the return GLuint value of the call to `glCreateProgram()`*

## FpsCounter:
A simple frames-per-second listener class.

### Functions:
#### void calculateFramesPerSec();
Calculates the current frames per second, as well as the number of milliseconds taken to render a single frame.

### Members:
> **framesPerSecond:** *Current number of frames being drawn per second. (type: `double`)* \
> **durationTillRendered:** *The duration of time taken in milliseconds to draw a single frame. (type: `double`)* 

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
#### MeshManager::Mesh translateMeshAsset(MeshManager::Mesh mesh, float x, float y, float z)
Applies a translation transformation (movement) to a mesh asset along the x, y and z axis from an offset of 0.0. \
Updates the `locationX`, `locationY` and `locationZ` properties of a `MeshManager::Mesh` object in real time. \
Returns a new mesh entity.

Params:
> **mesh:** *The mesh asset to be acted upon. (type: `MeshManager::Mesh`)* \
> **x:** *A floating point number used to increment / decrement the x-axis locative value of the mesh.* \
> **y:** *A floating point number used to increment / decrement the y-axis locative value of the mesh.* \
> **z:** *A floating point number used to increment / decrement the z-axis locative value of the mesh.*

#### MeshManager::Mesh rotateMeshAsset(MeshManager::Mesh mesh, float x, float y, float z)
Applies a rotation transformation to a mesh asset on it's x, y and z axis from an offset of 0.0. \
This rotation affects the yaw, pitch and roll of the mesh. Not to be confused with an orbital rotation. \
Returns a new mesh entity.

Params:
> **mesh:** *The mesh asset to be acted upon. (type: `MeshManager::Mesh`)* \
> **x:** *A floating point number used to increment / decrement the x-axis (yaw) rotational value of the mesh.* \
> **y:** *A floating point number used to increment / decrement the y-axis (pitch) rotational value of the mesh.* \
> **z:** *A floating point number used to increment / decrement the z-axis (roll) rotational value of the mesh.*

#### CameraManager::Camera translateCameraAsset(CameraManager::Camera camera, float x, float y, float z, float velocity)
Applies a translation transformation (movement) to a camera asset along the x, y and z axis from an offset of 0.0. \
Updates the `locationX`, `locationY` and `locationZ` properties of a `CameraManager::Camera` object in real time. \
Returns a new camera entity.

Params:
> **camera:** *The camera asset to be acted upon. (type: `CameraManager::Camera`)* \
> **x:** *A floating point number used to increment / decrement the x-axis locative value of the camera.* \
> **y:** *A floating point number used to increment / decrement the y-axis locative value of the camera.* \
> **z:** *A floating point number used to increment / decrement the z-axis locative value of the camera.*
> **velocity:** *The speed at which the camera should translate through space per update. (default: `0.1`)*

#### CameraManager::Camera rotateCameraAsset(CameraManager::Camera camera, float x, float y, float z)
Applies a rotation transformation to a camera asset on it's x, y and z axis from an offset of 0.0. \
This rotation affects the yaw, pitch and roll of the camera. Not to be confused with an orbital rotation. \
Returns a new camera entity.

Params:
> **camera:** *The camera asset to be acted upon. (type: `CameraManager::Camera`)* \
> **x:** *A floating point number used to increment / decrement the x-axis locative value of the camera.* \
> **y:** *A floating point number used to increment / decrement the y-axis locative value of the camera.* \
> **z:** *A floating point number used to increment / decrement the z-axis locative value of the camera.*

#### LightManager::Light translateLightAsset(LightManager::Light light, float x, float y, float z)
Applies a translation transformation (movement) to a light asset along the x, y and z axis from an offset of 0.0. \
Updates the `locationX`, `locationY` and `locationZ` properties of a `LightManager::Light` object in real time. \
Returns a new light entity.


Params:
> **light:** *The light asset to be acted upon. (type: `LightManager::Light`)* \
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
#### MeshManager::Mesh create3DAsset(std::string meshPath, std::string materialPath, std::string texturePath)
Finds and reads a wavefront (obj) file located at `meshPath`. \
Creates a new instance of a `Mesh`, initialises the values of its properties and returns it. \
Invokes the `MaterialLoader::loadMaterial()` function and passes on the `materialPath`.

Returns a new mesh entity.

Params:
> **meshPath:** *The relative path to the wavefront mesh asset you wish to render.* 
> **materialPath:** *The relative path to the wavefront material asset you wish to render.*
> **texturePath:** *The relative path to the texture image. (optional)*

#### MeshManager::Mesh createQuad(float width, float height, std::string texturePath, float uvXL, float uvXR, float uvY)
Creates a quad (2D plane) to the size of the specified height and width. \
Textures loaded into a quad have their fragments discarded where the texture opacity is 0.0 - used for sprites.

Returns a new mesh entity.

Params:
> **width:** *The relative path to the wavefront mesh asset you wish to render.* 
> **height:** *The relative path to the wavefront material asset you wish to render.*
> **texturePath:** *The relative path to the texture image. (optional)*
> **uvXL:** *The normalised x-axis coordinate of the UV's left-side. Used for text rendering. (optional)*
> **uvXR:** *The normalised x-axis coordinate of the UV's right-side. Used for text rendering. (optional)*
> **uvY:** *The normalised y-axis coordinate of the UV's top edge. Used for text rendering. (optional)*

#### MeshManager::Mesh loadMesh(MeshManager::Mesh meshData)
Loads a mesh object's buffer data into their correct GPU uniform positions located inside the shader program that was referenced in the class constructor.

Params:
> **meshData:** *The mesh object who's buffer data you wish to pass into the shader program.*

#### MeshManager::Mesh drawMesh(MeshManager::Mesh meshData)
Draws the mesh object contents of the shader program's uniforms onto the render loops back buffer (see: `WindowManager::handleBuffers()`). \
Be sure to bring the back buffer forward to see the draw result.

> Params:
> **meshData:** *The mesh object you wish to draw.*

### Members:
> **Mesh:** *A collection of properties which make up a mesh entity. (type: `struct`)* 
>	- **textureId:** *The serialised id of the mesh objects texture. The layer depth of the texture. (type: `int`)* 
>   - **is3D:** *Literal 0 (false) or 1 (true). Flags the shader to treat the mesh as a sprite, discarding fragments with an alpha value higher than 0.1 (type: `int`)*
>	- **numOfFaces:** *The number of faces that make up the mesh. (type: `int`)* 
>	- **numOfVertices:** *The number of vertices that make up the mesh. (type: `int`)* 
>	- **meshFilepath:** *The absolute path (from system root) to the wavefront file containing this mesh's vertex data. (type: `std::string`)*
>	- **materialFilepath:** *The absolute path (from system root) to the wavefront file containing this mesh's material data. (type: `std::string*`)*
>	- **textureFilepath:** *The absolute path (from system root) to the wavefront file containing this mesh's texture image. (type: `std::string`)*
>	- **locationX:** *The x-axis coordinate of the mesh's position in world space. (type: float)*
>	- **locationY:** *The y-axis coordinate of the mesh's position in world space. (type: float)*
>	- **locationZ:** *The z-axis coordinate of the mesh's position in world space. (type: float)*
>	- **attributes:** *This mesh's vertex attributes interleaved in order of position, diffuse color, normal coords and finally uv coords. (type: `glm::vector<glm::vec3>>`)*
>	- **diffuse:** *Diffuse material (colour) data extracted from the wavefront material (mtl) file. (type: `glm::vector<glm::vec3>>`)*
>   - **textureData:** *A struct containing image data. (type: `FileReader::Image`)*
>	- **modelViewMatrix:** *A 4x4 modelview matrix to be passed into the shader program at the uniform location of `modelViewUniformLocation`. (type: `glm::mat4`)*
>	- **modelViewUniformLocation:** *The uniform location / index of the vert shader's modelview matrice. (type: GLuint)*
>   - **samplerUniformLocation:** *The location inside of the shader program of the texture array uniform which holds this mesh's texture data (if any). (type: `GLint`)*
>   - **textureLayerUniformLocation:** *The location inside of the shader program of the uniform in which the active texture layer is passed.*

## MeshLoader:
A simple loader class for loading wavefront (obj) files and marshalling their contents into variables.

### Contructor:
#### MeshLoader()
Default-initialises this classes members.

### Functions:
#### bool parseWavefrontObj(const char* path, std::vector\<glm::vec3> &outAttributes, std::vector\<glm::vec3> &outDiffuse, GLuint &outTextureId, FileReader::Image &imageData, const char *meshPath, const char *materialPath, const char *texturePath = "")
Parses a wavefront (obj) file.

Returns a boolean, if an error occurs or the file cannot be loaded this value will be `false`.

Params:
> **outAttributes:** *A vector to store interleaved vertex attribute data in AOS format in the following order: vertex positions, diffuse colors, normal coordinates, uv coordinates.* \
> **outDiffuse:** *A vector for storing diffuse color data. Literal `-0.01f` values indicate the use of an image texture.* \
> **outTextureId:** *The serialised ID of a valid named texture. Returns 0 if no textures are used. Otherwise returns an integer used to indicate where the texture is stored in the shader programs texture array.* \
> **imageData:** *A struct containing texel data, image width and height.* \
> **meshPath:** *The absolute path to this mesh's wavefront (.obj) file* \
> **materialPath:** *The absolute path to this mesh's wavefront material (.mtl) file.* \
> **texturePath:** *The absolute path to this mesh's texture file (optional).* 

### Members:
> **foundMaterial:** *The absolute path to this mesh's matertial (.mtl) file. (type: `std::string`)* \
> **file:** *A pointer to the material file stored in your local filesystem. (type: `std::ifstream*`)* \
> **vertexIndices:** *Grouped indexes / locations of relevant vertex coordinate data who's values make up the geometry of one of the mesh's faces. (type: `std::vector<unsigned int>`)* \
> **uvIndices:** *Grouped indexes / locations of relevant vertex texture data who's values make up the texture wrap of one of the mesh's faces. (type: `std::vector<unsigned int>`)* \
> **normalIndices:** *Grouped indexes / locations of relevant vertex normal data who's values make up the direction that one of the mesh's faces is facing. (type: `std::vector<unsigned int>`)* \
> **tempVertices:** *A vector to store the actual vertex coordinate values found at the `vertexIndices` indexes. (type: `std::vector<glm::vec3>`)* \
> **tempUvs:** *A vector to store the actual texture values found at the `uvIndices` indexes. (type: `std::vector<glm::vec2>`)* \
> **tempNormals:** *A vector to store the actual vertex normal values found at the `normalIndices` indexes. (type: `std::vector<glm::vec3>`)* \
> **tempDiffuse:** *A vector to store diffuse colour data returned from a call to `MaterialLoader::loadMaterial()`. (type: `std::vector<glm::vec3>`)*

## MaterialLoader:
A simple loader class for loading wavefront (obj) files and marshalling their contents into variables.

### Contructor:
#### MaterialLoader()
Default-initialises this classes members.

### Functions: 
#### bool loadMaterial(std::vector\<glm::vec3> &out, std::vector<std::vector<int>> data, std::string materialPath, GLuint &textureId, FileReader::Image imageData, std::string texturePath = "")
Parses a wavefront material (mtl) file for it's diffuse colour values, which are converted to `float` and stored inside a `glm::vec3`. \

Returns a boolean, if an error occurs or the file cannot be loaded this value will be `false`.

Params:
> **out:** *A vector for storing diffuse colour data.* \
> **data:** *another vector which; at index 0 holds a material id and at index 1 is the number of faces using it.*
> **materialPath:** *The absolute path to this mesh's matertial (.mtl) file.* \
> **textureId:** *The serialised ID of a valid named texture. Returns 0 if no textures are used. Otherwise returns an integer used to indicate where the texture is stored in the shader programs texture array.* \
> **imageData:** *A struct containing texel data, image width and height.* \
> **texturePath:** *The absolute path to this mesh's texture file (optional).*

## CameraManager:

### Constructor:
Default initialises this class's members.

#### CameraManager(GLuint shader)

Params:
> **shader:** *The id of the shader program used to render this camera. Acquired from the return value of `Shader::initialiseShader()`*

### Functions: 
#### CameraManager::Camera createPerspectiveCam(double pX, double pY, double pZ, double tX, double tY, double tZ, int arX, int arY)
Creates a new instance of a `Camera`, with a perspective projection matrix. 

Returns a new camera entity.

Params:
> **pX:** *The x-axis starting position / location of the camera.* \
> **pY:** *The y-axis starting position / location of the camera.* \
> **pZ:** *The z-axis starting position / location of the camera.* \
> **tX:** *The x-axis target / lookat coordinate.* \
> **tY:** *The y-axis target / lookat coordinate.* \
> **tZ:** *The z-axis target / lookat coordinate.* \
> **arX:** *The x-axis aspect ratio / width of the viewport. (Default: `LAZARUS_PRIMARY_DISPLAY_WIDTH`)* \
> **arY:** *The y-axis aspect ratio / height of the viewport. (Default: `LAZARUS_PRIMARY_DISPLAY_HEIGHT`)* 

### Functions: 
#### CameraManager::Camera createOrthoCam(double pX, double pY, double pZ, double tX, double tY, double tZ, int arX, int arY)
Creates a new instance of a `Camera`, with an orthographic projection matrix. 

Returns a new camera entity.

Params:
> **arX:** *The x-axis aspect ratio / width of the viewport.*  \
> **arY:** *The y-axis aspect ratio / height of the viewport.* 

#### CameraManager::Camera loadCamera(CameraManager::Camera cameraData)
Passes the camera's projection matrix and view matrix into the shader program's corresponding uniform locations.

Returns a new camera entity.

Params:
> **cameraData:** *The camera asset you would like to render.*

### Members:
> **Camera:** *A collection of properties which make up a camera entity. (type: `struct`)* 
>	- **id:** *This camera's unique id. (type: `int`)* 
>	- **position:** *The x, y, z location of the camera. (type: `glm::vec3`)*
>	- **direction:** *The x, y, z location of where the camera is facing. (type: `glm::vec3`)*
>	- **target:** *The x, y, z location of the camera's target. (type: `glm::vec3`)*
>	- **upVector:** *Which way is considered up. (type: `glm::vec3`)*
>	- **aspectRatio:** *The camera's aspect ratio. (type: `float`)*
>	- **viewLocation:** *The uniform location / index vert shader's view matrice. (type: `GLuint`)*
>	- **projectionLocation:** *The uniform location / index vert shader's projection matrice. (type: `GLuint`)*
>	- **viewMatrix:** *The view matrix to be passed into the shader program at the uniform location of `viewLocation`. (type: `glm::mat4`)*
>	- **projectionMatrix:** *The projection matrix to be passed into the shader program at the uniform location of `projectionLocation`. (type: `glm::mat4`)*

## LightManager:
A management class for light assets and their properties. 

### Constructor:
#### LightManager(GLuint shader)

Params:
> **shader:** *The id of the shader program used to render this light. Acquired from the return value of `Shader::initialiseShader()`*

### Functions: 
#### LightManager::Light createLightSource(double x, double y, double z, double r, double g, double b)
Creates a new instance of an `Light`, initialises the values of its properties and returns it.

Returns a new light entity.

Params:
> **x:** *The x-axis starting coordinate of the light in world-space.* \
> **y:** *The y-axis starting coordinate of the light in world-space.* \
> **z:** *The z-axis starting coordinate of the light in world-space.* \
> **r:** *This light's red colour value.* \
> **g:** *This light's green colour value.* \
> **b:** *This light's blue colour value.* 

#### void loadLightSource(LightManager::Light &lightData)
Passes the light object's locative (x,y,z) values into the vertex shader and its' colour (r,g,b) values into the fragment shader.

Params:
> **lightData:** *A reference to the light asset you would like to render.*

### Members:
> **Light:** *A collection of properties which make up a light entity. (type: `struct`)*
>	- **id:** *This light's unique id. (type: `int`)*
>	- **locationX:** *The x-axis coordinate of the light's position in world space. (type: `float`)*
>	- **locationY:** *The y-axis coordinate of the light's position in world space. (type: `float`)*
>	- **locationZ:** *The z-axis coordinate of the light's position in world space. (type: `float`)*
>	- **lightPosition:** *The x, y, z location of the light. (type: glm::vec3)*
>	- **lightColor:** *The r, g, b color values of the light. (type: glm::vec3)*
>	- **lightPositionUniformLocation:** *The location / index of the vertex shader position uniform. (type: `GLuint`)*
>	- **lightColorUniformLocation:** *The location / index of the fragment shader diffuse uniform. (type: `GLuint`)*

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


#### AudioManager::Audio updateSourceLocation(AudioManager::Audio audioIn, float x, float y, float z)
Updates the location in 3D of a `AudioManager::Audio` source; using `FMOD` to calculate the sound's doppler, relative to the listener's current positioning (*see*: `AudioManager::listenerLocationX`, `AudioManager::listenerLocationY` and `AudioManager::listenerLocationZ`).

Params:
> **audioIn**: *The audio sample to be updated.* \
> **x**: *The desired audio source location on the x-axis.* \
> **y**: *The desired audio source location on the y-axis.* \
> **z**: *The desired audio source location on the z-axis.* 

#### void updateListenerLocation(float x, float y, float z)
Updates the location in 3D space of the audio's listener; using `FMOD` to calculate the doppler level, relative to the `AudioManager::Audio` current positioning. 

Params:
> **x**: *The desired audio listener location on the x-axis.* \
> **y**: *The desired audio listener location on the y-axis.* \
> **z**: *The desired audio listener location on the z-axis.* 

### Members:
> **Audio:** *A collection of properties which make up a sound asset. (type: `struct`)*
>	- **id:** *This audio's unique id. (type: `int`)*
>	- **sourceLocationX:** *The x-axis coordinate of the audio's position in world space. (type: `float`)*
>	- **sourceLocationY:** *The y-axis coordinate of the audio's position in world space. (type: `float`)*
>	- **sourceLocationZ:** *The z-axis coordinate of the audio's position in world space. (type: `float`)*
>   - **is3D:** *Signifies whether the sound should be played in relation to the space or ambiently. (type: `bool`)*
>   - **isPaused:** *Signifies whether the sound is currently playing or not (type: `bool`)*
>   - **loopCount:** *The number of times that this audio sample should loop. Use -1 to loop indefinitely. (type: `int`), (default: `0`)*
>   - **audioIndex:** *Used internally to identify the audio object's location in the audio vector. (type: `int`)* 
> 
> **listenerLocationX:** *The audio listener's location on the x-axis.* \
> **listenerLocationY:** *The audio listener's location on the y-axis.* \
> **listenerLocationZ:** *The audio listener's location on the z-axis.*

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

#### int loadText(std::string targetText, int posX, int posY, int letterSpacing, float red, float green, float blue, int layoutID)
Loads the desired text using glyphs from the selected font. Sets the text's colour, position on the screen and letterspacing. It's worth noting \
here that a space `' '` is equal to `letterSpacing * 8`.

Pushes the text string into *this* `TextManager`' layout container. Optionally overwrite / update the contents of an existing piece of text by specifying an existing `layoutID` / layout index.

Returns the word(s) layout index.

Params:
> **targetText:** *The desired string to load to memory.* \
> **posY:** *The y-axis coordinate of where the upper-left-most point of the first character should be positioned in pixels. With the origin (0.0) starting in the bottom left.* \
> **posX:** *The x-axis coordinate of where the upper-left-most point of the first character should be positioned in pixels. With the origin (0.0) starting in the bottom left.* \
> **letterSpacing:** *How much spacing (in pixels) to put between each character. Word spacing is equal to this value * 8. (default: `1`)* \
> **red:** *Set the decimal value of the text's red colour channel. (default: `0.0`)* \
> **green:** *Set the decimal value of the text's green colour channel. (default: `0.0`)* \
> **blue:** *Set the decimal value of the text's blue colour channel. (default: `0.0`)* \
> **layoutID** *The ID of an existing text string in the layout container. (default: `-1`)*

#### void drawText(int layoutIndex)
Draws text that has been loaded into the layout at `layoutIndex` onto the screen.

Params:
> **layoutIndex:** *The identifier of a string previously loaded by `TextManager::loadText`.*

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

## Status Codes:
- **LAZARUS_OK** *The engines default state. No problems. (Code: 0)* 
- **LAZARUS_FILE_NOT_FOUND** *The specified asset couldn't be found (Code: 101)* 
- **LAZARUS_FILE_UNREADABLE** *The located file cannot be read. (Code: 102)* 
- **LAZARUS_FILESTREAM_CLOSED** *The filestream input closed unexpectedly. (Code: 103)* 
- **LAZARUS_IMAGE_LOAD_FAILURE** *STB was unable to load the contents of the given image file into a 8_8_8_8 (RGBA) buffer. (Code: 104)*
- **LAZARUS_IMAGE_RESIZE_FAILURE** *STB was unable to resize the image to the height and width specified at `LAZARUS_MAX_IMAGE_WIDTH` / `LAZARUS_MAX_IMAGE_HEIGHT` (Code: 105)*
- **LAZARUS_FT_INIT_FAILURE** *Lazarus failed to inititialise the freetype2 library. (Code: 106)*
- **LAZARUS_FT_LOAD_FAILURE** *Freetype has indicated that it is unable to load the TrueType font from the desired location. (Code: 107)*
- **LAZARUS_FT_RENDER_FAILURE** *Despite being able to load the target glyph's splines, freetype was unable to render them into a monochrome bitmap. (Code: 108)*
- **LAZARUS_SHADER_ERROR** *OpenGL does not regard the output from shader compilation to be a valid shader program. (Code: 201)* 
- **LAZARUS_VSHADER_COMPILE_FAILURE** *The vertex shader failed to compile. (Code: 202)*
- **LAZARUS_FSHADER_COMPILE_FAILURE** *The fragment shader failed to compile. (Code: 203)
- **LAZARUS_SHADER_LINKING_FAILURE** *OpenGL failed to link the shaders. (Code: 204)*
- **LAZARUS_UNIFORM_NOT_FOUND** *Lazarus failed to perform a lookup on the desired uniform from the vertex or fragment shader. (Code: 205)*
- **LAZARUS_MATRIX_LOCATION_ERROR** *Lazarus failed to perform a shader lookup on the desired modelview, projection or view-matrix required to render the target entity. (Code: 206)*
- **LAZARUS_OPENGL_ERROR** *An error occured in the OpenGL graphics pipeline. (Code: 301)*
- **LAZARUS_NO_CONTEXT** *Unable to find a window with an active OpenGL context. (Code: 302)*
- **LAZARUS_WINDOW_ERROR** *An error occured in the GLFW window API. (Code: 303)*
- **LAZARUS_GLFW_NOINIT** *GL framework wrangler failed to initialise. (Code: 304)*
- **LAZARUS_WIN_EXCEEDS_MAX** *The requested window size is larger than the dimensions of the primary monitor. (Code: 305)*
- **LAZARUS_AUDIO_ERROR** *An error occured in the FMOD audio backend. (Code: 401)*
- **LAZARUS_INVALID_RADIANS** *Lazarus recieved a rotational value which exceeds 360.0. (Code: 501)*
- **LAZARUS_INVALID_CUBEMAP** *The images recieved to construct a cubemap texture are not all of equal height and width (Code: 502)*