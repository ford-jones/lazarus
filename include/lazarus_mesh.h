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
#include <sstream>

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

#ifndef LAZARUS_MESH_H
#define LAZARUS_MESH_H

class MaterialLoader
{
    public:        
        MaterialLoader();
        bool loadMaterial(vector<vec3> &out, vector<vector<int>> data, string materialPath);
        virtual ~MaterialLoader();

    private:
        unique_ptr<FileReader> fileReader;
    	unique_ptr<TextureLoader> textureLoader;
        
        vec3 diffuse;                                           //  Diffuse colour, the main / dominant colour of a face
        ifstream file;
        char currentLine[256];
        int diffuseCount;                                    //  The number of times an instance of `char[]="Kd"`(diffuse color) has appeared since the last invocation
        int texCount;

        GlobalsManager globals;
};

class MeshLoader : private MaterialLoader
{
    public:
        ifstream file;

        vector<unsigned int> vertexIndices, uvIndices, normalIndices;
        vector<vec3> tempVertexPositions;
        vector<vec2> tempUvs;
        vector<vec3> tempNormals;
        vector<vec3> tempDiffuse;
    	
    	MeshLoader();	
    	    
        bool parseWavefrontObj(
            vector<vec3> &outAttributes,
            vector<vec3> &outDiffuse,
            vector<unsigned int> &outIndexes,
            const char *meshPath,
            const char *materialPath
        );
        
        virtual ~MeshLoader();

    private:
        vector<string> splitTokensFromLine(const char *wavefrontData, char delim);
        void interleaveBufferData(vector<vec3> &outAttributes, vector<unsigned int> &outIndexes, vector<vec3> outDiffuse, int numOfAttributes);
        void constructTriangle();

        vector<string> coordinates;

        vector<vector<int>> materialBuffer;
        vector<int> materialData;
        int materialIdentifierIndex;
        int triangleCount;
        
        char currentLine[256];
        vector<string> attributeIndexes;
        
        vec3 vertex;
        vec2 uv;
        vec3 normal;

        GlobalsManager globals;
};

class MeshManager : private MeshLoader, public TextureLoader
{
    public:
        struct Mesh
        {
            int id;

            int numOfVertices;
            int numOfFaces;
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

            int is3D;
            int isGlyph;
            int isSkybox;
            bool isClickable;
        };
		
		MeshManager(GLuint shader);
		
        Mesh create3DAsset(string meshPath, string materialPath, string texturePath = LAZARUS_DIFFUSE_MESH, bool selectable = false);
        Mesh createQuad(float width, float height, string texturePath = LAZARUS_DIFFUSE_MESH, float uvXL = 0.0, float uvXR = 0.0, float uvY = 0.0, bool selectable = false);
        Mesh createCube(float scale, string texturePath = LAZARUS_SKYBOX_CUBE, bool selectable = false);

        void clearMeshStorage();

        void loadMesh(Mesh &meshIn);
        void drawMesh(Mesh &meshIn);

        virtual ~MeshManager();
    private:
        struct MeshData
        {
            int id;
            int stencilBufferId;
            int textureUnit;
            FileReader::Image textureData;
            GLuint textureId;
            GLuint textureLayer;
            GLuint VAO;                                                                         //  The OpenGL Vertex Array Object
            GLuint VBO;
            GLuint EBO;

            vector<unsigned int> indexes;
            vector<vec3> attributes;
            vector<vec3> diffuse;
        };

        void resolveFilepaths(string texPath = LAZARUS_DIFFUSE_MESH, string mtlPath = LAZARUS_TEXTURED_MESH, string objPath = LAZARUS_PRIMITIVE_MESH);
        void setInherentProperties();
        void initialiseMesh();
        void makeSelectable(bool selectable);
        void prepareTextures();
        
        void checkErrors(const char *file, int line);

        int errorCode;
        int layerCount;

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