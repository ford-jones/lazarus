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
        enum CameraType
        {
            ORTHOGRAPHIC,
            PERSPECTIVE_FLYING,
            // PERSPECTIVE_ORBIT /* TODO: Add orbital, toggle logic in transformer appropriately */
        };
        struct CameraConfig
        {
            std::string name = "CAMERA_";
            CameraType type = CameraType::ORTHOGRAPHIC;
            uint32_t aspectRatioX = 0;
            uint32_t aspectRatioY = 0;
            float clippingDistance = 0.0f;
        };
         struct Camera
        {
            uint32_t id;
			
            vec3 position;
            vec3 direction;
            vec3 upVector;

            CameraConfig config;

            mat4 viewMatrix;
            mat4 projectionMatrix;
        };
		
        lazarus_result createCamera(Camera &out, CameraConfig options);
        lazarus_result loadCamera(Camera &cameraIn);
        
        lazarus_result getPixelOccupant(uint32_t windowX, uint32_t windowY, uint8_t &out);

        virtual ~CameraManager();

    private:
        void setAspectRatio(uint32_t x, uint32_t y);
        lazarus_result checkErrors(const char *file, uint32_t line);
        lazarus_result clearErrors();

        float aspectRatio;
        int32_t errorCode;
        uint32_t pixelWidth;
        uint32_t pixelHeight;
        
        GLint pixel;
        
        GLuint shader;
        GLuint viewLocation;
        GLuint perspectiveProjectionLocation;
        GLuint orthographicProjectionLocation;
        GLuint projectionTypeLocation;
        
        Camera camera;
};

#endif
