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
#include <memory>
#include <string>

#include "lazarus_file_loader.h"
#include "lazarus_mesh.h"
#include "lazarus_camera.h"

#ifndef LAZARUS_WORLD_FX_H
#define LAZARUS_WORLD_FX_H

class WorldFX : private MeshManager
{
    public:
        WorldFX(GLuint shaderProgram);
        struct Skybox
        {
            std::vector<string> paths;
            std::vector<FileLoader::Image> cubeMap;

            MeshManager::Mesh cube;
        };

        struct Fog
        {
            glm::vec3 color;
            glm::vec3 viewpoint;

            float minDistance;
            float maxDistance;
            float density;
        };

        lazarus_result createSkyBox(Skybox &out, std::string rightPath, std::string leftPath, std::string downPath, std::string upPath, std::string frontPath, std::string backPath);
        lazarus_result drawSkyBox(Skybox skyboxIn, CameraManager::Camera camera);

        lazarus_result createFog(
            Fog &out,
            float minDistance, 
            float maxDistance, 
            float thickness, 
            glm::vec3 color,
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f)
        );
        lazarus_result loadFog(Fog fogIn, int32_t shader = 0);

        virtual ~WorldFX();

    private:
        lazarus_result loadSkyMap();

        GLuint shader;

        GLint fogColorUniformLocation;
        GLint fogViewpointUniformLocation;
        GLint fogMaxDistUniformLocation;
        GLint fogMinDistUniformLocation;
        GLint fogDensityUniformLocation;

        Skybox skyBoxOut;
        Fog fogOut;

        std::unique_ptr<FileLoader> imageLoader;

        int32_t error;
};

#endif