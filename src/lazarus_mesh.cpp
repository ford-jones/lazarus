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

#include "../include/lazarus_mesh.h"

ModelManager::ModelManager(GLuint shader, TextureLoader::StorageType textureType)
    : ModelManager::AssetLoader(), 
      ModelManager::TextureLoader(textureType)
{
	LOG_DEBUG("Constructing Lazarus::ModelManager");

    this->childCount = 0;
    this->previousMs = 0;
    this->upTimeMs = 0;

    this->shaderProgram = shader;
    this->finder = std::make_unique<FileLoader>();

    this->clearMeshStorage();

    /* ===================================================
        Bind samplers to texture units and load locations.
        https://www.khronos.org/opengl/wiki/Sampler_(GLSL)#:~:text=The%20value%20you%20provide%20to%20a%20sampler%20uniform%20is%20the%20texture%20image%20unit%20to%20which%20you%20will%20bind%20the%20texture%20that%20the%20sampler%20will%20access
    ====================================================== */

    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureAtlas"), 1);
    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureArray"), 2);
    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureCube"), 3);

    this->meshVariantLocation           = glGetUniformLocation(this->shaderProgram, "samplerType");
    this->discardFragsLocation          = glGetUniformLocation(this->shaderProgram, "discardFrags");
    // this->motionCountLocation           = glGetUniformLocation(this->shaderProgram, "motionCount");

    this->maxTexWidth = 0;
    this->maxTexHeight = 0;

    this->textureStorage = textureType;
};

lazarus_result ModelManager::create3DAsset(ModelManager::Model &out, ModelManager::AssetConfig options)
{
    this->modelOut = {};
    this->modelData = {};
    
    glActiveTexture(GL_TEXTURE2);

    modelOut.meshFilepath = options.meshPath;
    modelOut.materialFilepath = options.materialPath;
    
    //  TODO:
    //  If not present populate from file contents instead of filename
    this->modelOut.name = options.name.size()
    ? options.name
    : modelOut.meshFilepath + "_" + std::to_string(this->modelStore.size());

    /* ==========================================
        Determine whether the file is wavefront
        or gltf/glb.
    ============================================= */
    uint32_t suffixDelimiter = modelOut.meshFilepath.find_last_of(".");
    std::string suffix = modelOut.meshFilepath.substr(suffixDelimiter + 1);

    lazarus_result status = lazarus_result::LAZARUS_OK;
    std::vector<AssetLoader::AssetData> assets = {};

    if(suffix.compare("obj") == 0)
    {
        status = AssetLoader::parseWavefrontObj(
            assets,
            modelOut.meshFilepath.c_str(),
            modelOut.materialFilepath.c_str()
        );

        modelOut.type = ModelManager::ModelType::WAVEFRONT;
    }
    else if(suffix.compare("glb") == 0)
    {
        status = AssetLoader::parseGlBinary(
            assets,
            modelOut.meshFilepath.c_str()
        );
        
        modelOut.type = ModelManager::ModelType::GLB;
    };
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    LOG_DEBUG(std::string("Model consists of [").append(std::to_string(assets.size()) + "] mesh objects.").c_str());

    for(size_t i = 0; i < assets.size(); i++)
    {
        AssetLoader::AssetData &assetData = assets[i];
        this->meshData = {};
        this->meshData.texture.unitId = GL_TEXTURE2;
        this->meshData.instanceCount = options.instanceCount;
        
        this->setMeshProperties(assetData);
        
        if(assetData.animations.size() > 0)
        {
            meshData.isAnimated = true;

            /* =========================================================
                Overide default global transform (identity) to use 
                whatever was acquired from asset loading.
            ============================================================ */
            this->meshData.globalTransform = assetData.globalTransform;

            /* ====================================================
                Construct joints and transfroms from loaded data. 
                Flatten out this map and resolve indices 
                corrections.
            ======================================================= */
            std::map<uint32_t, AssetLoader::AssetData::JointData> rig = assetData.armature;

            // uint32_t rootID = UINT32_MAX;
            std::set<uint32_t> heirachy;
            for(auto pair : rig)
            {
                AssetLoader::AssetData::JointData jointData = pair.second;
                AssetLoader::AssetData::JointMotion jointMotion = assetData.animations[0].at(jointData.id);
                MeshData::MotionPoint motionPoint = {};

                motionPoint.id = jointData.id;
                motionPoint.inverseBindMatrix = jointData.inverseBindMatrix;
                motionPoint.animationData = jointMotion;

                /* ============================================
                    Construct pose transform. Note: Blender
                    exports its' quats with 'w' on the wrong 
                    side.
                =============================================== */
                motionPoint.localJointTransform = (
                    glm::translate(glm::mat4(1.0f), jointData.translation) *
                    glm::mat4_cast(glm::quat(jointData.rotation.w, jointData.rotation.x, jointData.rotation.y, jointData.rotation.z)) * 
                    // glm::mat4_cast(glm::quat(jointData.rotation)) * 
                    glm::scale(glm::mat4(1.0f), jointData.scale)
                );
                motionPoint.globalJointTransform = motionPoint.localJointTransform;
                motionPoint.jointMatrix = glm::mat4(0.0f);
                
                for(size_t k = 0; k < jointData.children.size(); k++)
                {
                    /* ============================================
                        Use GLB node id to look up the actual joint 
                        locations within the buffer (.id), these 
                        are required for computing the joint matrix 
                        (the same as JointMotion::targetJoint)
                    =============================================== */
                    uint32_t childID = jointData.children[k];
                    uint32_t index = assetData.armature.at(childID).id;
                    motionPoint.children.push_back(index);

                    /*
                        {J,  J,  m,  c,  J,  J, ...}
                         0   1           4   5       <-childID
                         |   |     ______|   |
                         |   |    /    ______|
                        {J,  J,  J,  J,         ...}
                         0   1   2   3               <-JointData::id
                    */
                   heirachy.insert(index);
                };
                
                meshData.armature.push_back(motionPoint);
            };
            
            /* =========================================
                Ensure flattened armature is indexed in
                the same order of appearance as in glb.
            ============================================ */
            std::sort(
                meshData.armature.begin(), 
                meshData.armature.end(), 
                [](MeshData::MotionPoint &jointA, MeshData::MotionPoint &jointB) {
                    return jointA.id < jointB.id;
                }
            );
            

            /* ====================================================
                Determine armature root (The only node in the tree
                which is never referred to as a child of another 
                node).
            ======================================================= */
            for(size_t j = 0; j < meshData.armature.size(); j++)
            {
                if(heirachy.insert(j).second)
                {
                    meshData.armatureRoot = j;
                    break;
                };
            };

            /* ==================================================
                Traverse joint structure from root and apply 
                transforms forward to children as the tree is 
                descended.
            ===================================================== */
            MeshData::MotionPoint &rootJoint = meshData.armature[meshData.armatureRoot];
            std::vector<uint32_t> children = rootJoint.children;
            std::vector<uint32_t> parents;
            parents.resize(rootJoint.children.size(), meshData.armatureRoot);
            std::set<uint32_t> visited = {rootJoint.id};
            rootJoint.parentID = meshData.armatureRoot;
            rootJoint.posePosition = rootJoint.globalJointTransform;
            rootJoint.jointMatrix = rootJoint.globalJointTransform * rootJoint.inverseBindMatrix;
            
            while(children.size() > 0)
            {
                std::vector<uint32_t> nextChildren;
                std::vector<uint32_t> nextParents;

                for(size_t k = 0; k < children.size(); k++)
                {
                    uint32_t childID = children[k];
                    uint32_t parentID = parents[k];

                    if(visited.insert(childID).second)
                    {
                        MeshData::MotionPoint &child = meshData.armature[childID];     // Note: Joint is declared as a reference
                        MeshData::MotionPoint &parent = meshData.armature[parentID];
                        nextChildren.insert(
                            nextChildren.end(), 
                            child.children.begin(), 
                            child.children.end()
                        );
                        nextParents.resize(nextParents.size() + child.children.size(), childID);
    
                        glm::mat4 localTransform = child.globalJointTransform;
                        child.parentID = parentID;
                        child.globalJointTransform = parent.globalJointTransform * localTransform;
                        child.posePosition = child.globalJointTransform;
                        child.jointMatrix = child.globalJointTransform * child.inverseBindMatrix;
                    };
                };

                children = nextChildren;
                parents = nextParents;
            };
        };

        this->instantiateMesh(options.selectable);
        
        status = this->setMaterials(assetData);
        if(status != lazarus_result::LAZARUS_OK)
        {
            break;
        };

        status = this->uploadVertexData();
        if(status != lazarus_result::LAZARUS_OK)
        {
            break;
        };
            
        this->modelData.push_back(this->meshData);
    };
    
    out = this->modelOut;
    glBindVertexArray(0);
    
    this->modelStore.insert(std::pair<uint32_t, ModelManager::ModelData>(this->modelOut.id, this->modelData));
    this->setSelectable(options.selectable);

    return this->uploadTextures();
};

