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

/* =========================================
    Required for generating hashes for glm
    types.
============================================ */
#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <glm/gtx/hash.hpp>
#include <unordered_set>
#include <iterator>

#include "lazarus_file_loader.h"
#include "lazarus_texture_loader.h"

using std::unique_ptr;
using std::string;
using std::vector;
using glm::vec3;
using glm::vec2;
using std::ifstream;
using std::stringstream;

#ifndef LAZARUS_ASSET_LOADER_H
#define LAZARUS_ASSET_LOADER_H

class AssetLoader 
{
    protected:
        uint32_t layerCount;
    	AssetLoader();	
    	    
        struct AssetData
        {
            std::string name;
            std::vector<glm::vec3> attributes;
            std::vector<glm::vec3> colors;
            std::vector<uint32_t> indices;
            std::vector<FileLoader::Image> textures;
        };

        lazarus_result parseWavefrontObj(
            std::vector<AssetData> &out,
            const char *meshPath,
            const char *materialPath
        );

        lazarus_result parseWavefrontMtl(
            const char *materialPath,
            std::vector<std::vector<uint32_t>> data,
            std::vector<glm::vec3> &temp,
            std::vector<glm::vec3> &outColors,
            std::vector<FileLoader::Image> &outImages
        );

        lazarus_result parseGlBinary(
            std::vector<AssetData> &out,
            const char *meshPath
        );
        
        virtual ~AssetLoader();

    private:
        //  glb-related members

        std::string jsonData;
        std::string binaryData;

        //  Top level: mesh properties (note the square-bracket)
        const std::string NODES = "\"nodes\":[{";
        const std::string MESHES = "\"meshes\":[";
        const std::string SKINS = "\"skins\":[";
        const std::string PRIMITIVES = "\"primitives\":[";
        const std::string MATERIALS = "\"materials\":[";
        const std::string TEXTURES = "\"textures\":[";
        const std::string ACCESSORS = "\"accessors\":[";
        const std::string IMAGES = "\"images\":[";
        const std::string BUFFERVIEWS = "\"bufferViews\":[";
        const std::string BUFFERS = "\"buffers\":[{";

        //  Subsequent levels: property attributes
        const std::string MESH = "\"mesh\":";
        const std::string TRANSLATION = "\"translation\":";
        const std::string ROTATION = "\"rotation\":";
        const std::string MATERIALID = "\"material\":";
        const std::string TEXTUREID = "\"baseColorTexture\":";
        const std::string DIFFUSE = "\"baseColorFactor\":";
        const std::string INDEX = "\"index\":";
        const std::string ATTRIBUTES = "\"attributes\":";
        const std::string INDICES = "\"indices\":";
        const std::string SAMPLERID = "\"sampler\":";
        const std::string IMAGEID = "\"source\":";
        const std::string BUFFERVIEWID = "\"bufferView\":";
        const std::string BUFFERID = "\"buffer\":";
        const std::string COUNT = "\"count\":";
        const std::string COMPONENTTYPE = "\"componentType\":";
        const std::string BYTEOFFSET = "\"byteOffset\":";
        const std::string BYTELENGTH = "\"byteLength\":";
        const std::string BYTESTRIDE = "\"byteStride\":";
        const std::string NAME = "\"name\":";

        struct glbNodeData
        {
            std::string name;
            int32_t meshIndex;
            int32_t skinIndex;
            std::vector<int32_t> children;
            glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec4 rotation = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        };
        struct glbAttributeData
        {
            uint32_t positionAccessor;
            uint32_t normalsAccessor;
            uint32_t indicesAccessor;
            int32_t uvAccessor;
            uint32_t materialIndex;
        };
        struct glbMaterialData
        {
            glm::vec3 diffuse;
            int32_t textureIndex;
        };
        struct glbTextureData
        {
            uint32_t samplerIndex;
            uint32_t imageIndex;
        };
        struct glbImageData
        {
            uint32_t bufferViewIndex;
        };
        struct glbAccessorData
        {
            uint32_t count;
            uint32_t bufferViewIndex;
            int32_t byteOffset;
            std::uint32_t componentType;
            std::string type;
        };
        struct glbBufferViewData
        {
            uint32_t bufferIndex;
            uint32_t byteLength;
            uint32_t byteOffset;
            int32_t byteStride;
        };
        struct glbBufferData
        {
            uint32_t offset;
            uint32_t stride;
        };

