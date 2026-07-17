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

#include <iostream>
#include <memory>
#include <string>

#include "lazarus_file_loader.h"
#include "lazarus_mesh.h"
#include "lazarus_camera.h"
#include "lazarus_shader.h"

#ifndef LAZARUS_WORLD_FX_H
#define LAZARUS_WORLD_FX_H

class WorldFX : private ModelManager
{
    public:
        WorldFX(Shader &shader);
        struct Skybox
        {
            std::vector<string> paths;
            std::vector<FileLoader::Image> cubeMap;

            ModelManager::Model cube;
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
        lazarus_result loadFog(Fog fogIn);

        virtual ~WorldFX();

    private:
        lazarus_result updateUniformLocations();
        lazarus_result loadSkyMap();

        GLuint activeShaderID;
        
        GLint viewMatrixLocation;
        GLint fogColorUniformLocation;
        GLint fogViewpointUniformLocation;
        GLint fogMaxDistUniformLocation;
        GLint fogMinDistUniformLocation;
        GLint fogDensityUniformLocation;

        Skybox skyBoxOut;
        Fog fogOut;
        Shader *shader;
        std::unique_ptr<FileLoader> imageLoader;

        int32_t error;
};

#endif