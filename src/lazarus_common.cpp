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

int                 LAZARUS_EXECUTION_STATUS               = 0;
int                 LAZARUS_PRIMARY_DISPLAY_WIDTH          = 0;
int                 LAZARUS_PRIMARY_DISPLAY_HEIGHT         = 0;
int                 LAZARUS_LIGHT_COUNT                    = 0;
std::vector<int>    LAZARUS_SELECTABLE_ENTITIES            = {};
const int           LAZARUS_MAX_LIGHTS                     = 150;
const int           LAZARUS_MAX_SELECTABLE_ENTITIES        = 254;

int                 LAZARUS_MAX_IMAGE_WIDTH                = 0;
int                 LAZARUS_MAX_IMAGE_HEIGHT               = 0;
bool                LAZARUS_ENFORCE_IMAGE_SANITY           = false;
bool                LAZARUS_DISABLE_CURSOR_VISIBILITY      = false;
bool                LAZARUS_LAUNCH_IN_FULLSCREEN           = false;
bool                LAZARUS_DISABLE_VSYNC                  = false;
bool                LAZARUS_DO_STENCIL_BUFFER              = false;
bool                LAZARUS_CULL_BACK_FACES                = true;
bool                LAZARUS_DEPTH_TEST_FRAGS               = true;

int                 LAZARUS_LISTENER_KEYCODE               = 0;
int                 LAZARUS_LISTENER_SCANCODE              = 0;
int                 LAZARUS_LISTENER_MOUSECODE             = 0;
double              LAZARUS_LISTENER_MOUSEX                = 0;
double              LAZARUS_LISTENER_MOUSEY                = 0;
double              LAZARUS_LISTENER_SCROLLCODE            = 0;
const int           LAZARUS_MOUSE_NOCLICK                  = 999;

const char*         LAZARUS_SKYBOX_CUBE                    = "Skybox target.";
const char*         LAZARUS_GLYPH_QUAD                     = "Glyph target.";
const char*         LAZARUS_PRIMITIVE_MESH                 = "Primitive mesh.";
const char*         LAZARUS_TEXTURED_MESH                  = "Textured mesh.";
const char*         LAZARUS_DIFFUSE_MESH                   = "Diffuse colored mesh.";

const int           LAZARUS_OK                             = 0;
const int           LAZARUS_FILE_NOT_FOUND                 = 101;
const int           LAZARUS_FILE_UNREADABLE                = 102;
const int           LAZARUS_FILESTREAM_CLOSED              = 103;
const int           LAZARUS_IMAGE_LOAD_FAILURE             = 104;
const int           LAZARUS_IMAGE_RESIZE_FAILURE           = 105;
const int           LAZARUS_FT_INIT_FAILURE                = 106;
const int           LAZARUS_FT_LOAD_FAILURE                = 107;
const int           LAZARUS_FT_RENDER_FAILURE              = 108;
const int           LAZARUS_SHADER_ERROR                   = 201;
const int           LAZARUS_VSHADER_COMPILE_FAILURE        = 202;
const int           LAZARUS_FSHADER_COMPILE_FAILURE        = 203;
const int           LAZARUS_SHADER_LINKING_FAILURE         = 204;
const int           LAZARUS_UNIFORM_NOT_FOUND              = 205;
const int           LAZARUS_MATRIX_LOCATION_ERROR          = 206;
const int           LAZARUS_OPENGL_ERROR                   = 301;
const int           LAZARUS_NO_CONTEXT                     = 302;
const int           LAZARUS_WINDOW_ERROR                   = 303;
const int           LAZARUS_GLFW_NOINIT                    = 304;
const int           LAZARUS_WIN_EXCEEDS_MAX                = 305;
const int           LAZARUS_TIME_ERROR                     = 306;
const int           LAZARUS_AUDIO_ERROR                    = 401;
const int           LAZARUS_AUDIO_PLAYBACK_POSITION_ERROR  = 402;
const int           LAZARUS_AUDIO_LOAD_ERROR               = 403;
const int           LAZARUS_INVALID_RADIANS                = 501;
const int           LAZARUS_INVALID_CUBEMAP                = 502;
const int           LAZARUS_INVALID_COORDINATE             = 503;
const int           LAZARUS_INVALID_DIMENSIONS             = 504;
const int           LAZARUS_INVALID_INTENSITY              = 505;
const int           LAZARUS_FEATURE_DISABLED               = 506;

void GlobalsManager::setEnforceImageSanity(bool shouldEnforce)
{
    LAZARUS_ENFORCE_IMAGE_SANITY = shouldEnforce;

    return;
};

void GlobalsManager::setDisplaySize(int width, int height)
{
    LAZARUS_PRIMARY_DISPLAY_WIDTH = width;
    LAZARUS_PRIMARY_DISPLAY_HEIGHT = height;
    
    return;
};

int GlobalsManager::getDisplayWidth()
{
    return LAZARUS_PRIMARY_DISPLAY_WIDTH;
};

int GlobalsManager::getDisplayHeight()
{
    return LAZARUS_PRIMARY_DISPLAY_HEIGHT;
};

bool GlobalsManager::getEnforceImageSanity()
{
    return LAZARUS_ENFORCE_IMAGE_SANITY;
};

void GlobalsManager::setMaxImageSize(int width, int height)
{
    LAZARUS_MAX_IMAGE_WIDTH = width;
    LAZARUS_MAX_IMAGE_HEIGHT = height;

    return;
};

int GlobalsManager::getMaxImageWidth() 
{
    return LAZARUS_MAX_IMAGE_WIDTH;
};

int GlobalsManager::getMaxImageHeight() 
{
    return LAZARUS_MAX_IMAGE_HEIGHT;
};

void GlobalsManager::setExecutionState(int state)
{
    LAZARUS_EXECUTION_STATUS = state;
    
    return;   
};

int GlobalsManager::getExecutionState()
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
};

bool GlobalsManager::getVsyncDisabled()
{
    return LAZARUS_DISABLE_VSYNC;
};

void GlobalsManager::setNumberOfActiveLights(int count)
{
    if(count < LAZARUS_MAX_LIGHTS) LAZARUS_LIGHT_COUNT = count;
};

int GlobalsManager::getNumberOfActiveLights()
{
    return LAZARUS_LIGHT_COUNT;
};

void GlobalsManager::setManageStencilBuffer(bool shouldManage)
{
    LAZARUS_DO_STENCIL_BUFFER = shouldManage;
};

bool GlobalsManager::getManageStencilBuffer()
{
    return LAZARUS_DO_STENCIL_BUFFER;
};

int GlobalsManager::getNumberOfPickableEntities()
{
    return LAZARUS_SELECTABLE_ENTITIES.size();
};

void GlobalsManager::setPickableEntity(int entityId)
{
    if(LAZARUS_SELECTABLE_ENTITIES.size() < LAZARUS_MAX_SELECTABLE_ENTITIES) LAZARUS_SELECTABLE_ENTITIES.push_back(entityId);
};

int GlobalsManager::getPickableEntity(int index)
{
    return LAZARUS_SELECTABLE_ENTITIES[index - 1];
};