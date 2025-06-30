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

#include "lazarus_texture_loader.h"
#include "lazarus_file_reader.h"
#include "lazarus_mesh.h"
#include "lazarus_camera.h"

#ifndef LAZARUS_WORLD_FX_H
#define LAZARUS_WORLD_FX_H

class WorldFX : private MeshManager
{
    public:
        WorldFX(GLuint shaderProgram);

        struct SkyBox
        {
            std::vector<std::string> paths;
            std::vector<FileReader::Image> cubeMap;

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

        /* ==================================================================================================================================================
                                            +x                    -x                    -y                  +y                    +z                    -z
        ===================================================================================================================================================== */
        SkyBox createSkyBox(
            std::string rightPath, 
            std::string leftPath, 
            std::string downPath, 
            std::string upPath, 
            std::string frontPath, 
            std::string backPath
        );
        void drawSkyBox(SkyBox skyboxIn, CameraManager::Camera camera);

        Fog createFog(
            float minDistance, 
            float maxDistance, 
            float thickness, 
            float r, 
            float g, 
            float b, 
            float x = 0.0f, 
            float y = 0.0f, 
            float z = 0.0f
        );
        void loadFog(Fog fogIn);

        virtual ~WorldFX();

    private:
        void loadSkyMap();

        GLuint shader;

        GLint fogColorUniformLocation;
        GLint fogViewpointUniformLocation;
        GLint fogMaxDistUniformLocation;
        GLint fogMinDistUniformLocation;
        GLint fogDensityUniformLocation;

        SkyBox skyBoxOut;
        Fog fogOut;
        GlobalsManager globals;
        std::unique_ptr<FileReader> imageLoader;

        int32_t status;
};

#endif