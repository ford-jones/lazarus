//              .,                                                                                                          .(*      ,*,                
//                ((.     (.    ,                                                                          */*  ,%##%%%%%%%%%%%%%%%#(*           .      
//         .//**/#%%%%%%%%%%%%%%#*   .,                                                             ,**   .(%%%%%%#(******,***,/#%%%%%%%%###(/,         
//             #%%%#*.#%/***,,..,*(%(.    ,,                                                     *     /#%##/*****,,,,,,,,,.,...,,#%,  .#%#.            
//    .,     *%&#/   %#**,,*..,....,.*#,     ..                                               *     ,%#%#/*,,*,*,,,,,.,.,,.,...,...((     /#(//*/**.    
//           (%#    *#*...,.,,..........*/      ,                                          .      *#%(#(**,,,,,,,,..,..,..,,........(.     *#(          
//           *#     *(......,.............(#      ,                                       .     ,((, ##,,,.....,.................. ./       **  .,.     
//            *     ./........ ...........*#*,                                          ,      ,(,  ./*,,,..,,.................  .. *                   
//                   /, ........    ... ../(  *.                                              ,*     /,...,,.,,.....   ............**                   
//                    *... .............  /    ,                                             *,      ,*,,............  ,....     ...                    
//                     *.   ..... .... ..*                                                  .*        *...................   .  ...                     
//               *       ... ......... ,.                                                   ,          ... ..........  ...     ..       ,               
//                ((        .,.,.. ...                                                                   .  . .. .  .  ... .  ..      //                
//              ,/(#%#*                                                                                     .....  ... ......       .#*                 
//                 /((##%#(*                                                                                      .......        ,(#(*,                 
//               (.           .,,,,,                                                                                        .*#%%(                      
//                                                                                                      .***,.   . .,/##%###(/.  ...,,.      
/*  LAZARUS ENGINE */

#include "../include/lazarus_common.h"

uint32_t                 LAZARUS_EXECUTION_STATUS               = 0;
uint32_t                 LAZARUS_PRIMARY_DISPLAY_WIDTH          = 0;
uint32_t                 LAZARUS_PRIMARY_DISPLAY_HEIGHT         = 0;
uint32_t                 LAZARUS_LIGHT_COUNT                    = 0;
std::vector<uint32_t>    LAZARUS_SELECTABLE_ENTITIES            = {};
const uint32_t           LAZARUS_MAX_LIGHTS                     = 150;
const uint32_t           LAZARUS_MAX_SELECTABLE_ENTITIES        = 254;
const uint32_t           LAZARUS_MOUSE_NOCLICK                  = 999;

uint32_t                 LAZARUS_MAX_IMAGE_WIDTH                = 0;
uint32_t                 LAZARUS_MAX_IMAGE_HEIGHT               = 0;
bool                     LAZARUS_ENFORCE_IMAGE_SANITY           = false;
bool                     LAZARUS_DISABLE_CURSOR_VISIBILITY      = false;
bool                     LAZARUS_LAUNCH_IN_FULLSCREEN           = false;
bool                     LAZARUS_DISABLE_VSYNC                  = false;
bool                     LAZARUS_DO_STENCIL_BUFFER              = false;
bool                     LAZARUS_CULL_BACK_FACES                = true;
bool                     LAZARUS_DEPTH_TEST_FRAGS               = true;

uint32_t                 LAZARUS_LISTENER_KEYCODE               = 0;
uint32_t                 LAZARUS_LISTENER_SCANCODE              = 0;
uint32_t                 LAZARUS_LISTENER_MOUSECODE             = LAZARUS_MOUSE_NOCLICK;
_Float32                 LAZARUS_LISTENER_MOUSEX                = 0;
_Float32                 LAZARUS_LISTENER_MOUSEY                = 0;
_Float32                 LAZARUS_LISTENER_SCROLLCODE            = 0;

const char*              LAZARUS_SKYBOX_CUBE                    = "Skybox target.";
const char*              LAZARUS_GLYPH_QUAD                     = "Glyph target.";
const char*              LAZARUS_PRIMITIVE_MESH                 = "Primitive mesh.";
const char*              LAZARUS_TEXTURED_MESH                  = "Textured mesh.";
const char*              LAZARUS_DIFFUSE_MESH                   = "Diffuse colored mesh.";

const uint32_t           LAZARUS_OK                             = 0;
const uint32_t           LAZARUS_FILE_NOT_FOUND                 = 101;
const uint32_t           LAZARUS_FILE_UNREADABLE                = 102;
const uint32_t           LAZARUS_FILESTREAM_CLOSED              = 103;
const uint32_t           LAZARUS_IMAGE_LOAD_FAILURE             = 104;
const uint32_t           LAZARUS_IMAGE_RESIZE_FAILURE           = 105;
const uint32_t           LAZARUS_FT_INIT_FAILURE                = 106;
const uint32_t           LAZARUS_FT_LOAD_FAILURE                = 107;
const uint32_t           LAZARUS_FT_RENDER_FAILURE              = 108;
const uint32_t           LAZARUS_SHADER_ERROR                   = 201;
const uint32_t           LAZARUS_VSHADER_COMPILE_FAILURE        = 202;
const uint32_t           LAZARUS_FSHADER_COMPILE_FAILURE        = 203;
const uint32_t           LAZARUS_SHADER_LINKING_FAILURE         = 204;
const uint32_t           LAZARUS_UNIFORM_NOT_FOUND              = 205;
const uint32_t           LAZARUS_MATRIX_LOCATION_ERROR          = 206;
const uint32_t           LAZARUS_OPENGL_ERROR                   = 301;
const uint32_t           LAZARUS_NO_CONTEXT                     = 302;
const uint32_t           LAZARUS_WINDOW_ERROR                   = 303;
const uint32_t           LAZARUS_EVENT_ERROR                    = 304;
const uint32_t           LAZARUS_GLFW_NOINIT                    = 305;
const uint32_t           LAZARUS_WIN_EXCEEDS_MAX                = 306;
const uint32_t           LAZARUS_TIME_ERROR                     = 307;
const uint32_t           LAZARUS_AUDIO_ERROR                    = 401;
const uint32_t           LAZARUS_AUDIO_PLAYBACK_POSITION_ERROR  = 402;
const uint32_t           LAZARUS_AUDIO_LOAD_ERROR               = 403;
const uint32_t           LAZARUS_INVALID_RADIANS                = 501;
const uint32_t           LAZARUS_INVALID_CUBEMAP                = 502;
const uint32_t           LAZARUS_INVALID_COORDINATE             = 503;
const uint32_t           LAZARUS_INVALID_DIMENSIONS             = 504;
const uint32_t           LAZARUS_INVALID_INTENSITY              = 505;
const uint32_t           LAZARUS_FEATURE_DISABLED               = 506;

