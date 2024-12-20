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
    #include "lz_gl_includes.h"
#endif

#ifndef LAZARUS_CONSTANTS_H
	#include "lz_constants.h"
#endif

#ifndef LAZARUS_GLOBALS_MANAGER_H
    #include "lz_globals_manager.h"
#endif

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <memory>

#include "lz_file_reader.h"
#include "lz_mesh_loader.h"
#include "lz_texture_loader.h"

using std::unique_ptr;
using std::shared_ptr;
using std::string;
using std::vector;
using glm::mat4;
using glm::vec3;
using glm::vec2;

#ifndef LAZARUS_MESH_H
#define LAZARUS_MESH_H

class MeshManager
{
    public:
        struct Mesh
        {
            GLuint VAO;                                                                         //  The OpenGL Vertex Array Object
            GLuint VBO;
            GLuint textureId;
            GLuint textureLayer;

            int numOfVertices;
            int numOfFaces;
            /* ===============================
            	TODO | Things to add:
                - Material count
                - VBO & VAO (persisted)
            ================================== */
            
            string meshFilepath;
            string materialFilepath;
            string textureFilepath;

            float locationX;
            float locationY;
            float locationZ;

            vector<vec3> attributes;
            vector<vec3> diffuse;                                                               //  Buffer to store diffusion color data

            FileReader::Image textureData;

            mat4 modelMatrix;                                                                                    //  A modelview matrix matrice passed into the shader program as a uniform

            GLint modelMatrixUniformLocation;                                                                        //  The location / index of the modelview matrix inside the vert shader program
            GLint samplerUniformLocation;
            GLint textureLayerUniformLocation;
            GLint is3DUniformLocation;
            GLint isGlyphUniformLocation;
            GLint isSkyBoxUniformLocation;

            int is3D;
            int isGlyph;
            int isSkybox;

            int textureUnit;
        };
		
		MeshManager(GLuint shader);
		
        Mesh create3DAsset(string meshPath, string materialPath, string texturePath = "");
        Mesh createQuad(float width, float height, string texturePath = "", float uvXL = 0.0, float uvXR = 0.0, float uvY = 0.0);
        Mesh createCube(float scale, string texturePath = "");

        void prepareTextures();

        void loadMesh(Mesh &meshData);
        void drawMesh(Mesh &meshData);

        virtual ~MeshManager();

    private:
        void resolveFilepaths(Mesh &asset, string texPath = "", string mtlPath = "", string objPath = "");
        void setInherentProperties(Mesh &asset);
        void lookupUniforms(Mesh &asset);
        void initialiseMesh(Mesh &meshData);
        
        void checkErrors(const char *invoker);

        int errorCode;
        int layerCount;

		GLuint shaderProgram;

        unique_ptr<FileReader> finder;
        unique_ptr<MeshLoader> meshLoader;
        unique_ptr<TextureLoader> texLoader;
        
        Mesh mesh;
        vector<Mesh> meshStore;

        GlobalsManager globals;

};

#endif