lazarus_result ModelManager::uploadTextures()
{
    /* ===============================================================
        Reload the entire texture stack / array if the mesh isn't
        being used for anything special like glyphs or skyboxes, which
        use different loaders. 
        
        I.e. reallocate the existing memory size + the new amount and 
        upload all of the textures again if the current mesh uses 
        image textures.
    ================================================================== */
    bool containsTextures = false;
    lazarus_result status = lazarus_result::LAZARUS_OK;

    if(this->textureStorage == TextureLoader::StorageType::ARRAY)
    {
        for(size_t i = 0; i < modelOut.materials.size(); i++)
        {
            if(modelOut.materials[i].type == MaterialType::IMAGE_TEXTURE)
            {
                containsTextures = true;
                break;
            };
        };
        if(containsTextures)
        {
            status = this->reallocateTextures();
            if(status != lazarus_result::LAZARUS_OK)
            {
                return status;
            };
        };
    };

    return status;
};

/* ========================================================================================
    Note: There is some strange behaviour with this function.
    If a single instance of *this* class is responsible for creating quads as well as 3D
    assets - a texturing artifact will present itself when the user *creates* the quad 
    prior to creating the 3D assets. (i.e. createQuad is called before create3DAsset).

    At the quads origin in worldspace, a "shadow" of the quad will render using texture 
    number 1 off of the xyzTextureStack. Not sure why.

    This is *very* similar to the behaviour seen on MacOS and is possibly related. It 
    doesn't present itself on linux.

    Worth mentioning that this doesn't seem to happen with glyphs - which are wrapped over
    a quad under the hood.
=========================================================================================== */

