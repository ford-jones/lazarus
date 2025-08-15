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

#ifndef LAZARUS_COMMON_H
#define LAZARUS_COMMON_H

#define GREEN_TEXT "\x1b[32m"
#define BLUE_TEXT "\x1b[34m"
#define RESET_TEXT "\x1b[37m"
#define RED_TEXT  "\x1b[31m"


/* Engine Codes */

extern uint32_t                  LAZARUS_EXECUTION_STATUS;
extern uint32_t                  LAZARUS_PRIMARY_DISPLAY_WIDTH;
extern uint32_t                  LAZARUS_PRIMARY_DISPLAY_HEIGHT;
extern uint32_t                  LAZARUS_LIGHT_COUNT;
extern std::vector<uint32_t>     LAZARUS_SELECTABLE_ENTITIES;
extern const uint32_t            LAZARUS_MAX_LIGHTS;
extern const uint32_t            LAZARUS_MAX_SELECTABLE_ENTITIES;

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

extern uint32_t                  LAZARUS_LISTENER_KEYCODE;
extern uint32_t                  LAZARUS_LISTENER_SCANCODE;
extern uint32_t                  LAZARUS_LISTENER_MOUSECODE;
extern float                     LAZARUS_LISTENER_MOUSEX;
extern float                     LAZARUS_LISTENER_MOUSEY;
extern float                     LAZARUS_LISTENER_SCROLLCODE;


/* Placeholders */

extern const uint32_t            LAZARUS_MOUSE_NOCLICK;

extern const char*               LAZARUS_SKYBOX_CUBE;
extern const char*               LAZARUS_GLYPH_QUAD;
extern const char*               LAZARUS_PRIMITIVE_MESH;
extern const char*               LAZARUS_TEXTURED_MESH;
extern const char*               LAZARUS_DIFFUSE_MESH;

enum StatusCode
{
    LAZARUS_OK                             = 0,
    LAZARUS_FILE_NOT_FOUND                 = 101,
    LAZARUS_FILE_UNREADABLE                = 102,
    LAZARUS_FILESTREAM_CLOSED              = 103,
    LAZARUS_IMAGE_LOAD_FAILURE             = 104,
    LAZARUS_IMAGE_RESIZE_FAILURE           = 105,
    LAZARUS_FT_INIT_FAILURE                = 106,
    LAZARUS_FT_LOAD_FAILURE                = 107,
    LAZARUS_FT_RENDER_FAILURE              = 108,
    LAZARUS_SHADER_ERROR                   = 201,
    LAZARUS_VSHADER_COMPILE_FAILURE        = 202,
    LAZARUS_FSHADER_COMPILE_FAILURE        = 203,
    LAZARUS_SHADER_LINKING_FAILURE         = 204,
    LAZARUS_UNIFORM_NOT_FOUND              = 205,
    LAZARUS_MATRIX_LOCATION_ERROR          = 206,
    LAZARUS_OPENGL_ERROR                   = 301,
    LAZARUS_NO_CONTEXT                     = 302,
    LAZARUS_WINDOW_ERROR                   = 303,
    LAZARUS_EVENT_ERROR                    = 304,
    LAZARUS_GLFW_NOINIT                    = 305,
    LAZARUS_WIN_EXCEEDS_MAX                = 306,
    LAZARUS_TIME_ERROR                     = 307,
    LAZARUS_AUDIO_ERROR                    = 401,
    LAZARUS_AUDIO_PLAYBACK_POSITION_ERROR  = 402,
    LAZARUS_AUDIO_LOAD_ERROR               = 403,
    LAZARUS_INVALID_RADIANS                = 501,
    LAZARUS_INVALID_CUBEMAP                = 502,
    LAZARUS_INVALID_COORDINATE             = 503,
    LAZARUS_INVALID_DIMENSIONS             = 504,
    LAZARUS_INVALID_INTENSITY              = 505,
    LAZARUS_FEATURE_DISABLED               = 506
};

class GlobalsManager
{
    public:        
        void setMaxImageSize(uint32_t width, uint32_t height);
        uint32_t getMaxImageWidth();
        uint32_t getMaxImageHeight();

        void setEnforceImageSanity(bool shouldEnforce);
        bool getEnforceImageSanity();

        void setCursorHidden(bool shouldHide);
        bool getCursorHidden();

        void setBackFaceCulling(bool shouldCull);
        bool getBackFaceCulling();

        void setDepthTest(bool shouldTest);
        bool getDepthTest();

        void setLaunchInFullscreen(bool shouldEnlarge);
        bool getLaunchInFullscreen();

        void setExecutionState(uint32_t state);
        uint32_t getExecutionState();

        void setVsyncDisabled(bool shouldDisable);
        bool getVsyncDisabled();

        void setNumberOfActiveLights(uint32_t count);
        uint32_t getNumberOfActiveLights();

        void setDisplaySize(uint32_t width, uint32_t height);
        uint32_t getDisplayWidth();
        uint32_t getDisplayHeight();

        void setManageStencilBuffer(bool shouldManage);
        bool getManageStencilBuffer();

        uint32_t getNumberOfPickableEntities();

        void setPickableEntity(uint32_t entityId);
        uint8_t getPickableEntity(uint8_t index);
};

#endif