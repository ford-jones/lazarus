# Lazarus by Example:

## Window creation:
```cpp
#include <lazarus.h>

int main()
{
    Lazarus::WindowManager window = Lazarus::WindowManager(800, 600, "Game Window");
    window.initialise();

    return 0;
}
```
## Shaders:

### Using your own shaders:
Shaders submitted to and compiled by Lazarus should be written in GLSL at version **410**. \
When you submit your own fragment and / or vertex shaders with a call to `Shader::compileShaders()`, there are a number of layout positions, as well as; sampler and uniform names reserved by Lazarus. These variables are accessible in any of your own shader programs and are listed below.

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
```

#### Pixel / Fragment shader inputs:
Note these inputs can be found at `LAZARUS_DEFAULT_FRAG_LAYOUT`.
```c
    #define MAX_LIGHTS 150                          

    in vec3 fragPosition;                           //  Input 3D fragment position
    in vec3 diffuseColor;                           //  Input fragment color
    in vec3 normalCoordinate;                       //  Input fragment normals
    in vec3 textureCoordinate;                      //  Input UV coordinates 
    in vec3 skyBoxTextureCoordinate;                //  Input UV coords for skyboxes

    flat in int isUnderPerspective;                 //  1 if a perspective camera is being used to observe this fragment, otherwise 0

    uniform int lightCount;                         //  The number of lights currently bound to lazarus
    uniform vec3 lightPositions[MAX_LIGHTS];        //  A container of 3D light positions in-order
    uniform vec3 lightColors[MAX_LIGHTS];           //  A container of light color values in-order
    uniform float lightBrightness[MAX_LIGHTS];      //  A container of light brightness variables in-order

    uniform vec3 fogColor;                          //  The color of environmental fog if it is present
    uniform vec3 fogViewpoint;                      //  Where the epicenter from which fog thickness attenuates out from
    uniform float fogMaxDist;                       //  The maximum distance at which environment fog effects are observable
    uniform float fogMinDist;                       //  The minimum distance at which environment fog effects are observable
    uniform float fogDensity;                       //  Thickness of environment fog if present

    uniform vec3 textColor;                         //  The color of the text, if the fragment is part of an ascii glyph

    uniform int spriteAsset;                        //  1 if the fragment is part of a 2D cutout (i.e. alpha 0 is removed), otherwise 0
    uniform int glyphAsset;                         //  1 if fragment is part of an ascii character, otherwise 0
    uniform int isSkyBox;                           //  1 if the fragment is a skybox, otherwise 0

    uniform float textureLayer;                     //  The index of the render subjects texture in the texture array if one was specified

    uniform sampler2D textureAtlas;                 //  Glyph atlas used for font bitmaps
    uniform sampler2DArray textureArray;            //  Array sampler / testure stack
    uniform samplerCube textureCube;                //  Cubemap sampler used for skyboxes

    out vec4 outFragment;                           //  The output fragment color
```