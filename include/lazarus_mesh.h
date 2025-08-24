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
    : private AssetLoader, public TextureLoader
{
    public:
        struct Mesh
        {
            uint32_t id;

            uint32_t numOfVertices;
            uint32_t numOfFaces;

            /* ===============================
            	TODO:
                - Material struct
            ================================== */
            
            std::string meshFilepath;
            std::string materialFilepath;
            std::string textureFilepath;

            glm::vec3 position;
            glm::vec3 direction;
            glm::vec3 scale;

            glm::mat4 modelMatrix;

            uint8_t is3D;
            uint8_t isGlyph;
            uint8_t isSkybox;
            bool isClickable;
        };
		
		MeshManager(GLuint shader, TextureLoader::StorageType textureType = TextureLoader::StorageType::ARRAY);
		
        Mesh create3DAsset(std::string meshPath, std::string materialPath = LAZARUS_TEXTURED_MESH, std::string texturePath = LAZARUS_DIFFUSE_MESH, bool selectable = false);
        Mesh createQuad(float width, float height, std::string texturePath = LAZARUS_DIFFUSE_MESH, float uvXL = 0.0, float uvXR = 0.0, float uvYU = 0.0, float uvYD = 0.0, bool selectable = false);
        Mesh createCube(float scale, std::string texturePath = LAZARUS_SKYBOX_CUBE, bool selectable = false);

        void clearMeshStorage();

        void loadMesh(Mesh &meshIn);
        void drawMesh(Mesh &meshIn);

        virtual ~MeshManager();
    private:
        struct MeshData
        {
            uint32_t id;

            int32_t stencilBufferId;
            int32_t textureUnit;

            FileLoader::Image textureData;
            GLuint textureId;
            GLuint textureLayer;
            GLuint VAO;
            GLuint VBO;
            GLuint EBO;

            std::vector<uint32_t> indexes;
            std::vector<glm::vec3> attributes;
            std::vector<glm::vec3> diffuse;
        };

        void resolveFilepaths(std::string texPath = LAZARUS_DIFFUSE_MESH, std::string mtlPath = LAZARUS_TEXTURED_MESH, std::string objPath = LAZARUS_PRIMITIVE_MESH);
        void setInherentProperties();
        void initialiseMesh();
        void makeSelectable(bool selectable);
        void prepareTextures();
        
        void checkErrors(const char *file, uint32_t line);
        void clearErrors();

        int32_t errorCode;
        int32_t layerCount;

        uint32_t maxTexWidth;
        uint32_t maxTexHeight;

		GLuint shaderProgram;
        GLint modelMatrixUniformLocation;
        GLint textureLayerUniformLocation;
        GLint is3DUniformLocation;
        GLint isGlyphUniformLocation;
        GLint isSkyBoxUniformLocation;

        std::unique_ptr<FileLoader> finder;
        
        /* ====================================
            Convert std::map to std::set once
            instanced rendering is available.
        ======================================= */

        Mesh meshOut;
        MeshData meshData;
        std::map<uint32_t, MeshData> dataStore;

        GlobalsManager globals;

};

#endif