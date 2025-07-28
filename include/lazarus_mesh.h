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
#include <string>
#include <stdlib.h>
#include <memory>
#include <fstream>
#include <cstring>
#include <algorithm>

#include "lazarus_file_reader.h"
#include "lazarus_texture_loader.h"

using std::unique_ptr;
using std::shared_ptr;
using std::string;
using std::vector;
using glm::mat4;
using glm::vec3;
using glm::vec2;
using std::ifstream;
using std::stringstream;

#ifndef LAZARUS_MATERIAL_LOADER_H
#define LAZARUS_MATERIAL_LOADER_H

class MaterialLoader
{
    public:        
        MaterialLoader();
        bool loadMaterial(vector<vec3> &out, vector<vector<uint32_t>> data, string materialPath);
        virtual ~MaterialLoader();

    private:
        vec3 diffuse;                                           //  Diffuse colour, the main / dominant colour of a face
        ifstream file;
        char currentLine[256];
        uint32_t diffuseCount;                                    //  The number of times an instance of `char[]="Kd"`(diffuse color) has appeared since the last invocation
        uint32_t texCount;

        GlobalsManager globals;
};
#endif

#ifndef LAZARUS_MESH_LOADER_H
#define LAZARUS_MESH_LOADER_H

class MeshLoader : private MaterialLoader
{
    public:
        ifstream file;
    	MeshLoader();	
    	    
        bool parseWavefrontObj(
            vector<vec3> &outAttributes,
            vector<vec3> &outDiffuse,
            vector<uint32_t> &outIndexes,
            const char *meshPath,
            const char *materialPath
        );

        bool parseGlBinary(
            vector<vec3> &outAttributes,
            vector<vec3> &outDiffuse,
            vector<uint32_t> &outIndexes,
            FileReader::Image &outImage,
            const char *meshPath
        );
        
        virtual ~MeshLoader();

    private:
        //  glb
        std::string jsonData;
        std::string binaryData;
        
        struct glbMeshData
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
        std::vector<glbMeshData> meshes;
        std::vector<glbMaterialData> materials;
        std::vector<glbTextureData> textures;
        std::vector<glbImageData> images;
        std::vector<glbAccessorData> accessors;
        std::vector<glbBufferViewData> bufferViews;
        std::vector<glbBufferData> buffers;
        
        void loadGlbChunks(const char *filepath);
        void populateBufferFromAccessor(glbAccessorData accessor, std::vector<glm::vec3> &buffer);
        template<typename T> void populateVectorFromMemory(glbAccessorData accessor, glbBufferViewData bufferView, std::vector<T> &vertexData);
        std::vector<std::string> extractContainedContents(std::string bounds, std::string containerStart, std::string containerEnd);
        int32_t extractAttributeIndex(std::string bounds, std::string target);

        //  wavefront

        void constructTriangle();
        
        vector<string> coordinates;
        
        vector<vector<uint32_t>> materialBuffer;
        vector<uint32_t> materialData;
        uint32_t materialIdentifierIndex;
        uint32_t triangleCount;
        
        char currentLine[256];
        vector<string> attributeIndexes;
        
        //  Shared

        std::unique_ptr<FileReader> imageLoader;
        vector<string> splitTokensFromLine(const char *wavefrontData, char delim);
        void constructIndexBuffer(vector<vec3> &outAttributes, vector<uint32_t> &outIndexes, vector<vec3> outDiffuse, uint32_t numOfAttributes);
        void resetMembers();

        vector<uint32_t> vertexIndices;
        vector<uint32_t> uvIndices;
        vector<uint32_t> normalIndices;
        vector<vec3> tempVertexPositions;
        vector<vec3> tempUvs;
        vector<vec3> tempNormals;
        vector<vec3> tempDiffuse;

        vec3 vertex;
        vec3 uv;
        vec3 normal;

        GlobalsManager globals;
};

#endif

#ifndef LAZARUS_MESH_H
#define LAZARUS_MESH_H

class MeshManager : private MeshLoader, public TextureLoader
{
    public:
        struct Mesh
        {
            uint32_t id;

            uint32_t numOfVertices;
            uint32_t numOfFaces;
            /* ===============================
            	TODO | Things to add:
                - Material count
                - Scale
            ================================== */
            
            string meshFilepath;
            string materialFilepath;
            string textureFilepath;

            float locationX;
            float locationY;
            float locationZ;

            mat4 modelMatrix;

            uint8_t is3D;
            uint8_t isGlyph;
            uint8_t isSkybox;
            bool isClickable;
        };
		
		MeshManager(GLuint shader);
		
        Mesh create3DAsset(string meshPath, string materialPath = LAZARUS_TEXTURED_MESH, string texturePath = LAZARUS_DIFFUSE_MESH, bool selectable = false);
        Mesh createQuad(float width, float height, string texturePath = LAZARUS_DIFFUSE_MESH, float uvXL = 0.0, float uvXR = 0.0, float uvYU = 0.0, float uvYD = 0.0, bool selectable = false);
        Mesh createCube(float scale, string texturePath = LAZARUS_SKYBOX_CUBE, bool selectable = false);

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

            FileReader::Image textureData;
            GLuint textureId;
            GLuint textureLayer;
            GLuint VAO;                                                                         //  The OpenGL Vertex Array Object
            GLuint VBO;
            GLuint EBO;

            vector<uint32_t> indexes;
            vector<vec3> attributes;
            vector<vec3> diffuse;
        };

        void resolveFilepaths(string texPath = LAZARUS_DIFFUSE_MESH, string mtlPath = LAZARUS_TEXTURED_MESH, string objPath = LAZARUS_PRIMITIVE_MESH);
        void setInherentProperties();
        void initialiseMesh();
        void makeSelectable(bool selectable);
        void prepareTextures();
        
        void checkErrors(const char *file, uint32_t line);

        int32_t errorCode;
        int32_t layerCount;

        uint32_t maxTexWidth;
        uint32_t maxTexHeight;

		GLuint shaderProgram;
        GLint modelMatrixUniformLocation;                                                                        //  The location / index of the modelview matrix inside the vert shader program
        GLint textureLayerUniformLocation;
        GLint is3DUniformLocation;
        GLint isGlyphUniformLocation;
        GLint isSkyBoxUniformLocation;

        unique_ptr<FileReader> finder;
        
        Mesh meshOut;
        MeshData meshData;
        vector<MeshData> dataStore;

        GlobalsManager globals;

};

#endif