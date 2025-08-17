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
#include <string>

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
            uint32_t id;
			
            vec3 position;
            vec3 direction;
            vec3 upVector;

            float aspectRatio;

            mat4 viewMatrix;
            mat4 projectionMatrix;

            uint8_t usesPerspective;
        };
		
        Camera createPerspectiveCam(uint32_t aspectRatioX = 0, uint32_t aspectRatioY = 0);
        Camera createOrthoCam(uint32_t aspectRatioX  = 0, uint32_t aspectRatioY = 0);
        void loadCamera(Camera &cameraIn);
        
        uint8_t getPixelOccupant(uint32_t windowX, uint32_t windowY);

        virtual ~CameraManager();

    private:
        void setAspectRatio(uint32_t x, uint32_t y);
        void checkErrors(const char *file, uint32_t line);
        void clearErrors();

        int32_t errorCode;
        uint32_t pixelWidth;
        uint32_t pixelHeight;
        
        GLint pixel;
        
        GLuint shader;
        GLuint viewLocation;
        GLuint perspectiveProjectionLocation;
        GLuint orthographicProjectionLocation;
        
        GlobalsManager globals;
        Camera camera;
};

#endif
