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
#include <set>
#include <string>
#include <stdlib.h>
#include <memory>
#include <cstring>
#include <algorithm>

#include "lazarus_file_loader.h"
#include "lazarus_asset_loader.h"
#include "lazarus_texture_loader.h"
#include "lazarus_shader.h"

using std::unique_ptr;
using std::string;
using std::vector;
using glm::mat4;
using glm::vec3;

#ifndef LAZARUS_MESH_H
#define LAZARUS_MESH_H


class ModelManager 
    : private AssetLoader, protected TextureLoader
{
    public:
        enum MaterialType
        {
            IMAGE_TEXTURE = 1,
            DIFFUSE_COLOR = 2
        };
        struct Material
        {
            uint32_t id;
            MaterialType type;
            
            FileLoader::Image texture;
            glm::vec3 diffuse;
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
            bool textureTransparency = false;
            glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
            // glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
        };
        
		ModelManager(Shader &shader, TextureLoader::StorageType textureType = TextureLoader::StorageType::ARRAY);
		
        lazarus_result create3DAsset(Model &out, AssetConfig options);
        lazarus_result createQuad(Model &out, AssetConfig options);
        lazarus_result createCube(Model &out, AssetConfig options);

        lazarus_result loadModel(Model &meshIn);
        lazarus_result drawModel(Model &meshIn);
        
        void copyModel(Model &dest, Model src);

        lazarus_result setActiveAnimation(Model &meshIn, uint32_t animationIndex, uint32_t loopCount = 0);
        lazarus_result setToPosePosition(Model &meshIn);
        lazarus_result pauseAnimation(Model &meshIn);
        lazarus_result playAnimation(Model &meshIn);

        virtual ~ModelManager();
        
        // protected:
        lazarus_result clearMeshStorage();
        protected:
            void setUvOffset(float xL, float xR, float yU, float yD);
        
    private:
        struct MeshData
        {
            struct MotionPoint
            {
                uint32_t id;
                int32_t parentID;
                std::vector<uint32_t> children;
                
                glm::mat4 posePosition;
                glm::mat4 inverseBindMatrix;
                glm::mat4 localJointTransform;
                glm::mat4 globalJointTransform;
                glm::mat4 jointMatrix;

                std::vector<AssetLoader::AssetData::JointMotion> animationData;

                uint32_t playbackPosition = 0;  //  Animation playback pos relative to duration
                int32_t maxLoops = 0;
                uint32_t elapsedLoops = 0;
                uint32_t elapsedPlaytime = 0;   //  The total amount of time the animation has been playing for
                uint32_t previousPlaytime = 0;  //  The above ^ value last-tick
            };

            uint32_t id;
            uint8_t stencilBufferId;

            uint32_t instanceCount;

            uint8_t isAnimated;
            int16_t armatureRoot = -1;
            int32_t activeAnimation = -1;
            uint32_t animationCount = 0;
            bool animationPaused = true;
            
            GLuint VAO;     //  Vertex Array Object
            GLuint VBO;     //  Vertex Buffer Object (attributes: interleaved)
            GLuint EBO;     //  Element Buffer Object (indices: tightly-packed)
            GLuint MBO;     //  Matrice Buffer Object (per-instance matrix: tightly-packed)
            GLuint ABO;     //  Animation Buffer Object (joints + weights: interleaved)
            GLuint IIBO;    //  Instance-info Buffer Object (per-instance: tightly-packed -> will probably end up interleaved)
            
            ModelType type;
            TextureLoader::TextureData texture;
            
            std::vector<FileLoader::Image> images;
            std::vector<uint32_t> indexes;
            std::vector<glm::vec3> attributes;

            std::vector<MotionPoint> armature;
            std::vector<glm::vec4> movements;
        };
        typedef std::vector<MeshData> ModelData;

        /**
         * Set the hang / "pose" position of an animated assets' rigging / armature by establishing
         * a heirachy of joints branching out from a root position.
        */
        lazarus_result composeArmature(AssetLoader::AssetData assetData);

        /**
         * Determines whether the model's meshes contain primitives that use image textures 
         * or instead are composed by defined properties.
        */
        lazarus_result setMaterials(AssetLoader::AssetData &assetData);

        /**
         * Checks for room in the stencil buffer. If room is found a stencil buffer ID is assigned to the 
         * model and each of its composite meshes, as well as their GPU instances. This allows
         * an item to be located on-screen while amongst others by doing a pixel lookup and comparing with 
         * the stencil buffer.
        */
        lazarus_result setSelectable(bool selectable);

        /**
         * Determine whether the active shader program used by `this->shader` has changed and updates
         * the reference if so.
        */
        lazarus_result syncShader();

        /**
         * Generates the models main Vertex attribute object amongst other VBO's, packs them with vertex
         * data and desribes to OpenGL how all of this should be interpretted on the GPU. 
        */
        lazarus_result uploadVertexData();

        /**
         * Determines if any of the materials used by the meshes that compose `this->modelOut` contain 
         * textures and engages the allocator.
        */
        lazarus_result uploadTextures();

        /**
         * Finds and sets the values of shader uniforms which are required for `this` to function correctly.
         * Most likely the active program in the Lazarus::Shader used to instantiate `this` has changed.
        */
        lazarus_result updateUniformLocations();

        /**
         * Reallocates the current texture stack's memory size + 
         * the new amount and reUploads all of the textures again in-order
         * if the current mesh uses image textures.
         * 
         * I.e. Reloads the entire texture stack / array if `this->modelOut` isn't
         * being used for anything special like glyphs or skyboxes, which
         * use different loaders. 
        */
        lazarus_result reallocateTextures();

        /**
         * Identifies and reports on issues with OpenGL
        */
        lazarus_result checkErrors(const char *file, uint32_t line);

        /**
         * Flushes out openGL's error state
        */
        void clearErrors();

        /**
         * Generates a buffer used to handle a meshes GPU instance data
        */
        void instantiateMesh(bool selectable);

        /**
         * Initialises `this->modelOut`'s default properties
        */
        void setMeshProperties(AssetLoader::AssetData &assetData);

        /**
         * Transforms each of the meshes that compose a model to their respective starting positions, directions and size.
         * 
         * TODO:
         * handle rotation
         * do the same for camera / light configs
        */
        void setStartingOrientation(Model &meshIn, glm::vec3 translation, glm::vec3 scale);
        
        /**
         * Based on the current elapsed ms (time), determine
         * where abouts we are in the animation sequence. Use
         * this to look up and interpolate the relevant TRS 
         * keyframe values for the next draw of the animated
         * asset.
        */
        glm::mat4 computeLocalJointTransform(MeshData::MotionPoint &motionPoint, uint32_t animationID);

        /**
         * Applies an animations keyframe data to the the joint of an asset's 
         * armature to modify the location of its' effected vertices.
         */
        void loadAnimation(MeshData &data);

        /**
         * Uses the current elapsed time to locate the indices of the keyframe that should 
         * next be iterated to and interpolated against the current frame.
        */
        uint32_t getKeyframeIndex(AssetLoader::AssetData::JointMotion::TransformData motion, uint32_t &playbackPosition, uint32_t &elapsedMs, uint32_t &previousMs);

        /**
         * Calculate the linear interpolation between keyframe values. Note that 
         * cubicspline interpolation is not supported.
        */
        glm::vec4 getTransformLerp(AssetLoader::AssetData::JointMotion::TransformData motion, uint32_t frameBegin, uint32_t sequenceCursor);
        
        uint32_t childCount;

        int32_t errorCode;

        uint32_t maxTexWidth;
        uint32_t maxTexHeight;

        GLint meshVariantLocation;
        GLint discardFragsLocation;
        GLint isAnimatedLocation;
        GLint jointsMatricesLocation;
        GLuint activeShaderID;

        Shader *shader;
        std::unique_ptr<FileLoader> finder;
        TextureLoader::StorageType textureStorage;

        /*
            Convert std::map to std::set once
            instanced rendering is available.
        */

        Model modelOut;
        MeshData meshData;
        ModelData modelData;
        std::map<uint32_t, ModelData> modelStore;

        /**
         * TODO:
         * There used to be variants for each "create*()" function e.g. createQuad(Quad, QuadConfig)
         * prior to consolidation of it all into "Model" and "AssetConfig".
         * 
         * These floats were the only params that weren't marshalled to the new pattern because
         * they are weird and shouldn't be a part of general config anyway. They were previously in
         * QuadConfig.
         * 
         * Supporting animated textures for sprites would provide the necessary tooling to make
         * changes to uv's dynamically; ideally that same tooling could be used in-place of this... 
         * OR offload glyph stuff from here entirely but that would require duplicating parts
         * of the openGL code in the TextManager like VBO / EBO setup etc which seems... exhausting.
         * OR introduce a createRawGeometry function that could make custom rectangles
        */

        float uvYD = 0.0f;
        float uvYU = 0.0f;
        float uvXL = 0.0f;
        float uvXR = 0.0f;
};

#endif