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

class ModelManager 
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
        enum ModelType 
        {
            GLB = 1,
            WAVEFRONT = 2,
            PLANE = 3,
            CUBE = 4
        };
        struct Model
        {
            struct Instance
            {
                //  TODO:
                //  Add a visibility flag that can be used to 
                //  toggle whether frags should be discarded or not

                uint32_t id;
                glm::vec3 position;
                glm::vec3 direction;
                glm::vec3 scale;

                glm::mat4 modelMatrix;

                bool isClickable;
                bool isVisible;
            };

            uint32_t id;
            std::string name;

            ModelType type;
            std::vector<Material> materials;

            uint32_t numOfVertices;
            uint32_t numOfFaces;
            
            std::string meshFilepath;
            std::string materialFilepath;

            std::map<uint32_t, Instance> instances;
        };
        struct AssetConfig
        {
            std::string meshPath = "";
            std::string materialPath = "";
            std::string name = "";
            uint32_t instanceCount = 1;
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
            uint32_t instanceCount = 1;
            bool selectable = false;
        };
        struct CubeConfig
        {
            std::string name = "CUBE_";
            std::string texturePath = "";
            float scale = 1.0f;
            uint32_t instanceCount = 1;
            bool selectable = false;
        };
        
		ModelManager(GLuint shader, TextureLoader::StorageType textureType = TextureLoader::StorageType::ARRAY);
		
        lazarus_result create3DAsset(Model &out, AssetConfig options);
        lazarus_result createQuad(Model &out, QuadConfig options);
        lazarus_result createCube(Model &out, CubeConfig options);

        lazarus_result loadModel(Model &meshIn);
        lazarus_result drawModel(Model &meshIn);
        
        //  TODO:
        //  This isn't quite the appropriate place to do this
        //  but ok for now
        
        void setDiscardFragments(Model &meshIn, bool shouldDiscard);
        void copyModel(Model &dest, Model src);
        
        virtual ~ModelManager();
        
        // protected:
        lazarus_result clearMeshStorage();
        
    private:
        struct MeshData
        {
            uint32_t id;
            uint32_t instanceCount;
            uint8_t stencilBufferId;
            
            GLuint VAO;     //  Vertex Array Object
            GLuint VBO;     //  Vertex Buffer Object (attributes: interleaved)
            GLuint EBO;     //  Element Buffer Object (indices: tightly-packed)
            GLuint MBO;     //  Matrice Buffer Object (per-instance matrix: tightly-packed)
            GLuint IIBO;    //  Instance-info Buffer Object (per-instance: tightly-packed -> will probably end up interleaved)
            
            ModelType type;
            TextureLoader::TextureData texture;
            
            std::vector<FileLoader::Image> images;
            std::vector<uint32_t> indexes;
            std::vector<glm::vec3> attributes;
        };
        typedef std::vector<MeshData> ModelData;

        lazarus_result setMaterials(AssetLoader::AssetData &assetData);
        lazarus_result setSelectable(bool selectable);
        lazarus_result uploadVertexData();
        lazarus_result uploadTextures();
        lazarus_result reallocateTextures();
        lazarus_result checkErrors(const char *file, uint32_t line);
        
        void clearErrors();
        void instantiateMesh(bool selectable);
        void setSharedProperties();
        
        uint32_t childCount;

        int32_t errorCode;

        uint32_t maxTexWidth;
        uint32_t maxTexHeight;

		GLuint shaderProgram;
        GLint meshVariantLocation;
        GLint discardFragsLocation;

        std::unique_ptr<FileLoader> finder;
        TextureLoader::StorageType textureStorage;

        /* ====================================
            Convert std::map to std::set once
            instanced rendering is available.
        ======================================= */

        Model modelOut;
        MeshData meshData;
        ModelData modelData;
        std::map<uint32_t, ModelData> modelStore;
};

#endif