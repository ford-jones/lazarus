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

#ifndef LAZARUS_COMMON_H
#define LAZARUS_COMMON_H

#define GREEN_TEXT "\x1b[32m"
#define BLUE_TEXT "\x1b[34m"
#define RESET_TEXT "\x1b[37m"
#define RED_TEXT  "\x1b[31m"


/* Engine Status */

extern int          LAZARUS_EXECUTION_STATUS;
extern int          LAZARUS_PRIMARY_DISPLAY_WIDTH;
extern int          LAZARUS_PRIMARY_DISPLAY_HEIGHT;
extern int          LAZARUS_LIGHT_COUNT;

/* User Settings */

extern bool         LAZARUS_ENFORCE_IMAGE_SANITY;
extern bool         LAZARUS_DISABLE_CURSOR_VISIBILITY;
extern bool         LAZARUS_CULL_BACK_FACES;
extern bool         LAZARUS_DEPTH_TEST_FRAGS;
extern bool         LAZARUS_LAUNCH_IN_FULLSCREEN;
extern bool         LAZARUS_DISABLE_VSYNC;
extern int          LAZARUS_MAX_IMAGE_WIDTH;
extern int          LAZARUS_MAX_IMAGE_HEIGHT;

/* Events */

extern int          LAZARUS_LISTENER_KEYCODE;
extern int          LAZARUS_LISTENER_SCANCODE;
extern int          LAZARUS_LISTENER_MOUSECODE;
extern double       LAZARUS_LISTENER_MOUSEX;
extern double       LAZARUS_LISTENER_MOUSEY;
extern double       LAZARUS_LISTENER_SCROLLCODE;
  
/* Placeholders */

extern const int    LAZARUS_MOUSE_NOCLICK;

extern const char*  LAZARUS_SKYBOX_CUBE;
extern const char*  LAZARUS_GLYPH_QUAD;
extern const char*  LAZARUS_PRIMITIVE_MESH;
extern const char*  LAZARUS_TEXTURED_MESH;
extern const char*  LAZARUS_DIFFUSE_MESH;
    
/* Status Codes */

extern const int    LAZARUS_OK;
extern const int    LAZARUS_FILE_NOT_FOUND;
extern const int    LAZARUS_FILE_UNREADABLE;
extern const int    LAZARUS_FILESTREAM_CLOSED;
extern const int    LAZARUS_IMAGE_LOAD_FAILURE;
extern const int    LAZARUS_IMAGE_RESIZE_FAILURE;
extern const int    LAZARUS_FT_INIT_FAILURE;
extern const int    LAZARUS_FT_LOAD_FAILURE;
extern const int    LAZARUS_FT_RENDER_FAILURE;
extern const int    LAZARUS_SHADER_ERROR;
extern const int    LAZARUS_VSHADER_COMPILE_FAILURE;
extern const int    LAZARUS_FSHADER_COMPILE_FAILURE;
extern const int    LAZARUS_SHADER_LINKING_FAILURE;
extern const int    LAZARUS_UNIFORM_NOT_FOUND;
extern const int    LAZARUS_MATRIX_LOCATION_ERROR;
extern const int    LAZARUS_OPENGL_ERROR;
extern const int    LAZARUS_NO_CONTEXT;
extern const int    LAZARUS_WINDOW_ERROR;
extern const int    LAZARUS_GLFW_NOINIT;
extern const int    LAZARUS_WIN_EXCEEDS_MAX;
extern const int    LAZARUS_TIME_ERROR;
extern const int    LAZARUS_AUDIO_ERROR;
extern const int    LAZARUS_AUDIO_PLAYBACK_POSITION_ERROR;
extern const int    LAZARUS_AUDIO_LOAD_ERROR;
extern const int    LAZARUS_INVALID_RADIANS;
extern const int    LAZARUS_INVALID_CUBEMAP;
extern const int    LAZARUS_INVALID_COORDINATE;
extern const int    LAZARUS_INVALID_DIMENSIONS;
extern const int    LAZARUS_INVALID_INTENSITY;

class GlobalsManager
{
    public:
        void setMaxImageSize(int width, int height);
        int getMaxImageWidth();
        int getMaxImageHeight();

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

        void setExecutionState(int state);
        int getExecutionState();

        void setVsyncDisabled(bool shouldDisable);
        bool getVsyncDisabled();

        void setNumberOfActiveLights(int count);
        int getNumberOfActiveLights();

        void setDisplaySize(int width, int height);
        int getDisplayWidth();
        int getDisplayHeight();
};

#endif