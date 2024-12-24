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

#ifndef LAZARUS_GL_INCLUDES_H
    #include "lazarus_gl_includes.h"
#endif

#include "lazarus_globals_manager.h"
#include "lazarus_window_manager.h"
#include "lazarus_event_manager.h"
#include "lazarus_mesh.h"
#include "lazarus_transforms.h"
#include "lazarus_shader.h"
#include "lazarus_light.h"
#include "lazarus_camera.h"
#include "lazarus_fps_counter.h"
#include "lazarus_file_reader.h"
#include "lazarus_audio_manager.h"
#include "lazarus_text_manager.h"
#include "lazarus_world_fx.h"

#ifndef LAZARUS_H
#define LAZARUS_H

namespace Lazarus
{
    using ::WorldFX;
    using ::TextManager;
    using ::GlobalsManager;
    using ::WindowManager;
    using ::EventManager;
    using ::MeshManager;
    using ::Transform;
    using ::Shader;
    using ::LightManager;
    using ::CameraManager;
    using ::FpsCounter;
    using ::FileReader;
    using ::AudioManager;
}

#endif
