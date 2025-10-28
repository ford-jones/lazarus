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
#include <iostream>
#include <vector>
#include <chrono>

#ifndef LAZARUS_COMMON_H
#define LAZARUS_COMMON_H

#define GREEN_TEXT "\x1b[32m"
#define BLUE_TEXT "\x1b[34m"
#define RESET_TEXT "\x1b[37m"
#define RED_TEXT  "\x1b[31m"

extern void LOG_DEBUG(const char *DEBUG_MESSAGE);
extern void LOG_ERROR(const char *ERR_MESSAGE, const char *ERR_FILENAME, uint32_t ERR_LINE);

/* Engine Codes */

extern uint32_t                  LAZARUS_EXECUTION_STATUS;
extern uint32_t                  LAZARUS_PRIMARY_DISPLAY_WIDTH;
extern uint32_t                  LAZARUS_PRIMARY_DISPLAY_HEIGHT;
extern uint32_t                  LAZARUS_LIGHT_COUNT;
extern std::vector<uint32_t>     LAZARUS_SELECTABLE_ENTITIES;

/* User Settings */

extern bool                      LAZARUS_ENFORCE_IMAGE_SANITY;
extern bool                      LAZARUS_DISABLE_CURSOR_VISIBILITY;
extern bool                      LAZARUS_CULL_BACK_FACES;
extern bool                      LAZARUS_DEPTH_TEST_FRAGS;
extern bool                      LAZARUS_LAUNCH_IN_FULLSCREEN;
extern bool                      LAZARUS_DISABLE_VSYNC;
extern bool                      LAZARUS_DO_STENCIL_BUFFER;
extern uint32_t                  LAZARUS_MAX_IMAGE_WIDTH;
extern uint32_t                  LAZARUS_MAX_IMAGE_HEIGHT;

/* Events */

extern int32_t                  LAZARUS_LISTENER_KEYCODE;
extern int32_t                  LAZARUS_LISTENER_SCANCODE;
extern int32_t                  LAZARUS_LISTENER_MOUSECODE;
extern float                     LAZARUS_LISTENER_MOUSEX;
extern float                     LAZARUS_LISTENER_MOUSEY;
extern float                     LAZARUS_LISTENER_SCROLLCODE;


/* Placeholders */

extern const uint32_t            LAZARUS_MOUSE_NOCLICK;

typedef enum lazarus_result
{
    //  good

    LAZARUS_OK                             = 0,

    //  files

    LAZARUS_FILE_NOT_FOUND                 = 101,
    LAZARUS_FILE_UNREADABLE                = 102,
    LAZARUS_FILESTREAM_CLOSED              = 103,
    LAZARUS_IMAGE_LOAD_FAILURE             = 104,
    LAZARUS_IMAGE_RESIZE_FAILURE           = 105,

    //  shaders

    LAZARUS_SHADER_ERROR                   = 201,
    LAZARUS_VSHADER_COMPILE_FAILURE        = 202,
    LAZARUS_FSHADER_COMPILE_FAILURE        = 203,
    LAZARUS_SHADER_LINKING_FAILURE         = 204,
    LAZARUS_UNIFORM_NOT_FOUND              = 205,
    LAZARUS_MATRIX_LOCATION_ERROR          = 206,

    //  gl ecosystem

    LAZARUS_OPENGL_ERROR                   = 301,
    LAZARUS_NO_CONTEXT                     = 302,
    LAZARUS_WINDOW_ERROR                   = 303,
    LAZARUS_EVENT_ERROR                    = 304,
    LAZARUS_GLFW_NOINIT                    = 305,
    LAZARUS_WIN_EXCEEDS_MAX                = 306,
    LAZARUS_TIME_ERROR                     = 307,

    //  audio

    LAZARUS_AUDIO_ERROR                    = 401,
    LAZARUS_AUDIO_PLAYBACK_POSITION_ERROR  = 402,
    LAZARUS_AUDIO_LOAD_ERROR               = 403,

    //  input validation

    LAZARUS_INVALID_RADIANS                = 501,
    LAZARUS_INVALID_CUBEMAP                = 502,
    LAZARUS_INVALID_COORDINATE             = 503,
    LAZARUS_INVALID_DIMENSIONS             = 504,
    LAZARUS_INVALID_INTENSITY              = 505,
    LAZARUS_FEATURE_DISABLED               = 506,

    //  assets

    LAZARUS_ASSET_LOAD_ERROR               = 601,
    LAZARUS_LIMIT_REACHED                  = 602,

    //  text
    
    LAZARUS_FT_INIT_FAILURE                = 701,
    LAZARUS_FT_LOAD_FAILURE                = 702,
    LAZARUS_FT_RENDER_FAILURE              = 703
} lazarus_result;

class GlobalsManager
{
    public:        
        static void setMaxImageSize(uint32_t width, uint32_t height);
        static uint32_t getMaxImageWidth();
        static uint32_t getMaxImageHeight();

        static void setEnforceImageSanity(bool shouldEnforce);
        static bool getEnforceImageSanity();

        static void setCursorHidden(bool shouldHide);
        static bool getCursorHidden();

        static void setBackFaceCulling(bool shouldCull);
        static bool getBackFaceCulling();

        static void setDepthTest(bool shouldTest);
        static bool getDepthTest();

        static void setLaunchInFullscreen(bool shouldEnlarge);
        static bool getLaunchInFullscreen();

        static void setVsyncDisabled(bool shouldDisable);
        static bool getVsyncDisabled();

        static void setNumberOfActiveLights(uint8_t count);
        static uint8_t getNumberOfActiveLights();

        static void setDisplaySize(uint32_t width, uint32_t height);
        static uint32_t getDisplayWidth();
        static uint32_t getDisplayHeight();

        static void setManageStencilBuffer(bool shouldManage);
        static bool getManageStencilBuffer();

        static uint32_t getNumberOfPickableEntities();

        static void setPickableEntity(uint32_t entityId);
        static uint8_t getPickableEntity(uint8_t index);
};

#endif