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
			
            vec3 position;                                                                //  The (x,y,z) location of the camera                                                                  
            vec3 direction;                                                                     //  where the camera is looking                             
            vec3 upVector;

            float aspectRatio;                                                                  //  The viewport aspect ratio

            GLuint viewLocation;                                                                //  The location / index of the view matrix inside the vert shader program
            GLuint projectionLocation;                                                          //  The location / index of the projection matrix inside the vert shader program

            mat4 viewMatrix;                                                                    //  A view matrix matrice passed into the shader program as a uniform
            mat4 projectionMatrix;                                                              //  A projection matrix matrice passed into the shader program as a uniform

            int usesPerspective;
        };
		
        Camera createPerspectiveCam(int arX = 0, int arY = 0);
        Camera createOrthoCam(int arX, int arY);
        void loadCamera(Camera &cameraData);

        virtual ~CameraManager();

    private:
        GlobalsManager globals;
        int monitorX;
        int monitorY;

        GLuint shader;
        Camera camera;
};

#endif