void GlobalsManager::setEnforceImageSanity(bool shouldEnforce)
{
    LAZARUS_ENFORCE_IMAGE_SANITY = shouldEnforce;

    return;
};

void GlobalsManager::setDisplaySize(uint32_t width, uint32_t height)
{
    LAZARUS_PRIMARY_DISPLAY_WIDTH = width;
    LAZARUS_PRIMARY_DISPLAY_HEIGHT = height;
    
    return;
};

uint32_t GlobalsManager::getDisplayWidth()
{
    return LAZARUS_PRIMARY_DISPLAY_WIDTH;
};

uint32_t GlobalsManager::getDisplayHeight()
{
    return LAZARUS_PRIMARY_DISPLAY_HEIGHT;
};

bool GlobalsManager::getEnforceImageSanity()
{
    return LAZARUS_ENFORCE_IMAGE_SANITY;
};

void GlobalsManager::setMaxImageSize(uint32_t width, uint32_t height)
{
    LAZARUS_MAX_IMAGE_WIDTH = width;
    LAZARUS_MAX_IMAGE_HEIGHT = height;

    return;
};

uint32_t GlobalsManager::getMaxImageWidth() 
{
    return LAZARUS_MAX_IMAGE_WIDTH;
};

uint32_t GlobalsManager::getMaxImageHeight() 
{
    return LAZARUS_MAX_IMAGE_HEIGHT;
};

void GlobalsManager::setExecutionState(uint32_t state)
{
    LAZARUS_EXECUTION_STATUS = state;
    
    return;   
};

uint32_t GlobalsManager::getExecutionState()
{
    return LAZARUS_EXECUTION_STATUS;
};

void GlobalsManager::setCursorHidden(bool shouldHide)
{
    LAZARUS_DISABLE_CURSOR_VISIBILITY = shouldHide;

    return;
};

bool GlobalsManager::getCursorHidden()
{
    return LAZARUS_DISABLE_CURSOR_VISIBILITY;
};

void GlobalsManager::setBackFaceCulling(bool shouldCull)
{
    LAZARUS_CULL_BACK_FACES = shouldCull;

    return;
};

bool GlobalsManager::getBackFaceCulling()
{
    return LAZARUS_CULL_BACK_FACES;
};

void GlobalsManager::setDepthTest(bool shouldTest)
{
    LAZARUS_DEPTH_TEST_FRAGS = shouldTest;

    return;
};

bool GlobalsManager::getDepthTest()
{
    return LAZARUS_DEPTH_TEST_FRAGS;
};

void GlobalsManager::setLaunchInFullscreen(bool shouldEnlarge)
{
    LAZARUS_LAUNCH_IN_FULLSCREEN = shouldEnlarge;

    return;
};

bool GlobalsManager::getLaunchInFullscreen()
{
    return LAZARUS_LAUNCH_IN_FULLSCREEN;
};

void GlobalsManager::setVsyncDisabled(bool shouldDisable)
{
    LAZARUS_DISABLE_VSYNC = shouldDisable;

    return;
};

bool GlobalsManager::getVsyncDisabled()
{
    return LAZARUS_DISABLE_VSYNC;
};

void GlobalsManager::setNumberOfActiveLights(uint32_t count)
{
    if(count < LAZARUS_MAX_LIGHTS)
    {
        LAZARUS_LIGHT_COUNT = count;
    };

    return;
};

uint32_t GlobalsManager::getNumberOfActiveLights()
{
    return LAZARUS_LIGHT_COUNT;
};

void GlobalsManager::setManageStencilBuffer(bool shouldManage)
{
    LAZARUS_DO_STENCIL_BUFFER = shouldManage;

    return;
};

bool GlobalsManager::getManageStencilBuffer()
{
    return LAZARUS_DO_STENCIL_BUFFER;
};

uint32_t GlobalsManager::getNumberOfPickableEntities()
{
    return LAZARUS_SELECTABLE_ENTITIES.size();
};

void GlobalsManager::setPickableEntity(uint32_t entityId)
{
    if(LAZARUS_SELECTABLE_ENTITIES.size() < LAZARUS_MAX_SELECTABLE_ENTITIES)
    {
        LAZARUS_SELECTABLE_ENTITIES.push_back(entityId);
    };

    return;
};

uint32_t GlobalsManager::getPickableEntity(uint32_t index)
{
    return LAZARUS_SELECTABLE_ENTITIES[index - 1];
};