lazarus_result ModelManager::createQuad(ModelManager::Model &out, ModelManager::QuadConfig options)
{
    LOG_DEBUG("Generating quad");
    
    if(options.width < 0.0f || options.height < 0.0f)
    {
        LOG_ERROR("Asset Error:", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_INVALID_DIMENSIONS;
    };

    AssetLoader::AssetData assetData = {};
    this->modelOut = {};
    this->modelData = {};

    modelOut.type = ModelManager::ModelType::PLANE;

    modelOut.meshFilepath = "";
    modelOut.materialFilepath = "";

    this->modelOut.name = options.name.size() 
    ? options.name 
    : this->modelOut.name.append(std::to_string(this->modelStore.size()));
    
    int32_t textureUnit = this->textureStorage == TextureLoader::StorageType::ATLAS
    ? GL_TEXTURE1
    : GL_TEXTURE2;

    glActiveTexture(textureUnit);

    lazarus_result status = lazarus_result::LAZARUS_OK;

    if(options.texturePath.size() > 0)
    {
        FileLoader::Image image = {};
        status = finder->loadImage(image, options.texturePath.c_str());
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };

        assetData.textures.push_back(image);
        assetData.colors.push_back(vec3(-0.1f, -0.1f, -0.1f));

        /* ============================================
            Increment the loaders texture array slice 
            count to stay in sync with future loads of
            other assets.
        =============================================== */

        AssetLoader::layerCount += 1;
    };

    float layer = static_cast<float>(AssetLoader::layerCount - 1);

    /* ======================================================
        Ensure that the origin is centered.
        (E.g. width 2.0f, height 2.0f becomes 
        width -1.0f, height +1.0f) 
    ========================================================= */
    float xMin = -(options.width / 2.0f);
    float xMax = options.width / 2.0f;
    float yMax = options.height / 2.0f;
    float yMin = -(options.height / 2.0f);
    /* ==========================================================
        If the UV params aren't their default values (0.0) then
        this mesh is being created for a glyph which needs to be 
        looked up in the texture atlas.

        Otherwise it's a generic sprite.
    ============================================================= */
    if(options.uvXL || options.uvXR || options.uvYU || options.uvYD > 0.0f)
    {
    /* ======================================================================================================
            Vertex positions,           Diffuse colors,             Normals,                    UVs 
    ========================================================================================================= */
        assetData.attributes = {                                                                                          
            vec3(xMin, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(options.uvXL, options.uvYD, 0.0f),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(options.uvXR, options.uvYD, 0.0f), 
            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(options.uvXL, options.uvYU, 0.0f),
            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(options.uvXR, options.uvYU, 0.0f),

            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(options.uvXR, options.uvYU, 0.0f),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(options.uvXR, options.uvYD, 0.0f),
            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(options.uvXL, options.uvYU, 0.0f),
            vec3(xMin, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(options.uvXL, options.uvYD, 0.0f),
        };
    }
    else
    {
        /* ==================================================
            For some reason Mac requires two sets of the 
            vertices for what at a glance looks like an
            unwinding / culling type of issue. Note the sign
            of the normals.
        ===================================================== */
        assetData.attributes = {
            vec3(xMin, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(0.0f, 0.0f, layer),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(1.0f, 0.0f, layer),
            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(0.0f, 1.0f, layer),
            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(1.0f, 1.0f, layer),

            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(1.0f, 1.0f, layer),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(1.0f, 0.0f, layer),
            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(0.0f, 1.0f, layer),
            vec3(xMin, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(0.0f, 0.0f, layer),
        };
    };

    assetData.indices = {
        0, 1, 2, 
        2, 1, 3, 
        4, 5, 6, 
        6, 5, 7
    };

    /* ====================================================
        Zero-out movement / animation buf
    ======================================================= */
    assetData.movements.clear();
    assetData.movements.resize(assetData.attributes.size() / 2, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

    this->meshData = {};
    this->meshData.texture.unitId = textureUnit;
    this->meshData.instanceCount = options.instanceCount;

    this->setMeshProperties(assetData);
    this->instantiateMesh(options.selectable);
    
    status = this->setMaterials(assetData);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    status = this->uploadVertexData();
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    this->modelData.push_back(this->meshData);
    this->modelStore.insert(std::pair<uint32_t, ModelManager::ModelData>(this->modelOut.id, this->modelData));

    status = this->setSelectable(options.selectable);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    out = this->modelOut;
    glBindVertexArray(0);

    return this->uploadTextures();
}

lazarus_result ModelManager::createCube(ModelManager::Model &out, ModelManager::CubeConfig options)
{
    LOG_DEBUG("Generating cube");
    float vertexPosition = options.scale / 2.0f; 
    
    AssetLoader::AssetData assetData = {};
    this->modelOut = {};
    this->modelData = {};

    modelOut.type = ModelManager::ModelType::CUBE;

    modelOut.meshFilepath = "";
    modelOut.materialFilepath = "";

    this->modelOut.name = options.name.size() 
    ? options.name 
    : this->modelOut.name.append(std::to_string(this->modelStore.size()));
    
    int32_t textureUnit = this->textureStorage == TextureLoader::StorageType::CUBEMAP
    ? GL_TEXTURE3
    : GL_TEXTURE2;

    float layer = 0.0;
    
    lazarus_result status = lazarus_result::LAZARUS_OK;

    if(this->textureStorage == TextureLoader::StorageType::CUBEMAP)
    {
        FileLoader::Image image = {};
        image.height = 0;
        image.width = 0;
        image.pixelData = NULL;

        assetData.textures.push_back(image);   
    }
    else
    {
        if(options.texturePath.size() > 0)
        {
            FileLoader::Image image = {};
            status = finder->loadImage(image, options.texturePath.c_str());
            if(status != lazarus_result::LAZARUS_OK)
            {
                return status;
            };

            assetData.textures.push_back(image);   

            /* ============================================
                Increment the loaders texture array slice 
                count to stay in sync with future loads of
                other assets.
            =============================================== */
            
            AssetLoader::layerCount += 1;
        };
        layer = static_cast<float>(AssetLoader::layerCount);
    };
    
    assetData.colors.push_back(vec3(-0.1f, -0.1f, -0.1f));

    assetData.attributes = {                                                                                          
        /* ===========================================

               POS                      ST

          (5)_____(2, 6)           0:1 _______1:1
            \      /\               |          |
             \    /  \              |          |
              \  /    \             |          |
               \/______\            |__________|
            (1, 4)     (3)         0:0        1:0

        ============================================== */
        
        // Top face
        vec3(-vertexPosition, vertexPosition,  -vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 1.0f, 0.0f),  vec3(0.0f, 0.0f, layer),
        vec3(vertexPosition, vertexPosition,  vertexPosition),      vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 1.0f, 0.0f),  vec3(1.0f, 1.0f, layer),
        vec3(vertexPosition,  vertexPosition,  -vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 1.0f, 0.0f),  vec3(1.0f, 0.0f, layer),
        vec3(-vertexPosition,  vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 1.0f, 0.0f),  vec3(0.0f, 1.0f, layer),

        // Back face
        vec3(-vertexPosition, vertexPosition,  -vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, layer),
        vec3(vertexPosition, -vertexPosition, -vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 1.0f, layer),
        vec3(-vertexPosition,  -vertexPosition, -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 0.0f, layer),
        vec3(vertexPosition,  vertexPosition, -vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, layer),

        // Right face
        vec3(vertexPosition, vertexPosition, -vertexPosition),      vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(0.0f, 0.0f, layer),
        vec3(vertexPosition, -vertexPosition,  vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(1.0f, 1.0f, layer),
        vec3(vertexPosition,  -vertexPosition,  -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(1.0f, 0.0f, layer),
        vec3(vertexPosition,  vertexPosition,  vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(0.0f, 1.0f, layer),

        // Front face
        vec3(vertexPosition, vertexPosition, vertexPosition),       vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(0.0f, 0.0f, layer),
        vec3(-vertexPosition, -vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(1.0f, 1.0f, layer),
        vec3(vertexPosition,  -vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(1.0f, 0.0f, layer),
        vec3(-vertexPosition,  vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(0.0f, 1.0f, layer),

        // Left face
        vec3(-vertexPosition,  vertexPosition, vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, layer),
        vec3(-vertexPosition,  -vertexPosition, -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, layer),
        vec3(-vertexPosition,  -vertexPosition,  vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, layer),
        vec3(-vertexPosition,  vertexPosition,  -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, layer),

        // Bottom face
        vec3(vertexPosition, -vertexPosition, -vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, layer),
        vec3(-vertexPosition, -vertexPosition, vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(1.0f, 1.0f, layer),
        vec3(-vertexPosition, -vertexPosition,  -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(1.0f, 0.0f, layer),
        vec3(vertexPosition, -vertexPosition,  vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 1.0f, layer),
    };

    assetData.indices = {
        0, 1, 2, 0, 3, 1,
        4, 5, 6, 4, 7, 5, 
        8, 9, 10, 8, 11, 9,
        12, 13, 14, 12, 15, 13,
        16, 17, 18, 16, 19, 17,
        20, 21, 22, 20, 23, 21
    };
    
    /* ====================================================
        Zero-out movement / animation buf
    ======================================================= */
    assetData.movements.clear();
    assetData.movements.resize(assetData.attributes.size() / 2, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

    this->meshData = {};
    this->meshData.texture.unitId = textureUnit;
    this->meshData.instanceCount = options.instanceCount;

    this->setMeshProperties(assetData);
    this->instantiateMesh(options.selectable);
    
    status = this->setMaterials(assetData);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };
    
    status = this->uploadVertexData();
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };
    
    this->modelData.push_back(this->meshData);
    this->modelStore.insert(std::pair<uint32_t, ModelManager::ModelData>(this->modelOut.id, this->modelData));

    status = this->setSelectable(options.selectable);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    out = this->modelOut;
    glBindVertexArray(0);

    return this->uploadTextures();
};

lazarus_result ModelManager::uploadVertexData()
{	
    LOG_DEBUG("Allocating vertex array object");
    glGenVertexArrays(1, &meshData.VAO);
   	glBindVertexArray(meshData.VAO);

    this->clearErrors();

    LOG_DEBUG("Allocating index buffer object.");
    glGenBuffers(1, &meshData.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, 
        meshData.indexes.size() * sizeof(uint32_t), 
        &meshData.indexes[0], 
        GL_STATIC_DRAW
    );

    LOG_DEBUG("Allocating vertex buffer object");
    glGenBuffers(1, &meshData.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, meshData.VBO);
    glBufferData(
        GL_ARRAY_BUFFER, 
        meshData.attributes.size() * sizeof(glm::vec3), 
        &meshData.attributes[0], 
        GL_STATIC_DRAW
    );

    //  Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(glm::vec3)), (void*)0);

    //  Diffuse Colors
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(glm::vec3)), (void*)(1 * sizeof(glm::vec3)));

    //  Normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(glm::vec3)), (void*)(2 * sizeof(glm::vec3)));

    //  UV Coordinates
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(glm::vec3)), (void*)(3 * sizeof(glm::vec3)));

    lazarus_result status = this->checkErrors(__FILE__, __LINE__);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };
    
    this->clearErrors();

    LOG_DEBUG("Allocating per-instance matrice data");
    /* =================================================================
        Convert matrice map (RB BT, uint32_t key) to a vector of 
        map[i].second
    ==================================================================== */
    std::vector<glm::mat4> matrices;
    std::transform(
        this->modelOut.instances.begin(), 
        this->modelOut.instances.end(), 
        std::back_inserter(matrices), 
        [](const std::pair<uint32_t, ModelManager::Model::Instance> &pair){
            return pair.second.modelMatrix;
        }
    );

    /* ===========================================================
        Buffer used for storing per-instance model-matrix data. 
        This data is changed very frequently, so use dynamic draw.
    ============================================================== */
    LOG_DEBUG("Allocating matrice buffer object");
    glGenBuffers(1, &meshData.MBO);
    glBindBuffer(GL_ARRAY_BUFFER, meshData.MBO);

    glBufferData(
        GL_ARRAY_BUFFER, 
        this->modelOut.instances.size() * sizeof(glm::mat4), 
        &matrices[0][0],
        GL_DYNAMIC_DRAW
    );

    /* =============================================================
        Delineate vertex attribute positions within the buffer. 

        The largest data type available in GLSL is a vec4 (16 bytes)
        1 / 4 of the size required to store a matrice.
        Use attribute divisors to delineate matrice columns down to 
        4 vec4's. These then occupy locations 4 - 7 in the vertex
        shader layout.
    ================================================================ */
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, (4 * sizeof(glm::vec4)), (void*)0);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, (4 * sizeof(glm::vec4)), (void*)(1 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, (4 * sizeof(glm::vec4)), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, (4 * sizeof(glm::vec4)), (void*)(3 * sizeof(glm::vec4)));

    LOG_DEBUG("Setting memory divisors");
    /* ================================================
        Layout positons 4-7 are evaluated for every
        instance. I.e. per iteration of gl_InstanceID
        in the fragment shader.
    =================================================== */
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);

    LOG_DEBUG("Allocating supplementary instance information");
    /* ===============================================
        Additional buffer for storing per-instance 
        information / settings. Matrices are treated
        seperately to preserve alignment, to have them
        mixed up would be very slow.
    ================================================== */
    glGenBuffers(1, &meshData.IIBO);
    glBindBuffer(GL_ARRAY_BUFFER, meshData.IIBO);

    /* ==============================================
        The GPU is optimised for floating point with 
        less overhead in a large number of cases so
        promote the boolean.
    ================================================= */
    std::vector<float> visibility;
    std::transform(
        this->modelOut.instances.begin(), 
        this->modelOut.instances.end(), 
        std::back_inserter(visibility), 
        [](const std::pair<uint32_t, ModelManager::Model::Instance> &pair){
            uint32_t v = pair.second.isVisible;
            return static_cast<float>(v);
        }
    );

    //  This could possibly be changed to GL_STREAM_DRAW, as the contents *currently* will
    //  be updated but not often. Only hesitance is due to the fact that this buffer may 
    //  be used to store all sorts of other per-instance data and culmilatively this could 
    //  end up requiring updates every frame in some cases so use GL_DYNAMIC_DRAW.

    glBufferData(
        GL_ARRAY_BUFFER, 
        this->modelOut.instances.size() * sizeof(float), 
        &visibility[0],
        GL_DYNAMIC_DRAW
    );

    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);

    glVertexAttribDivisor(8, 1);

    LOG_DEBUG("Allocating animation buffer object");

    glGenBuffers(1, &meshData.ABO);
    glBindBuffer(GL_ARRAY_BUFFER, meshData.ABO);
    glBufferData(
        GL_ARRAY_BUFFER,
        meshData.movements.size() * sizeof(glm::vec4),
        &meshData.movements[0],
        GL_STATIC_DRAW
    );

    //  Joints
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec4), (void*)0);

    //  Weights
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec4), (void*)(1 * sizeof(glm::vec4)));

    status = this->checkErrors(__FILE__, __LINE__);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    LOG_DEBUG("Finished buffering vertex data to GPU");

    /* ===================================================
        The id of a mesh and it's respective internal
        data is the same as the id of it's first child 
        instance.
    ====================================================== */

    modelOut.id = modelOut.instances.at(0).id;
    meshData.id = this->modelData.size();
	
    return LAZARUS_OK;
};

