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

        /* ==================================================================================================================================================
                                            +x                    -x                    -y                  +y                    +z                    -z
        ===================================================================================================================================================== */
        SkyBox createSkyBox(std::string rightPath, std::string leftPath, std::string downPath, std::string upPath, std::string frontPath, std::string backPath);
        void drawSkyBox(SkyBox sky, CameraManager::Camera camera);

        virtual ~WorldFX();

    private:
        void loadSkyMap();

        SkyBox skyBox;

        // std::unique_ptr<MeshManager> meshLoader;
        std::unique_ptr<FileReader> imageLoader;
        // std::unique_ptr<TextureLoader> textureLoader;

        GlobalsManager globals;

        GLuint shader;
};

#endif