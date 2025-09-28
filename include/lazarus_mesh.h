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
#include <vector>
#include <map>
#include <string>
#include <stdlib.h>
#include <memory>
#include <cstring>
#include <algorithm>

#include "lazarus_file_loader.h"
#include "lazarus_asset_loader.h"
#include "lazarus_texture_loader.h"

using std::unique_ptr;
using std::string;
using std::vector;
using glm::mat4;
using glm::vec3;
using glm::vec2;

#ifndef LAZARUS_MESH_H
#define LAZARUS_MESH_H

class MeshManager 
    : private AssetLoader, protected TextureLoader
{
    public:
        enum MaterialType
        {
            IMAGE_TEXTURE = 1,
            BASE_COLOR = 2
        };
        struct Material
        {
            uint32_t id;
            MaterialType type;
            
            FileLoader::Image texture;
            glm::vec3 diffuse;
            
            bool discardsAlphaZero;
        };
        enum MeshType 
        {
            LOADED_GLB = 1,
            LOADED_WAVEFRONT = 2,
            PLANE = 3,
            CUBE = 4
        };
        struct Mesh
        {
            uint32_t id;
            std::string name;

            MeshType type;
            std::vector<Material> materials;

            uint32_t numOfVertices;
            uint32_t numOfFaces;
            
            std::string meshFilepath;
            std::string materialFilepath;

            glm::vec3 position;
            glm::vec3 direction;
            glm::vec3 scale;

            glm::mat4 modelMatrix;

            bool isClickable;
        };
        struct AssetConfig
        {
            std::string meshPath = "";
            std::string materialPath = "";
            std::string name = "";
            bool selectable = false;
        };
        struct QuadConfig
        {
            std::string name = "QUAD_";
            std::string texturePath = "";
            float width = 0.0f;
            float height = 0.0f;
            float uvXL = 0.0f;
            float uvXR = 0.0f;
            float uvYU = 0.0f;
            float uvYD = 0.0f;
            bool selectable = false;
        };
        struct CubeConfig
        {
            std::string name = "CUBE_";
            std::string texturePath = "";
            float scale = 1.0f;
            bool selectable = false;
        };
        
		MeshManager(GLuint shader, TextureLoader::StorageType textureType = TextureLoader::StorageType::ARRAY);
		
        // Mesh create3DAsset(std::string meshPath, std::string materialPath = "", bool selectable = false);
        // Mesh createQuad(float width, float height, std::string texturePath = "", float uvXL = 0.0, float uvXR = 0.0, float uvYU = 0.0, float uvYD = 0.0, bool selectable = false);
        // Mesh createCube(float scale, std::string texturePath = "", bool selectable = false);
        lazarus_result create3DAsset(Mesh &out, AssetConfig options);
        lazarus_result createQuad(Mesh &out, QuadConfig options);
        lazarus_result createCube(Mesh &out, CubeConfig options);

        lazarus_result loadMesh(Mesh &meshIn);
        lazarus_result drawMesh(Mesh &meshIn);

        //  TODO:
        //  This isn't quite the appropriate place to do this
        //  but ok for now

        void setDiscardFragments(Mesh &meshIn, bool shouldDiscard);

        virtual ~MeshManager();
    
    protected:
        lazarus_result clearMeshStorage();

    private:
        struct MeshData
        {
            uint32_t id;
            int32_t stencilBufferId;

            GLuint VAO;
            GLuint VBO;
            GLuint EBO;
            
            MeshType type;
            TextureLoader::TextureData texture;
            
            std::vector<FileLoader::Image> images;
            std::vector<uint32_t> indexes;
            std::vector<glm::vec3> attributes;
        };

        lazarus_result setMaterialProperties(std::vector<glm::vec3> diffuse, std::vector<FileLoader::Image> images);
        lazarus_result initialiseMesh();
        lazarus_result prepareTextures();
        void makeSelectable(bool selectable);
        void setSharedProperties();
        
        lazarus_result checkErrors(const char *file, uint32_t line);
        void clearErrors();

        int32_t errorCode;

        uint32_t maxTexWidth;
        uint32_t maxTexHeight;

		GLuint shaderProgram;
        GLint modelMatrixUniformLocation;
        GLint meshVariantLocation;
        GLint discardFragsLocation;

        std::unique_ptr<FileLoader> finder;
        TextureLoader::StorageType textureStorage;

        /* ====================================
            Convert std::map to std::set once
            instanced rendering is available.
        ======================================= */

        Mesh meshOut;
        MeshData meshData;
        std::map<uint32_t, MeshData> dataStore;
};

#endif