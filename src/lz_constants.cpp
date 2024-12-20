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
//                                        
/*  LAZARUS ENGINE */

#include "../include/lz_constants.h"

int          LAZARUS_EXECUTION_STATUS               = 0;
int          LAZARUS_PRIMARY_DISPLAY_WIDTH          = 0;
int          LAZARUS_PRIMARY_DISPLAY_HEIGHT         = 0;
bool         LAZARUS_ENFORCE_IMAGE_SANITY           = false;
bool         LAZARUS_DISABLE_CURSOR_VISIBILITY      = false;
bool         LAZARUS_CULL_BACK_FACES                = true;
bool         LAZARUS_DEPTH_TEST_FRAGS               = true;
bool         LAZARUS_LAUNCH_IN_FULLSCREEN           = false;
int          LAZARUS_MAX_IMAGE_WIDTH                = 0;
int          LAZARUS_MAX_IMAGE_HEIGHT               = 0;
int          LAZARUS_LISTENER_KEYCODE               = 0;
int          LAZARUS_LISTENER_SCANCODE              = 0;
int          LAZARUS_LISTENER_MOUSECODE             = 0;
double       LAZARUS_LISTENER_MOUSEX                = 0;
double       LAZARUS_LISTENER_MOUSEY                = 0;
double       LAZARUS_LISTENER_SCROLLCODE            = 0;
const int    LAZARUS_MOUSE_NOCLICK                  = 999;
const char*  LAZARUS_MESH_ISSKYBOX                  = "This mesh is used to display a skybox.";
const char*  LAZARUS_MESH_ISTEXT                    = "This mesh is used to display a glyph.";
const char*  LAZARUS_MESH_NOOBJ                     = "Non-wavefront mesh.";
const char*  LAZARUS_MESH_NOMTL                     = "Colourless mesh.";
const char*  LAZARUS_MESH_NOTEX                     = "Textureless mesh.";
const int    LAZARUS_OK                             = 0;
const int    LAZARUS_FILE_NOT_FOUND                 = 101;
const int    LAZARUS_FILE_UNREADABLE                = 102;
const int    LAZARUS_FILESTREAM_CLOSED              = 103;
const int    LAZARUS_IMAGE_LOAD_FAILURE             = 104;
const int    LAZARUS_IMAGE_RESIZE_FAILURE           = 105;
const int    LAZARUS_FT_INIT_FAILURE                = 106;
const int    LAZARUS_FT_LOAD_FAILURE                = 107;
const int    LAZARUS_FT_RENDER_FAILURE              = 108;
const int    LAZARUS_SHADER_ERROR                   = 201;
const int    LAZARUS_VSHADER_COMPILE_FAILURE        = 202;
const int    LAZARUS_FSHADER_COMPILE_FAILURE        = 203;
const int    LAZARUS_SHADER_LINKING_FAILURE         = 204;
const int    LAZARUS_UNIFORM_NOT_FOUND              = 205;
const int    LAZARUS_MATRIX_LOCATION_ERROR          = 206;
const int    LAZARUS_OPENGL_ERROR                   = 301;
const int    LAZARUS_NO_CONTEXT                     = 302;
const int    LAZARUS_WINDOW_ERROR                   = 303;
const int    LAZARUS_GLFW_NOINIT                    = 304;
const int    LAZARUS_WIN_EXCEEDS_MAX                = 305;
const int    LAZARUS_AUDIO_ERROR                    = 401;
const int    LAZARUS_AUDIO_PLAYBACK_POSITION_ERROR  = 402;
const int    LAZARUS_INVALID_RADIANS                = 501;
const int    LAZARUS_INVALID_CUBEMAP                = 502;