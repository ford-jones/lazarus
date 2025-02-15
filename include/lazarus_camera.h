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

#ifndef LAZARUS_GL_INCLUDES_H
    #include "lazarus_gl_includes.h"
#endif

#ifndef LAZARUS_COMMON_H
    #include "lazarus_common.h"
#endif

#include <iostream>
#include <stdlib.h>
#include <memory>

using std::shared_ptr;
using glm::vec3;
using glm::mat4;

#ifndef LAZARUS_CAMERA_H
#define LAZARUS_CAMERA_H

class CameraManager
{
    public:
        CameraManager(GLuint shader);
        struct Camera
        {
            int id;
			
            vec3 position;
            vec3 direction;
            vec3 upVector;

            float aspectRatio;

            mat4 viewMatrix;
            mat4 projectionMatrix;

            int usesPerspective;
        };
		
        Camera createPerspectiveCam(int arX = 0, int arY = 0);
        Camera createOrthoCam(int arX, int arY);
        void loadCamera(Camera &cameraData);

        virtual ~CameraManager();

    private:
        GLuint viewLocation;
        GLuint perspectiveProjectionLocation;
        GLuint orthographicProjectionLocation;
        GlobalsManager globals;
        int monitorX;
        int monitorY;

        GLuint shader;
        Camera camera;
};

#endif