lazarus_result ModelManager::reallocateTextures()
{
    uint32_t width = 0;
    uint32_t height = 0;

    /* =========================================
        Determine the height and width of the 
        texture array and allocate the space.
    ============================================ */

    if(GlobalsManager::getEnforceImageSanity())
    {
        width = GlobalsManager::getMaxImageWidth();
        height = GlobalsManager::getMaxImageHeight();
    }
    else
    {
        width = this->maxTexWidth;
        height = this->maxTexHeight;
    }

    lazarus_result status = TextureLoader::extendTextureStack(width, height, AssetLoader::layerCount);
    if (status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    /* ===============================================
        Since memory size has been redefined, the
        previous contents now need to be rewritten to
        the new location.

        TODO:
        Is there some way to invoke a memcpy of the
        VRAM contents from here?
    ================================================== */

    uint32_t textureCount = 0;
    /* Model Container */
    for(auto i: this->modelStore)
    {
        /* Model */
        for(auto j: i.second)
        {
            glActiveTexture(j.texture.unitId);
            if(this->textureStorage == TextureLoader::StorageType::ARRAY)
            {
                /* Mesh */
                for(size_t k = 0; k < j.images.size(); k++)
                {
                    if(j.images[k].pixelData != NULL)
                    {
                        textureCount += 1;
                        status = TextureLoader::loadImageToTextureStack(j.images[k], textureCount);
                        if (status != lazarus_result::LAZARUS_OK)
                        {
                            return status;
                        };
                    };
                };
            };
        };
    };

    return lazarus_result::LAZARUS_OK;
};

lazarus_result ModelManager::clearMeshStorage()
{	
    LOG_DEBUG("Freeing model store contents");
    this->errorCode = GL_NO_ERROR;
    
    this->clearErrors();
    for(auto i: modelStore)
    {
        for(auto j: i.second)
        {
            glDeleteBuffers         (1, &j.VBO);
            glDeleteBuffers         (1, &j.MBO);
            glDeleteBuffers         (1, &j.EBO);
            glDeleteBuffers         (1, &j.ABO);
            glDeleteBuffers         (1, &j.IIBO);
            glDeleteVertexArrays    (1, &j.VAO);
        };
    };
    
    this->modelOut = {};
    this->meshData = {};

    this->modelData.clear();
    this->modelStore.clear();

    this->childCount = 0;
    AssetLoader::layerCount = 0;
    
    return this->checkErrors(__FILE__, __LINE__);
};

lazarus_result ModelManager::setSelectable(bool selectable)
{
    uint32_t numOccupants = GlobalsManager::getNumberOfPickableEntities();
    if(selectable)
    {
        if(numOccupants < UINT8_MAX)
        {
            /* =============================================
                Items which can be picked from the stencil-
                depth buffer have their ID's stored in a
                global vector. The index position is then 
                used as the stencil function's reference 
                parameter. When that ID is then downloaded f
                rom the GPU after a draw call, it is used to 
                perform a lookup on the vector for the mesh 
                ID which; is then returned to userspace.
            ================================================ */

            GlobalsManager::setPickableEntity(modelOut.id);
            ModelManager::ModelData &m = modelStore.at(modelOut.id);
            for(auto &i: m)
            {
                i.stencilBufferId = numOccupants + 1;
            };
        }
        else
        {
            return lazarus_result::LAZARUS_LIMIT_REACHED;
        }
    }
    else
    {
        ModelManager::ModelData m = modelStore.at(modelOut.id);
        for(auto &i: m)
        {
            i.stencilBufferId = 0;
        };
    };

    return lazarus_result::LAZARUS_OK;
};

lazarus_result ModelManager::loadModel(ModelManager::Model &meshIn)
{
    std::chrono::system_clock::duration epoch = std::chrono::system_clock::now().time_since_epoch();
    uint32_t currentMs = epoch / std::chrono::milliseconds(1);
    uint32_t timeDelta = currentMs - previousMs;
    previousMs = currentMs;
    upTimeMs += timeDelta;
    
    LOG_DEBUG("Loading model data");
    ModelManager::ModelData &model = modelStore.at(meshIn.id);
    lazarus_result status = lazarus_result::LAZARUS_OK;
    
    for(size_t i = 0; i < model.size(); i++)
    {
        ModelManager::MeshData &data = model[i];

        std::vector<glm::mat4> matrices;
        std::transform(
            meshIn.instances.begin(), 
            meshIn.instances.end(), 
            std::back_inserter(matrices), 
            [](const std::pair<uint32_t, ModelManager::Model::Instance> &pair){
                return pair.second.modelMatrix; 
            }
        );
    
        std::vector<float> visibility;
        std::transform(
            meshIn.instances.begin(), 
            meshIn.instances.end(), 
            std::back_inserter(visibility), 
            [](const std::pair<uint32_t, ModelManager::Model::Instance> &pair){
                uint32_t v = pair.second.isVisible;
                return static_cast<float>(v);
            }
        );
    
        /* =========================================================
            Upload any changes that have been made to matrices.
            glBufferSubData is preferable here over glBufferData as 
            the size has been allocated up front on init and won't 
            change. This allows the data to be updated in-place 
            without performing a realloc. This per-frame upload is
            fine as per-instance MBO and IIBO were init'd with 
            GL_DYNAMIC_DRAW.
    
            TODO:
            Update a portion of the buffer in-place
            rather than the full buffer as is done here. This would 
            need to happen for every instance that has had any 
            change.
        ============================================================ */
        LOG_DEBUG("Uploading instance matrices");

        glBindVertexArray(data.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, data.MBO);
        glBufferSubData(
            GL_ARRAY_BUFFER, 
            0, 
            meshIn.instances.size() * sizeof(glm::mat4), 
            &matrices[0][0]
        );
    
        LOG_DEBUG("Uploading instance information");
        glBindBuffer(GL_ARRAY_BUFFER, data.IIBO);
        glBufferSubData(
            GL_ARRAY_BUFFER, 
            0, 
            meshIn.instances.size() * sizeof(float), 
            &visibility[0]
        );
    
        /* ===================================================
            Fill the stencil buffer with 0's. 
            Wherever an entity is occupying screenspace, fill 
            the buffercwith the mesh's selection.
        ====================================================== */
    
        if(GlobalsManager::getManageStencilBuffer())
        {
            LOG_DEBUG("Loading stencil ID");
            this->clearErrors();
            
            glStencilMask(0xFF);
            glClearStencil(0x00);
            glStencilFunc(GL_ALWAYS, data.stencilBufferId, 0xFF);
            
            status = this->checkErrors(__FILE__, __LINE__);
            if(status != lazarus_result::LAZARUS_OK)
            {
                break;
            };
        };
    
        this->clearErrors();
        
        glUniform1i(this->meshVariantLocation, this->textureStorage);
        glUniform1i(this->discardFragsLocation, data.texture.discardAlphaZero);
        glUniform1i(glGetUniformLocation(this->shaderProgram, "isAnimated"), 0);
        /* =================================================
            Upload animation data if present
        ==================================================== */
        if(data.isAnimated)
        {
            LOG_DEBUG("Loading animation data");
            std::cout << "uptime: " << upTimeMs << std::endl;

            MeshData::MotionPoint &animationRoot = data.armature[data.armatureRoot];

            uint32_t rootTranslateIdx = this->getKeyframeIndex(animationRoot.animationData.translation);
            uint32_t rootRotateIdx = this->getKeyframeIndex(animationRoot.animationData.rotation);
            uint32_t rootScaleIdx = this->getKeyframeIndex(animationRoot.animationData.scale);

            glm::vec4 rootTranslateKey = this->getTransformLerp(animationRoot.animationData.translation, rootTranslateIdx);
            glm::vec4 rootRotateKey = this->getTransformLerp(animationRoot.animationData.rotation, rootRotateIdx);
            glm::vec4 rootScaleKey = this->getTransformLerp(animationRoot.animationData.scale, rootScaleIdx);

            glm::mat4 keyframeTransform = (
                glm::translate(glm::mat4(1.0f), glm::vec3(rootTranslateKey)) *
                glm::mat4_cast(glm::quat(rootRotateKey.w, rootRotateKey.x, rootRotateKey.y, rootRotateKey.z)) *
                glm::scale(glm::mat4(1.0f), glm::vec3(rootScaleKey))
            );

            animationRoot.globalJointTransform = keyframeTransform;
            
            animationRoot.jointMatrix = (
                animationRoot.globalJointTransform * animationRoot.inverseBindMatrix
            );

            std::vector<uint32_t> children = animationRoot.children;
            std::set<uint32_t> visited = {animationRoot.id};

            while(children.size() > 0)
            {
                std::vector<uint32_t> nextChildren;
    
                for(size_t k = 0; k < children.size(); k++)
                {   
                    uint32_t childID = children[k];
                    /* ==========================================
                        Colaterally append all children that
                        exist at this depth to the control vector
                        if this joint hasn't already been 
                        inspected.
                    ============================================= */
                    if(visited.insert(childID).second)
                    {
                        MeshData::MotionPoint &child = data.armature[childID];     // Note: Joint is declared as a reference
                        MeshData::MotionPoint &parent = data.armature[child.parentID];

                        nextChildren.insert(
                            nextChildren.end(), 
                            child.children.begin(), 
                            child.children.end()
                        );

                        uint32_t translateIdx = this->getKeyframeIndex(child.animationData.translation);
                        uint32_t rotateIdx = this->getKeyframeIndex(child.animationData.rotation);
                        uint32_t scaleIdx = this->getKeyframeIndex(child.animationData.scale);

                        glm::vec4 translateKey = this->getTransformLerp(child.animationData.translation, translateIdx);
                        glm::vec4 rotateKey = this->getTransformLerp(child.animationData.rotation, rotateIdx);
                        glm::vec4 scaleKey = this->getTransformLerp(child.animationData.scale, scaleIdx);

                        glm::mat4 keyframeTx = (
                            glm::translate(glm::mat4(1.0f), glm::vec3(translateKey)) *
                            glm::mat4_cast(glm::quat(rotateKey.w, rotateKey.x, rotateKey.y, rotateKey.z)) *
                            glm::scale(glm::mat4(1.0f), glm::vec3(scaleKey))
                        );

                        child.globalJointTransform = parent.globalJointTransform * keyframeTx;

                        child.jointMatrix = (
                            child.globalJointTransform * child.inverseBindMatrix
                        );
                    } 
                };
                /* ==========================================
                    Descend into the children of the children
                    which were just processed.
                ============================================= */
                children = nextChildren;
            };

            /* ==============================================
                Upload updated armature locations
            ================================================= */
            LOG_DEBUG("Uploading joint matrices");

            for(auto joint : data.armature)
            {
                this->jointsMatricesLocation = glGetUniformLocation(this->shaderProgram, std::string("jointMatrices[").append(std::to_string(joint.id) + "]").c_str());
                glUniformMatrix4fv(this->jointsMatricesLocation, 1, GL_FALSE, &joint.jointMatrix[0][0]);
            };
            
            glUniform1i(glGetUniformLocation(this->shaderProgram, "isAnimated"), 1);
        };
    
        status = this->checkErrors(__FILE__, __LINE__);
        if(status != lazarus_result::LAZARUS_OK)
        {
            break;
        };
    };

    return status;
};

uint32_t ModelManager::getKeyframeIndex(AssetLoader::AssetData::JointMotion::TransformData &motion)
{
    /* ======================================================
        Lock time-advance within the maximum duration of the
        active motion.
        TODO:
        An "upTimeMs" is almost needed for every motion?
        i.e. an animation with a duration of 0.2ms that
        follows one that is 0.5ms would be cancelled out. Or 
        inversely, 0.5ms would never be reached... 
    ========================================================= */
    uint32_t durationMax = static_cast<uint32_t>(motion.timesteps.back() * 1000.0f);
    if(upTimeMs >= durationMax)
    {
        upTimeMs = 0;
    };
    /* ===========================================
        Establish current location in the frame 
        sequence.
    ============================================== */
    uint32_t now = upTimeMs;
    std::vector<float>::iterator it = std::find_if(
        motion.timesteps.begin(), 
        motion.timesteps.end(), 
        [now](float time) {
            /* ============================================
                Convert to ms and compare with current 
                time.
            =============================================== */
            uint32_t timestep = static_cast<uint32_t>(time * 1000.0f);
            return now < timestep;
        }
    );
    uint32_t index = it - motion.timesteps.begin();
    std::cout << "keyframe index: " << index << std::endl;

    return index;
};

glm::vec4 ModelManager::getTransformLerp(AssetLoader::AssetData::JointMotion::TransformData &motion, uint32_t frameBegin)
{
    glm::vec4 keyframe = motion.keyframes[frameBegin];
    if( motion.lerp == AssetData::JointMotion::TransformData::InterpolationType::LINEAR &&
        motion.keyframes.size() > 2)
    {
        float timestepStart = motion.timesteps[frameBegin];
        float timestepEnd = motion.timesteps[frameBegin + 1 != motion.keyframes.size() ? frameBegin + 1 : 0];
        float interpolatedTimestep = (upTimeMs - timestepStart * 1000.0f) / (timestepEnd * 1000.0f - timestepStart * 1000.0f);

        glm::vec4 keyframeEnd = motion.keyframes[frameBegin + 1 != motion.keyframes.size() ? frameBegin + 1 : 0];
        glm::quat k = glm::slerp(glm::quat(keyframe.w, keyframe.x, keyframe.y, keyframe.z), glm::quat(keyframeEnd.w, keyframeEnd.x, keyframeEnd.y, keyframeEnd.z), interpolatedTimestep);
        // keyframe = (1 - interpolatedTimestep) * keyframe + interpolatedTimestep * keyframeEnd;
        keyframe = glm::vec4(k.x, k.y, k.z, k.w);
    };

    return keyframe;
};

lazarus_result ModelManager::drawModel(ModelManager::Model &meshIn)
{
    ModelManager::ModelData &model = this->modelStore.at(meshIn.id);
    lazarus_result status = lazarus_result::LAZARUS_OK;
    
    for(size_t i = 0; i < model.size(); i++)
    {
        LOG_DEBUG("Rendering mesh asset");
        ModelManager::MeshData &data = model[i];

        this->clearErrors();
    
        glBindVertexArray(data.VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.EBO);
    
        glActiveTexture(data.texture.unitId);
        /* =========================================
            When a texture is present bind the id 
            of it's sampler to the texture target, 
            overwriting what was previously bound.
    
            Note the textureStoreVariant will be the 
            same for all materials.
        ============================================ */
    
        switch (this->textureStorage)
        {
            case TextureLoader::StorageType::ATLAS:
                glBindTexture(GL_TEXTURE_2D, data.texture.samplerId);
                break;
        
            case TextureLoader::StorageType::CUBEMAP:
                glBindTexture(GL_TEXTURE_CUBE_MAP, data.texture.samplerId);
                break;
            
            case TextureLoader::StorageType::ARRAY:
                glBindTexture(GL_TEXTURE_2D_ARRAY, data.texture.samplerId);
                break;
        
            default:
                break;
        };
    
        glDrawElementsInstanced(
            GL_TRIANGLES, 
            data.indexes.size(), 
            GL_UNSIGNED_INT, 
            nullptr, 
            data.instanceCount
        );
    
        glBindVertexArray(0);
    
        status = this->checkErrors(__FILE__, __LINE__);
        if(status != lazarus_result::LAZARUS_OK)
        {
            break;
        };
    };

    return status;
};

void ModelManager::setDiscardFragments(ModelManager::Model &meshIn, bool shouldDiscard)
{
    LOG_DEBUG("Setting fragment-discard flags");
    ModelManager::ModelData &model = this->modelStore.at(meshIn.id);

    for(size_t i = 0; i < model.size(); i++)
    {
        ModelManager::MeshData &data = model[i];
        data.texture.discardAlphaZero = shouldDiscard;
    };

    return;
};

lazarus_result ModelManager::setMaterials(AssetLoader::AssetData &assetData)
{
    LOG_DEBUG("Allocating materials");
    if(assetData.colors.size() != assetData.textures.size())
    {
        LOG_ERROR("Load Error: Invalid materials", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_ASSET_LOAD_ERROR;
    };

    for(size_t i = 0; i < assetData.colors.size(); i++)
    {
        Material material = {};
        material.diffuse = assetData.colors[i];
        material.texture = assetData.textures[i];

        material.id = i;

        if(((material.diffuse.r + material.diffuse.b + material.diffuse.g) < -0.1f) && 
             material.texture.pixelData != NULL)
        {
            LOG_DEBUG("Copying textures");
            material.type = MaterialType::IMAGE_TEXTURE;

            /* ===============================================
                The data struct must keep it's own copy of the 
                non-null images, used when time to upload / 
                re-upload.
            ================================================== */

            meshData.images.push_back(material.texture);
        }
        else
        {
            LOG_DEBUG("Copying diffuse colors");
            material.type = MaterialType::BASE_COLOR;
    
            meshData.texture.samplerId = 0;
            meshData.texture.unitId = GL_TEXTURE2;
        };

        modelOut.materials.push_back(material);

        /* =============================================================
            In the case that image sanitisation is not enabled, we must
            track which image in the textureStack is the largest.
        ================================================================ */

        this->maxTexWidth = std::max(maxTexWidth, material.texture.width);
        this->maxTexHeight = std::max(maxTexHeight, material.texture.height);
    };

    return lazarus_result::LAZARUS_OK;
};

void ModelManager::setMeshProperties(AssetLoader::AssetData &assetData)
{
    LOG_DEBUG("Loading base mesh properties");

    meshData.texture.discardAlphaZero = false;
    meshData.texture.samplerId = TextureLoader::textureId;
    meshData.globalTransform = glm::mat4(1.0f);

    meshData.attributes = assetData.attributes;     //  Positon, Diffuse, normal, uvs
    meshData.movements = assetData.movements;       //  joints, weights
    meshData.indexes = assetData.indices;           
    // meshData.animations = assetData.animations;

    meshData.isAnimated = false;
    /* ===============================================
        Note that these will be evaluated again when 
        loading from a file (create3DAsset).
    ================================================== */
    meshData.armature = {};
    
    modelOut.numOfVertices = meshData.attributes.size() / 4;
    modelOut.numOfFaces = (modelOut.numOfVertices) / 3;

   return;
}

lazarus_result ModelManager::checkErrors(const char *file, uint32_t line)
{
    this->errorCode = glGetError();
    
    if(this->errorCode != GL_NO_ERROR)
    {
        std::string message = std::string("OpenGL Error: ").append(std::to_string(this->errorCode));
        LOG_ERROR(message.c_str(), file, line);

        return lazarus_result::LAZARUS_OPENGL_ERROR;
    }

    return lazarus_result::LAZARUS_OK;
};

void ModelManager::clearErrors()
{
    /* ============================================================
        Reset OpenGL's error state by flushing out all of the 
        internal state flags containing the error value (which may 
        be several). This is so that persistence of an errors 
        life span is limitted to that function which caused it. 
        By doing so, subsequent glGetError calls made from a function
        other than that which actually caused the error will NOT 
        throw. 
        
        Absolutely painful, see:
        https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetError.xhtml#:~:text=glGetError%20should%20always%20be%20called%20in%20a%20loop
    ================================================================ */

    this->errorCode = glGetError();

    while(this->errorCode != GL_NO_ERROR)
    {
        this->errorCode = glGetError();
    };

    return;
};

void ModelManager::copyModel(ModelManager::Model &dest, ModelManager::Model src)
{
    LOG_DEBUG(std::string("Copying model [").append(src.name+ "]").c_str());
    this->modelOut = src;
    this->modelData = this->modelStore.at(modelOut.id);
    bool selectable = this->modelOut.instances.at(0).isClickable;
    
    ModelManager::ModelData data = {};
    for(size_t i = 0; i < this->modelData.size(); i++)
    {
        this->meshData = this->modelData[i];
    
        this->modelOut.instances.clear();
        this->instantiateMesh(selectable);
    
        this->modelOut.id = this->modelOut.instances.at(0).id;
        this->meshData.id = this->modelOut.id;
        data.push_back(this->meshData);
    };
    
    this->modelStore.insert(std::pair<uint32_t, ModelManager::ModelData>(this->modelOut.id, data));
    this->setSelectable(selectable);

    dest = this->modelOut;

    return;
};

void ModelManager::instantiateMesh(bool selectable)
{   
    LOG_DEBUG("Loading mesh instance(s)");
    for(size_t i = 0; i < meshData.instanceCount; i ++)
    {
        LOG_DEBUG("Mapping mesh instance");
        this->childCount += 1;

        ModelManager::Model::Instance instance = {};
        instance.id = this->childCount;
        instance.modelMatrix = meshData.globalTransform;
        instance.isClickable = selectable;
        instance.isVisible = true;
        
        /* =========================================
            Instances are created at the origin 
            looking down the z-axis at 1:1 scale to 
            that which was specified during VBO
            construction.
        ============================================ */
        
        instance.position = glm::vec3(0.0f, 0.0f, 0.0f);
        instance.direction = glm::vec3(0.0f, 0.0f, 1.0f);
        instance.scale = glm::vec3(1.0f, 1.0f, 1.0f);
        
        std::pair<uint32_t, ModelManager::Model::Instance> pair(static_cast<uint32_t>(i), instance);
        this->modelOut.instances.insert(pair);
    };
    
    return;
};

ModelManager::~ModelManager()
{
    LOG_DEBUG("Destroying Lazarus::ModelManager");
    
    this->clearErrors();

    for(auto i: this->modelStore)
    {
        for(auto j: i.second)
        {
            glDeleteBuffers         (1, &j.IIBO);
            glDeleteBuffers         (1, &j.ABO);
            glDeleteBuffers         (1, &j.MBO);
            glDeleteBuffers         (1, &j.VBO);
            glDeleteBuffers         (1, &j.EBO);
            glDeleteVertexArrays    (1, &j.VAO);
        };
    }

    this->checkErrors(__FILE__, __LINE__);
};