        typedef std::vector<glbAttributeData> glbMeshData;
        
        glbMeshData mesh;
        std::vector<glbNodeData> nodes;
        std::vector<glbMeshData> meshes;
        std::vector<glbMaterialData> materials;
        std::vector<glbTextureData> textures;
        std::vector<glbImageData> images;
        std::vector<glbAccessorData> accessors;
        std::vector<glbBufferViewData> bufferViews;
        std::vector<glbBufferData> buffers;
        
        //  Open a glb file and validate it. 
        //  populate members with respective chunkData.
        lazarus_result loadGlbChunks(const char *filepath);
        //  Hydrate 'buffer' with values pulled from this->binaryData at the locations specified by
        //  the 'accessor' and it's corresponding bufferView.
        void populateBufferFromAccessor(glbAccessorData accessor, std::vector<glm::vec3> &buffer);
        //  Perform copies from memory to a glm vector type, regardless of whether the values 
        //  are tightly packed or interleaved.
        template<typename T> void populateVectorFromMemory(glbAccessorData accessor, glbBufferViewData bufferView, std::vector<T> &vertexData);
        //  Retrieves all information from 'bounds' that occurs between an instance of 'containerStart'
        //  and 'containerEnd'.
        std::vector<std::string> extractContainedContents(std::string bounds, std::string containerStart, std::string containerEnd);
        //  Retrieve all integers immediately following 'target' that occur within 'bounds'.
        int32_t extractAttributeIndex(std::string bounds, std::string target);

        //  wavefront-related members

        std::vector<std::string> wavefrontCoordinates;
        
        std::vector<std::vector<uint32_t>> materialBuffer;
        std::vector<uint32_t> materialData;
        uint32_t materialIdentifierIndex;
        uint32_t triangleCount;
        
        char currentLine[UINT8_MAX];
        std::vector<std::string> attributeIndexes;

        //  wavefront mtl
        uint32_t diffuseCount;
        uint32_t textureCount;
        glm::vec3 diffuse;
        
        //  Read vertex attributes from temp* members and group them together 
        //  in sets of three's if possible.
        lazarus_result constructTriangle();

        //  Shared members
        
        ifstream file;
        std::unique_ptr<FileLoader> fileLoader;
        //  Identifies and contains the contents from 'wavefrontData' that occur between instances
        //  of 'delim'.
        std::vector<std::string> splitTokensFromLine(const char *wavefrontData, char delim);
        //  Deduplicate vertex attributes and construct a serial for those that are unique to be 
        //  passed to the renderers IBO. Interleaves attributes in the order that is expected by
        //  the renderers VBO.
        void constructIndexBuffer(std::vector<glm::vec3> &outAttributes, std::vector<uint32_t> &outIndexes, std::vector<glm::vec3> outDiffuse, uint32_t numOfAttributes);
        //  Clears containers of all their contents.
        void resetMembers();

        std::vector<uint32_t> layers;

        std::vector<uint32_t> vertexIndices;
        std::vector<uint32_t> uvIndices;
        std::vector<uint32_t> normalIndices;

        std::map<uint32_t, glm::vec3> tempVertexPositions;
        std::map<uint32_t, glm::vec3> tempUvs;
        std::map<uint32_t, glm::vec3> tempNormals;
        std::vector<FileLoader::Image> tempImages;
        std::vector<glm::vec3> tempDiffuse;

        glm::vec3 vertex;
        glm::vec3 uv;
        glm::vec3 normal;
};

#endif