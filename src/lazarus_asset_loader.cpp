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

#include "../include/lazarus_asset_loader.h"

AssetLoader::AssetLoader()
{
	LOG_DEBUG("Constructing Lazarus::AssetLoader");

	this->materialIdentifierIndex	= 0;
	this->triangleCount				= 0;
    this->diffuseCount              = 0;
    this->textureCount              = 0;
    this->layerCount                = 0;

    this->fileLoader                = std::make_unique<FileLoader>();

    this->tempVertexPositions = {};
    this->tempNormals = {};
    this->tempUvs = {};
    this->tempJoints = {};
    this->tempWeights = {};
    this->tempImages = {};
    this->tempDiffuse ={};
};

lazarus_result AssetLoader::parseWavefrontObj(std::vector<AssetLoader::AssetData> &out, const char *meshPath, const char *materialPath) 
{
    LOG_DEBUG("Parsing wavefront obj asset....");
    this->resetMembers();
    lazarus_result status = lazarus_result::LAZARUS_OK;

    std::string path = "";
    status = fileLoader->relativePathToAbsolute(meshPath, path);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    file.open(path);

    if( !file.is_open() )
    {
        LOG_ERROR("Filesystem Error:", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_FILE_UNREADABLE;
    }

    uint32_t positionCount = 0;
    uint32_t uvCount = 0;
    uint32_t normalCount = 0;

    while(file.getline(currentLine, UINT8_MAX))
    {
        switch (currentLine[0])
        {
            case 'v':
                /*
                    v = Vertex Position Coordinates (location)
                */
                if ( currentLine[1] == ' ' )
                {
                    wavefrontCoordinates = fileLoader->splitTokensFromLine(currentLine, ' ');
                    glm::vec3 vertex = glm::vec3(0.0f, 0.0f, 0.0f);

                    vertex.x = stof(wavefrontCoordinates[1]);
                    vertex.y = stof(wavefrontCoordinates[2]);
                    vertex.z = stof(wavefrontCoordinates[3]);

                    this->tempVertexPositions.insert(std::pair(positionCount, vertex));
                    positionCount += 1;
                } 
                /*
                    vt = Vertex Texture Coordinates (UV / ST)
                */
                else if ( currentLine[1] == 't' )
                {
                    wavefrontCoordinates = fileLoader->splitTokensFromLine(currentLine, ' ');
                    glm::vec3 uv = glm::vec3(0.0f, 0.0f, 0.0f);

                    /*
                        uv is extended from its generic xy components
                        to include a z value here to meet the expected
                        stride range for attributes in the vertex buffer.
                
                        i.e: (4 * sizeof(vec3)) = 12

                        Once in the shaders it is disregarded. 
                    */

                    uv.x = stof(wavefrontCoordinates[1]);
                    uv.y = stof(wavefrontCoordinates[2]);
                    uv.z = 0.0f;

                    this->tempUvs.insert(std::pair(uvCount, uv));
                    uvCount += 1;
                }
                /*
                    vn = Vertex Normal coordinates (direction)
                */
                else if ( currentLine[1] == 'n' )
                {
                    wavefrontCoordinates = fileLoader->splitTokensFromLine(currentLine, ' ');
                    glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);

                    normal.x = stof(wavefrontCoordinates[1]);
                    normal.y = stof(wavefrontCoordinates[2]);
                    normal.z = stof(wavefrontCoordinates[3]);

                    this->tempNormals.insert(std::pair(normalCount, normal));
                    normalCount += 1;
                }
                break;
            /*
                f = Face
            */
            case 'f':
                this->triangleCount += 1;

                wavefrontCoordinates = fileLoader->splitTokensFromLine(currentLine, ' ');

                for(auto i: wavefrontCoordinates) 
                {
                    stringstream ssJ(i);
                    string tokenJ;

                    /*
                        Unlike the other identifiers on the current
                        line which are folliowed by xyz coordinates; 
                        values following a face identifier contain 
                        the indexes describing which v, vt and vn
                        lines define the properties of *this* face.:

                        Note / TODO:
                        Some editors deliminate face data with a 
                        dash character '-', others use whitespace
                        ' '. Blender uses a forward-slash '/'.
                    */
                    while(getline(ssJ, tokenJ, '/')) 
                    {
                        if (tokenJ != "f") 
                        {
                            attributeIndexes.push_back(tokenJ);
                        }
                    }
                }            

                status = this->constructTriangle();
                break;
            /*
                usemtl = Use material identifier
            */
            case 'u':
                this->materialData = {materialIdentifierIndex, triangleCount};
		    	this->materialBuffer.push_back(this->materialData);
            
                this->materialIdentifierIndex += 1;
                this->triangleCount = 0;
                break;

            default:
                break;
        }

        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };
    }

    /*
        Wavefront animation isn't supported so zero-out the
        buffers.
    */

    for(size_t i = 0; i < tempVertexPositions.size(); i++)
    {
        tempJoints.insert(std::pair<uint32_t, glm::vec4>(i, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)));
        tempWeights.insert(std::pair<uint32_t, glm::vec4>(i, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)));
    };

    /*
        All required elements have been extracted. Begin 
        construction.
    */
    AssetLoader::AssetData data = {};
    if (file.eof())
    {
        file.close();
        //  TODO:
        //  The following operations need to happen in a loop in order to load multiple assets.
        this->materialData = {materialIdentifierIndex, triangleCount};
		this->materialBuffer.push_back(this->materialData);
        
        this->parseWavefrontMtl(materialPath, materialBuffer, tempDiffuse, data.colors, data.textures);
    }

    /*
        Rigging and animation for wavefront is not 
        supported.
    */
    data.armature = {};
    data.animations = {};

    this->constructIndexBuffer(data.attributes, data.movements, data.indices, tempDiffuse, this->vertexIndices.size());
    out.push_back(data);

    return lazarus_result::LAZARUS_OK;
};

lazarus_result AssetLoader::parseWavefrontMtl(const char *materialPath, vector<vector<uint32_t>> data, vector<vec3> &temp, vector<vec3> &outColors, std::vector<FileLoader::Image> &outImages)
{    
    lazarus_result status = lazarus_result::LAZARUS_OK;

    this->textureCount = 0;
    this->diffuseCount = 0;

    if(file.is_open())
    {
        file.close();
    };
    
    std::string path = "";
    status = fileLoader->relativePathToAbsolute(materialPath, path);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };
    
    file.open(path);
    
    if( !file.is_open() )
    {
        LOG_ERROR("Filesystem Error:", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_FILE_UNREADABLE;
    }   
    
    while(file.getline(currentLine, UINT8_MAX)) 
    {        
        /*
            Kd = diffuse colors
        */
        if( (currentLine[0] == 'K') && (currentLine[1] == 'd') )
        {
            diffuseCount += 1;
            for(auto i: data)
            {
                uint32_t index = i[0];
            	uint32_t faceCount = i[1];
            	
	            if(diffuseCount == index) {
                    string currentString = currentLine;
                    stringstream ss(currentString);
                    string token;
                    
                    vector<string> tokenStore;
                    while(getline(ss, token, ' ')) 
                    {
                        tokenStore.push_back(token);
                    }
                    
                    diffuse.r = std::stof(tokenStore[1]);
                    diffuse.g = std::stof(tokenStore[2]);
                    diffuse.b = std::stof(tokenStore[3]);
                    /*
                        Push the current diffuse object into the out
                        out parameter N times.

                        N = The number of vertices which use this color.
                        (faceCount * 3)
                    */
    	            for(size_t j = 0; j < faceCount * 3; j++)
    	            {
                        temp.push_back(diffuse);
    	            };
                    FileLoader::Image image = {};
                    image.width = 0;
                    image.height = 0;
                    image.pixelData = NULL;
                    
                    outColors.push_back(diffuse);
                    outImages.push_back(image);
    	        };        
            };
        }
        /*
            map_Kd = Image texture
        */
        if( (currentLine[0] == 'm') && 
            (currentLine[1] == 'a') && 
            (currentLine[2] == 'p')
        )
        {
            textureCount += 1;
            for(auto i: data)
            {
                uint32_t index = i[0];
                uint32_t faceCount = i[1];

                if(textureCount == index)
                {
                    for(size_t j = 0; j < faceCount * 3; j++)
                    {
                        /*
                            Negative values passed here are an indicator
                            to the fragment shader that it should instead 
                            interpret the desired frag color of this face
                            from the current layer of the sampler array 
                            (an image) instead of a diffuse texture.
                            i.e: 
                            positiveDiffuseValues
                            ? fragColor(positiveDiffuseValues.xyz) 
                            : fragColor(images[layer].xyz)
                        */
                        temp.push_back(vec3(-0.1f, -0.1f, -0.1f));
                        layers.push_back(layerCount);
                    };
                }  
            } 
            /*
                Construct path to texture file using 
                pre-computed absolute path to material file and 
                replacing the file suffix with the name of the 
                file as indicated by the material file. It's for
                this reason that wavefront assets must have
                their path names stripped on export.
            */

            uint32_t suffix = std::string(materialPath).find_last_of("/");
            std::string directory = std::string(materialPath).substr(0, suffix).append("/");
            uint32_t whiteSpace = (std::string(currentLine).find(" ")) + 1;
            std::string filename = std::string(currentLine).substr(whiteSpace);
            
            const char *path = directory.append(filename).c_str();

            FileLoader::Image image = {};
            status = fileLoader->loadImage(image, path);
            if(status != lazarus_result::LAZARUS_OK)
            {
                return status;
            };

            outColors.push_back(vec3(-0.1f, -0.1f, -0.1f));
            outImages.push_back(image);
            
            layerCount += 1;
        }
    };
        
    if (file.eof())
    {
        file.close();
    }
        
    return status;
};
    
lazarus_result AssetLoader::parseGlBinary(std::vector<AssetLoader::AssetData> &out, const char* meshPath)
{
    LOG_DEBUG("Parsing glb binary asset....");
    //  TODO:
    //  Consider a threading implentation to speed up loads and not block the main-thread/renderer
    //  Get the size of this function down, too many lines... (-_-*)

    this->resetMembers();
    lazarus_result status = lazarus_result::LAZARUS_OK;
    
    std::string path = "";
    status = fileLoader->relativePathToAbsolute(meshPath, path);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    status = this->loadGlbChunks(path.c_str());
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    std::vector<std::string> propertyIdentifiers = {MESHES,NODES,SKINS,ACCESSORS,MATERIALS,TEXTURES,IMAGES,BUFFERVIEWS,BUFFERS,ANIMATIONS};
    std::vector<std::string> propertyStrings = {};
    std::vector<uint32_t> propertyIndexes = {};

    /*
        Read top-level glb json properties, find their
        locations and then remove those that aren't present.
    */
    for(size_t i = 0; i < propertyIdentifiers.size(); i++)
    {
        std::string property = propertyIdentifiers[i];   
        int32_t propertyLocation = this->jsonData.find(property);

        if(propertyLocation > 0)
        {
            propertyIndexes.push_back(propertyLocation);
        };
    };
    /*
        Sort in order of appearance and split to avoid 
        duplicate / red-herring identifiers. 
        E.g. "buffer" vs "bufferView" vs "bufferViews"
    */
    std::sort(propertyIndexes.begin(), propertyIndexes.end());

    for(size_t i = 0; i < propertyIndexes.size(); i++)
    {
        uint32_t start = propertyIndexes[i];
        uint32_t end = propertyIndexes[i + 1];

        std::string json = (i + 1) >= propertyIndexes.size() 
        ? this->jsonData.substr(start) 
        : this->jsonData.substr(start, (end - start));
        
        propertyStrings.push_back(json);
    };

    /*
        Extract variables required for reading the next 
        chunk from the json contents between each 
        propertyIndex.
    */
    for(size_t i = 0; i < propertyStrings.size(); i++)
    {
        std::string json = propertyStrings[i];

        if(json.find(MATERIALS) == 0)
        {
            LOG_DEBUG("Parsing materials...");
            uint32_t prev = 0;
            
            /*
                Split-out and identify materials
            */
            for(size_t j = 0; j < json.size(); j++)
            {
                std::string current = json.substr(j, 3);
                
                if(current == "},{" || current == "}],")
                {
                    std::string material = json.substr(prev, j - prev);
                    prev = j + 2;

                    /*
                        Check whether the mesh uses an image texture or 
                        is diffuse-colored.
                    */
                    
                    if(material.find(DIFFUSE) != std::string::npos)
                    {
                        LOG_DEBUG("Inspecting diffuse colors");
                        std::vector<std::string> colors = fileLoader->extractContainedContents(material, DIFFUSE + "[", "]");
                        for(size_t j = 0; j < colors.size(); j++)
                        {
                            std::vector<std::string> color = fileLoader->splitTokensFromLine(colors[j].c_str(), ',');
                            glbMaterialData colorMaterial = {};
                            colorMaterial.diffuse = {std::stof(color[0]), std::stof(color[1]), std::stof(color[2])};
                            colorMaterial.textureIndex = -1;
                            materials.push_back(colorMaterial);
                        };
                    }
                    else if(material.find(TEXTURE_ID) != std::string::npos)
                    {
                        LOG_DEBUG("Inspecting texture info");
                        /*
                            Identify texture index.
                        */
                        std::vector<std::string> imageTextures = fileLoader->extractContainedContents(material, TEXTURE_ID + "{", "}");
                        for(size_t j = 0; j < imageTextures.size(); j++)
                        {            
                            int32_t index = this->extractAttributeIndex(imageTextures[j], INDEX);
        
                            glbMaterialData texturedMaterial = {};
                            texturedMaterial.diffuse = glm::vec3(-0.1f, -0.1f, -0.1f);
                            texturedMaterial.textureIndex = index;
                            materials.push_back(texturedMaterial);
                        };
                    }
                    else
                    {
                        LOG_ERROR("Asset Error:", __FILE__, __LINE__);
        
                        return lazarus_result::LAZARUS_FILE_UNREADABLE;
                    };
                };
            };
        }
        else if(json.find(NODES) == 0)
        {
            LOG_DEBUG("Parsing node...");
            std::string meshData = json;

            std::vector<std::string> nodeBuff = fileLoader->extractContainedContents(meshData, "{", "}");
            for(size_t j = 0; j < nodeBuff.size(); j++)
            {
                std::string &nodeData = nodeBuff[j];
                glbNodeData node = {};
                node.id = j;
                node.children = {};
                /*
                    Extract mesh name in the event that user 
                    didn't configure one.
                */
                size_t nameStart = nodeData.find(NAME);
                std::string nameBuff = *fileLoader->extractContainedContents(nodeData.substr(nameStart + NAME.size()), "\"", "\"").data();

                if(nodeData.find(TRANSLATION) != std::string::npos)
                {
                    std::string str = fileLoader->extractContainedContents(nodeData, TRANSLATION, "]")[0];
                    std::vector<std::string> axis = fileLoader->splitTokensFromLine(str.substr(1, str.size() - 2).c_str(), ',');
        
                    node.translation.x = std::stof(axis[0]);
                    node.translation.y = std::stof(axis[1]);
                    node.translation.z = std::stof(axis[2]);
                };
                
                if(nodeData.find(ROTATION) != std::string::npos)
                {
                    std::string str = fileLoader->extractContainedContents(nodeData, ROTATION, "]")[0];
                    std::vector<std::string> axis = fileLoader->splitTokensFromLine(str.substr(1, str.size() - 2).c_str(), ',');

                    /*
                        glb rotations are supplied as a
                        quaternion.
                    */
                    node.rotation.x = std::stof(axis[0]);
                    node.rotation.y = std::stof(axis[1]);
                    node.rotation.z = std::stof(axis[2]);
                    node.rotation.w = std::stof(axis[3]);
                };

                if(nodeData.find(SCALE) != std::string::npos)
                {
                    std::string str = fileLoader->extractContainedContents(nodeData, SCALE, "]")[0];
                    std::vector<std::string> axis = fileLoader->splitTokensFromLine(str.substr(1, str.size() - 2).c_str(), ',');

                    node.scale.x = std::stof(axis[0]);
                    node.scale.y = std::stof(axis[1]);
                    node.scale.z = std::stof(axis[2]);
                };

                size_t childrenLocation = nodeData.find(CHILDREN);
                if(childrenLocation != std::string::npos)
                {
                    std::string data = fileLoader->extractContainedContents(nodeData, CHILDREN + "[", "]")[0];
                    std::vector<std::string> children = fileLoader->splitTokensFromLine(data.substr(0, data.size() - 1).c_str(), ',');

                    std::transform(
                        children.begin(), 
                        children.end(), 
                        std::back_inserter(node.children), 
                        [](std::string child) {
                            return std::stoi(child);
                        }
                    );
                };

                node.name = nameBuff.substr(0, nameBuff.size() - 1);
                node.meshIndex = this->extractAttributeIndex(nodeData, MESH);
                node.skinIndex = this->extractAttributeIndex(nodeData, SKIN);

                nodes.push_back(node);
            };
        }
        else if(json.find(MESHES) == 0)
        {
            LOG_DEBUG("Parsing primitives...");
            std::string meshData = json;
            std::vector<std::string> primitives = fileLoader->extractContainedContents(meshData, PRIMITIVES, "]");
            
            meshes = {};
            
            for(size_t a = 0; a < primitives.size(); a++)
            {
                std::string &primitiveData = primitives[a];
                std::vector<std::string> attributes = fileLoader->extractContainedContents(primitiveData, ATTRIBUTES, "}");
                LOG_DEBUG("Inspecting attributes");

                mesh.clear();
                for(size_t b = 0; b < attributes.size(); b++)
                {           
                    glbAttributeData properties = {};
                    std::string attributeBuff = attributes[b].substr(1, attributes[b].size() - 2);
                    std::vector<std::string> attributeProperties = fileLoader->splitTokensFromLine(attributeBuff.c_str(), ',');

                    for(size_t c = 0; c < attributeProperties.size(); c++)
                    {
                        std::string property = attributeProperties[c];
                        
                        int32_t index = property.find(":");
                        if(index < 0)
                        {
                            LOG_ERROR("Asset Error:", __FILE__, __LINE__);
                            
                            return lazarus_result::LAZARUS_ASSET_LOAD_ERROR;
                        };
    
                        /*
                            Note that UV's, joints and weights are marked as absent
                            by default. Uv's are only present when image textures
                            are in use. Joints and weights are only present if the 
                            primitives are components in an animated skin.
                        */
                        int32_t value = std::stoi(property.substr(index + 1));
                        switch (property[1])
                        {
                            //  POSITION
                            case 'P':
                                properties.positionAccessor = value;
                                break;
                            //  NORMAL
                            case 'N':
                                properties.normalsAccessor = value;
                                break;
                            //  TEXCOORD
                            case 'T':
                                properties.uvAccessor = value;
                                break;
                            //  JOINT
                            case 'J':
                                properties.jointsAccessor = value;
                                break;
                            //  WEIGHT
                            case 'W':
                                properties.weightsAccessor = value;
                                break;
                            default:
                                break;
                        };
                    };
    
                    properties.indicesAccessor = this->extractAttributeIndex(primitiveData, INDICES);
                    properties.materialIndex = this->extractAttributeIndex(primitiveData, MATERIAL_ID);
                    mesh.push_back(properties);
    
                    std::string nextObject = "},{";
                    int32_t location = primitiveData.find(nextObject);
                    primitiveData = primitiveData.substr(location + nextObject.size());
                };

                meshes.push_back(mesh);
            };
        }
        else if(json.find(SKINS) == 0)
        {
            LOG_DEBUG("Parsing skinning data...");
            std::vector<std::string> skinData = fileLoader->extractContainedContents(json, "{", "}");
            for(size_t j = 0; j < skinData.size(); j++)
            {
                glbSkinData skin = {};
                std::string jointData = fileLoader->extractContainedContents(skinData[j], JOINTS + "[", "]")[0];
                std::vector<std::string> joints = fileLoader->splitTokensFromLine(jointData.substr(0, jointData.size() -1).c_str(), ',');

                skin.inverseBindMatriceAccessor = this->extractAttributeIndex(skinData[j], INVERSE_BIND_MATRICES);
                std::transform(
                    joints.begin(), 
                    joints.end(), 
                    std::back_inserter(skin.joints),
                    [](std::string joint) {
                        return std::stoi(joint);
                    }
                );

                skins.push_back(skin);
            };
        }
        else if(json.find(ANIMATIONS) == 0)
        {
            LOG_DEBUG("Parsing animation data...");
            
            std::string animationData = json;
            std::vector<std::string> channels = fileLoader->extractContainedContents(animationData, CHANNELS + "[", "]");
            std::vector<std::string> samplers = fileLoader->extractContainedContents(animationData, SAMPLERS + "[", "]");
            
            /*
                Apply the same methodology as is used during mesh
                extraction due to the nested structs in these json 
                objects.
            */
            
            for(size_t j = 0; j < channels.size(); j++)
            {
                glbAnimationData animation = {};

                std::string targetChannels = channels[j];
                std::vector<std::string> channelInfo = fileLoader->extractContainedContents(targetChannels, TARGET, "}");

                for(size_t k = 0; k < channelInfo.size(); k++)
                {
                    glbAnimationChannel channel = {};
                    std::string info = channelInfo[k];

                    size_t pathStart = info.find(PATH);
                    std::string transformType = *fileLoader->extractContainedContents(info.substr(pathStart + PATH.size()), "\"", "\"").data();
                    
                    switch (transformType[0])
                    {
                        //  TRANSLATION
                        case 't':
                            channel.transformType = AssetData::JointMotion::TransformData::TransformType::TRANSLATION;
                            break;
                        //  ROTATION
                        case 'r':
                            channel.transformType = AssetData::JointMotion::TransformData::TransformType::ROTATION;
                            break;
                        //  SCALE
                        case 's':
                            channel.transformType = AssetData::JointMotion::TransformData::TransformType::SCALE;
                            break;
                    
                        default:
                            break;
                    };
                    channel.nodeIndex = this->extractAttributeIndex(info, NODE_ID);
                    channel.samplerIndex = this->extractAttributeIndex(targetChannels, SAMPLER_ID);
    
                    std::string nextObject = "},{";
                    int32_t location = targetChannels.find(nextObject);
                    targetChannels = targetChannels.substr(location + nextObject.size());
    
                    animation.channels.push_back(channel);
                };
    
                std::string targetSamplers = samplers[j];
                std::vector<std::string> samplerInfo = fileLoader->extractContainedContents(targetSamplers, "{", "}");
                
                for(size_t k = 0; k < samplerInfo.size(); k++)
                {
                    glbAnimationSampler sampler = {};
                    std::string info = samplerInfo[k];
    
                    size_t lerpStart = info.find(INTERPOLATION);
                    std::string lerpType = *fileLoader->extractContainedContents(info.substr(lerpStart + INTERPOLATION.size()), "\"", "\"").data();
    
                    switch (lerpType[0])
                    {
                        //  LINEAR
                        case 'L':
                            sampler.lerpType = AssetData::JointMotion::TransformData::InterpolationType::LINEAR;
                            break;
                        //  STEP
                        case 'S':
                            sampler.lerpType = AssetData::JointMotion::TransformData::InterpolationType::STEP;
                            break;
                        //  CUBIC SPLINE
                        case 'C':
                            sampler.lerpType = AssetData::JointMotion::TransformData::InterpolationType::CUBICSPLINE;
                            break;
    
                        default:
                            break;
                    };
    
                    sampler.timestepAccessor = this->extractAttributeIndex(info, INPUT);
                    sampler.keyframeContentsAccessor = this->extractAttributeIndex(info, OUTPUT);
    
                    animation.samplers.push_back(sampler);
                };
    
                animations.push_back(animation);
            }
        }
        else if(json.find(TEXTURES) == 0)
        {
            LOG_DEBUG("Parsing texture data...");
            glbTextureData texture = {};
            std::vector<std::string> textureProperties = fileLoader->extractContainedContents(json, "{", "}");
            for(size_t j = 0; j < textureProperties.size(); j++)
            {
                texture.samplerIndex = this->extractAttributeIndex(textureProperties[j], SAMPLER_ID);
                texture.imageIndex = this->extractAttributeIndex(textureProperties[j], IMAGE_ID);
                textures.push_back(texture);
            };
        }
        else if(json.find(IMAGES) == 0)
        {
            LOG_DEBUG("Copying image data...");
            std::vector<std::string> imageProperties = fileLoader->extractContainedContents(json, "{", "}");
            for(size_t j = 0; j < imageProperties.size(); j++)
            {
                glbImageData image = {}; 
                image.bufferViewIndex = this->extractAttributeIndex(imageProperties[j], BUFFERVIEW_ID);
                images.push_back(image);
            };
        }
        else if(json.find(ACCESSORS) == 0)
        {
            LOG_DEBUG("Parsing accessors...");
            std::vector<std::string> data = fileLoader->extractContainedContents(json, "{", "}");

            std::vector<std::string> types;
            types = fileLoader->extractContainedContents(json, "\"type\":\"", ",");

            for(size_t j = 0; j < data.size(); j++)
            {
                std::string type = types[j].substr(0, types[j].find("\""));
                std::string accessorData = data[j];

                glbAccessorData accessor = {};
                accessor.bufferViewIndex = this->extractAttributeIndex(accessorData, BUFFERVIEW_ID);
                accessor.componentType = this->extractAttributeIndex(accessorData, COMPONENT_TYPE);
                accessor.byteOffset = this->extractAttributeIndex(accessorData, BYTE_OFFSET);
                accessor.count = this->extractAttributeIndex(accessorData, COUNT);
                accessor.type = type;

                accessors.push_back(accessor);
            };

        }
        else if(json.find(BUFFERVIEWS) == 0)
        {
            LOG_DEBUG("Inspecting bufferviews...");
            /*
                Note that the bufferView's 'target' property (responsible for describing 
                what kind of buffer object this data should be written to) is skipped. 
                
                This is because we already know that an accessor with componentType 5123
                (GL_UNSIGNED_SHORT) or 5124 (GL_UNSIGNED_INT) will be written to a buffer of 
                type GL_ELEMENT_ARRAY_BUFFER. Anything other than that should be of 
                componentType 5126 (GL_FLOAT), which will be written to GL_ARRAY_BUFFER. If 
                it isn't then the mesh is not supported.
            */
            std::vector<std::string> data = fileLoader->extractContainedContents(json, "{", "}");
            
            for(size_t j = 0; j < data.size(); j++)
            {
                std::string bvData = data[j];

                glbBufferViewData bufferView = {};

                bufferView.bufferIndex  = this->extractAttributeIndex(bvData, BUFFER_ID);
                bufferView.byteOffset   = this->extractAttributeIndex(bvData, BYTE_OFFSET);
                bufferView.byteLength   = this->extractAttributeIndex(bvData, BYTE_LENGTH);
                bufferView.byteStride   = this->extractAttributeIndex(bvData, BYTE_STRIDE);

                bufferViews.push_back(bufferView);
            };
        }
        else if(json.find(BUFFERS) == 0)
        {
            LOG_DEBUG("Copying buffer data...");
            std::vector<std::string> data = fileLoader->extractContainedContents(json, "{", "}");
            uint32_t offset = 0;
            for(size_t j = 0; j < data.size(); j++)
            {
                std::string bufferData = data[j];

                glbBufferData buffer = {};
                buffer.offset = offset;
                buffer.stride = this->extractAttributeIndex(bufferData, BYTE_LENGTH);
                offset += buffer.stride;

                buffers.push_back(buffer);
            };
        };
    };

    /*
        Load values from this->binaryData
        ...
    */
    for(size_t i = 0; i < nodes.size(); i++)
    {
        AssetLoader::AssetData asset = {};
        glbNodeData node = nodes[i];

        tempVertexPositions.clear();
        tempNormals.clear();
        tempUvs.clear();
        tempJoints.clear();
        tempWeights.clear();
        tempDiffuse.clear();
        tempImages.clear();
        vertexIndices.clear();
        normalIndices.clear();
        uvIndices.clear();
        jointIndices.clear();
        weightIndices.clear();
        layers.clear();

        uint32_t indicesCount = 0;
        
        if(node.meshIndex >= 0)
        {
            LOG_DEBUG(std::string("Loading asset: [").append(node.name + "]").c_str());
            LOG_DEBUG(std::string("Mesh Index: ").append(std::to_string(node.meshIndex)).c_str());
            
            /*
                Skin is present so load the models rigging and 
                animation data.
            */
            if(node.skinIndex >= 0)
            {
                /*
                    Extract inverse-bind-matrices (one for each joint)
                    used to perform animation transforms on a joint.
                */
                glbSkinData skinData = skins[node.skinIndex];
                glbAccessorData accessor = accessors[skinData.inverseBindMatriceAccessor];
                glbBufferViewData bufferView = bufferViews[accessor.bufferViewIndex];

                std::vector<glm::mat4> inverseBindMatrices = {};
                this->populateVectorFromMemory(accessor, bufferView, inverseBindMatrices);

                /*
                    Load armature data

                    TODO:
                    identify armature transform node and apply it
                    against each of the joints
                */
                for(size_t j = 0; j < skinData.joints.size(); j++)
                {
                    glbNodeData jointNode = nodes[skinData.joints[j]];
                    AssetData::JointData joint = {};

                    joint.id = asset.armature.size();
                    joint.children = jointNode.children;            //  Note that these are the
                    joint.inverseBindMatrix = inverseBindMatrices[j];
                    joint.translation = jointNode.translation;
                    joint.rotation = jointNode.rotation;
                    joint.scale = jointNode.scale;

                    /*
                        Preserve and use original node index position as 
                        key for lookup by children and sampler.targets
                    */
                    asset.armature.insert(std::pair<uint32_t, AssetData::JointData>(jointNode.id, joint));
                };
                
                /*
                    Load the models animations
                */
                for(size_t j = 0; j < animations.size(); j++)
                {
                    AssetData::Animation animation = {};
                    glbAnimationData animationData = animations[j];

                    for(size_t k = 0; k < animationData.channels.size(); k++)
                    {
                        AssetData::JointMotion movement = {};

                        glbAnimationChannel channel = animationData.channels[k];
                        glbAnimationSampler sampler = animationData.samplers[channel.samplerIndex];

                        /*
                            Note: Translation, rotation and scale
                            keyframe values for a joint are stored 
                            in seperate channels.

                            So get the map key and try to insert
                            (emplace ensures uniqueness). Then
                            update the value in-place accordingly.
                        */
                        uint32_t id = asset.armature.at(channel.nodeIndex).id;
                        animation.emplace(id, movement);
                        
                        AssetData::JointMotion &m = animation.at(id);
                        AssetData::JointMotion::TransformData t = {};

                        t.transform = channel.transformType;
                        t.lerp = sampler.lerpType;
    
                        glbAccessorData kfAccessor = accessors[sampler.keyframeContentsAccessor];
                        glbAccessorData tsAccessor = accessors[sampler.timestepAccessor];
    
                        this->populateVectorFromMemory<float>(tsAccessor, bufferViews[tsAccessor.bufferViewIndex], t.timesteps);

                        /*
                            Rotation values are vec4, everything
                            else is vec3.
                        */
                        if(t.transform == AssetData::JointMotion::TransformData::TransformType::ROTATION)
                        {
                            this->populateVectorFromMemory<glm::vec4>(kfAccessor, bufferViews[kfAccessor.bufferViewIndex], t.keyframes);
                            m.rotation = t;
                        }
                        else
                        {
                            std::vector<glm::vec3> transforms;
                            this->populateVectorFromMemory<glm::vec3>(kfAccessor, bufferViews[kfAccessor.bufferViewIndex], transforms);
                            std::transform(
                                transforms.begin(), 
                                transforms.end(), 
                                std::back_inserter(t.keyframes), 
                                [](glm::vec3 transform) {
                                    return glm::vec4(transform.x, transform.y, transform.z, 1.0f);
                                }
                            );

                            if(t.transform == AssetData::JointMotion::TransformData::TransformType::TRANSLATION)
                            {
                                m.translation = t;
                            }
                            else
                            {
                                m.scale = t;
                            };
                        };
                    };

                    asset.animations.push_back(animation);
                };
            };
            
            /*
                A model may be constructed from a number of
                different meshes. A mesh may be constructed
                from a number of different attributes, each
                with their own defining characteristics. E.g. 
                some may be textured, others may use diffuse
                colors.
            */
            glbMeshData meshData = meshes[node.meshIndex];
            for(size_t j = 0; j < meshData.size(); j++)
            {
                LOG_DEBUG(std::string("Loading attribute: [").append(std::to_string(j) + "]").c_str());
                std::vector<glm::vec3> vertexPositions;
                std::vector<glm::vec3> vertexNormals;
                std::vector<glm::vec3> vertexUvs;

                std::vector<glm::vec4> vertexJoints;
                std::vector<glm::vec4> vertexWeights;
    
                glbAttributeData mesh = meshData[j];
        
                /*
                    Load face data / primitives. Unlike wavefront, this format can
                    support n'gons due to it's serialisation of indices per-face.
                    Note Uvs may not be present, in which case atleast the diffuse 
                    colors should be.

                    TODO:
                    Error if no uv's and no diffuse values
                */
                glbAccessorData posiitonAccessor = accessors[mesh.positionAccessor];
                glbAccessorData normalAccessor = accessors[mesh.normalsAccessor];

                this->populateBufferFromAccessor(posiitonAccessor, vertexPositions);
                this->populateBufferFromAccessor(normalAccessor, vertexNormals);
        
                if(mesh.uvAccessor >= 0)
                {
                    glbAccessorData uvAccessor = accessors[mesh.uvAccessor];
                    this->populateBufferFromAccessor(uvAccessor, vertexUvs);
                };

                /*
                    Load vertex joints and weights describing the
                    parts of the armature of an animated mesh that 
                    should effect a given vertex.

                    TODO:
                    Error if animation but no rigging
                    Error if weight values don't add up to 1.0
                */
                if(mesh.jointsAccessor >= 0 && mesh.weightsAccessor >= 0)
                {
                    /*
                        A single vertex may be affected by 4 different
                        joints, maximum, hence vec4 usage. The indices of 
                        effective joints are stored at  'vertexJoints' and 
                        aligned 1:1 with the contents of 'vertexWeights'. 
                        The sum of a vec4 of weights should add up to 1.0, 
                        with each component describing by how much (%) a 
                        joint transformation should effect a vertex. 
                        
                        Note that much like the vertex indices, the 
                        joint indexes here may be either 8-bit OR 16-bit.
                        ("-.-).
                    */

                    std::vector<uint32_t> joints = skins[node.skinIndex].joints;
                    std::vector<glm::u8vec4> smallRigJoints;
                    std::vector<glm::u16vec4> bigRigJoints;

                    glbAccessorData jointAccessor = accessors[mesh.jointsAccessor];
                    if(jointAccessor.componentType == GL_UNSIGNED_BYTE)
                    {
                        this->populateVectorFromMemory<glm::u8vec4>(jointAccessor, bufferViews[jointAccessor.bufferViewIndex], smallRigJoints);
                        /*
                            Using 8-bit indices.

                            Lookup joint's node position (id) in the skins array and 
                            use it to subsequently look up that nodes position in
                            the (yet to be created) flattened joints array (which 
                            excludes mesh, camera and lighting node types).
                            
                            E.g. 

                            {J,  J,  m,  c,  J,  J, ...}
                             0   1           4   5       <-childID
                             |   |     ______|   |
                             |   |    /    ______|
                            {J,  J,  J,  J,         ...}
                             0   1   2   3               <-JointData::id
                        */
                        std::transform(
                            smallRigJoints.begin(), 
                            smallRigJoints.end(),
                            std::back_inserter(vertexJoints),
                            [asset, joints](glm::u8vec4 jointIndices) {
                                uint32_t xIndex = joints[jointIndices.x];
                                uint32_t yIndex = joints[jointIndices.y];
                                uint32_t zIndex = joints[jointIndices.z];
                                uint32_t wIndex = joints[jointIndices.w];

                                return glm::vec4(
                                    asset.armature.at(xIndex).id,
                                    asset.armature.at(yIndex).id,
                                    asset.armature.at(zIndex).id,
                                    asset.armature.at(wIndex).id
                                );
                            }
                        );
                    }
                    else
                    {
                        this->populateVectorFromMemory<glm::u16vec4>(jointAccessor, bufferViews[jointAccessor.bufferViewIndex], bigRigJoints);
                        /*
                            Same again using 16-bit indices.
                        */
                        std::transform(
                            bigRigJoints.begin(), 
                            bigRigJoints.end(),
                            std::back_inserter(vertexJoints),
                            [asset, joints](glm::u16vec4 jointIndices) {
                                uint32_t xIndex = joints[jointIndices.x];
                                uint32_t yIndex = joints[jointIndices.y];
                                uint32_t zIndex = joints[jointIndices.z];
                                uint32_t wIndex = joints[jointIndices.w];

                                return glm::vec4(
                                    asset.armature.at(xIndex).id,
                                    asset.armature.at(yIndex).id,
                                    asset.armature.at(zIndex).id,
                                    asset.armature.at(wIndex).id
                                );
                            }
                        );
                    };

                    glbAccessorData weightAccessor = accessors[mesh.weightsAccessor];
                    this->populateVectorFromMemory<glm::vec4>(weightAccessor, bufferViews[weightAccessor.bufferViewIndex], vertexWeights);
                };
                
                /*
                    Load materials. Load the image from memory if the mesh
                    uses an image texture. If an image is loaded, the 
                    diffuse portion of the attributes vector is zero'd.
                */
        
                glbMaterialData material = materials[mesh.materialIndex];
                bool usesTextures = false;
                
                if(material.textureIndex >= 0)
                {
                    usesTextures = true;
        
                    glbTextureData texture = textures[material.textureIndex];
                    glbImageData image = images[texture.imageIndex];
        
                    glbBufferViewData bufferView = bufferViews[image.bufferViewIndex];
        
                    /*
                        Allocate the image buffer on the heap. Even when the texture 
                        image is compressed, it's raw size can be in the MBs and in
                        the worst case can cause stack overflows (and has).
                    */
        
                    unsigned char *buffer = new unsigned char[bufferView.byteLength];
                    std::memset(buffer, 0, sizeof(unsigned char) * bufferView.byteLength);
                    std::memcpy(buffer, &this->binaryData[bufferView.byteOffset], sizeof(unsigned char) * bufferView.byteLength);
        
                    FileLoader::Image loadResult = {};
                    status = fileLoader->loadImage(loadResult, NULL, buffer, bufferView.byteLength, false);
                    if(status != lazarus_result::LAZARUS_OK)
                    {
                        return status;
                    };
        
                    tempImages.push_back(loadResult);
                    delete[] buffer;
                }
                else
                {
                    //  TODO:
                    //  This block pops up everywhere and should be defaulted / removed

                    FileLoader::Image image = {};
                    image.width = 0;
                    image.height = 0;
                    image.pixelData = NULL;
    
                    tempImages.push_back(image);
                }
                
                /*
                    Load indices data and perform lookups.
                */
        
                glbAccessorData indicesAccessor = accessors[mesh.indicesAccessor];
                glbBufferViewData indicesBufferView = bufferViews[indicesAccessor.bufferViewIndex];
                
                indicesCount += indicesAccessor.count;
        
                uint32_t indicesOffset = indicesAccessor.byteOffset != -1 
                ? indicesBufferView.byteOffset + indicesAccessor.byteOffset
                : indicesBufferView.byteOffset;
        
                /*
                    Ensure that the correct size is being used as indices values may 
                    be expressed as either 16 OR 32 bit. 
                */
                std::vector<uint16_t> indicesShort(indicesAccessor.count);
                std::vector<uint32_t> indices(indicesAccessor.count);
                
                indicesAccessor.componentType == GL_UNSIGNED_SHORT
                ? std::memcpy(indicesShort.data(), &this->binaryData[indicesOffset], sizeof(uint16_t) * indicesAccessor.count)
                : std::memcpy(indices.data(), &this->binaryData[indicesOffset], sizeof(uint32_t) * indicesAccessor.count);
                
                /*
                    The indices / SCALAR buffer-values contained inside  of the glb file don't actually 
                    correspond directly to the indices used to traverse the VBO. Instead; these values 
                    pertain to the indices of the attributes for how a single face / primitive is  
                    constructed. I.e. if the mesh is a cube, EVERY value will be between 0 and 3 laid out in 
                    chunks of 6 at a time (four points shared between two triangles = 1 face).
        
                    To get a picture of this; export a cube as glb from some modeling software and load it. 
                    Print the contents of indices[j] here and compare them with the hardcoded index buffer 
                    contents of the 'MeshManager::createCube' function.
        
                    To get an actual index buffer, the verts must be duplicated one face at a time using 
                    these values, once that's done 'constructIndexBuffer' can be called to deduplicate them 
                    again and do it properly... sigh.
                */
        
                for(size_t j = 0; j < indicesAccessor.count; j++)
                {
                    uint32_t index = indicesAccessor.componentType == GL_UNSIGNED_SHORT
                    ? indicesShort[j]
                    : indices[j];
        
                    uint32_t serial = (indicesCount - indicesAccessor.count) + j;
        
                    /*
                        Apply localspace transforms specified by the node
                        data to each of the meshes vertices. This is done here
                        so that the orientation of each vertex is baked into
                        the VBO with no modification to the model matrix, which
                        should remain at the origin.
    
                        I.e. (rotation + translation) * scale
    
                        Using the formula described here for passive 
                        quaternion rotation:
                        https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html#:~:text=(13)-,Quaternion%20Rotation,-We%20can%20rotate
                    */
    
                    glm::quat quaternion = glm::quat(node.rotation);
                    glm::quat conjugate = glm::conjugate(quaternion);
                    glm::vec3 orientation = quaternion * vertexPositions[index] * conjugate;
    
                    tempVertexPositions.emplace(serial, (orientation + node.translation) * node.scale);
                    
                    tempNormals.emplace(serial, vertexNormals[index]);
        
                    /*
                        mesh.uvAccessor is optional. I.e. it doesn't nessecarily exist. 
                        In the case that it's not present in the json chunk, the VBO will 
                        still need to be populated so push back zeroes.
                    */
        
                    mesh.uvAccessor >= 0 
                    ? tempUvs.emplace(serial, vertexUvs[index]) 
                    : tempUvs.emplace(serial, glm::vec3(0.0f, 0.0f, 0.0f));

                    /*
                        Same goes for this animation data which is optional. Zero out the
                        relevant portion of the buffer if no animations are present.
                    */

                    mesh.jointsAccessor >= 0 
                    ? tempJoints.emplace(serial, vertexJoints[index])
                    : tempJoints.emplace(serial, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

                    mesh.weightsAccessor >= 0
                    ? tempWeights.emplace(serial, vertexWeights[index])
                    : tempWeights.emplace(serial, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
                    
                    vertexIndices.push_back(serial + 1);
                    normalIndices.push_back(serial + 1);
                    uvIndices.push_back(serial + 1);
                    jointIndices.push_back(serial + 1);
                    weightIndices.push_back(serial + 1);
        
                    tempDiffuse.push_back(material.diffuse);
                    if(usesTextures)
                    {
                        layers.push_back(layerCount);
                    };
        
                };
    
                asset.colors.push_back(material.diffuse);
                
                if(usesTextures)
                {
                    this->layerCount += 1;
                };
            };
            asset.name = node.name;
            asset.textures = tempImages;
    
            this->constructIndexBuffer(asset.attributes, asset.movements, asset.indices, tempDiffuse, tempVertexPositions.size());
            out.push_back(asset);
        };
    };

    return lazarus_result::LAZARUS_OK;
};

void AssetLoader::populateBufferFromAccessor(glbAccessorData accessor, std::vector<glm::vec3> &buffer)
{    
    glbBufferViewData bufferView = bufferViews[accessor.bufferViewIndex];

    if(accessor.type == "VEC3")
    {
        std::vector<glm::vec3> vertexData;

        this->populateVectorFromMemory<glm::vec3>(accessor, bufferView, vertexData);
        std::copy(vertexData.begin(), vertexData.end(), std::back_inserter(buffer));
    }
    else if(accessor.type == "VEC2")
    {
        std::vector<glm::vec2> vertexData;

        this->populateVectorFromMemory<glm::vec2>(accessor, bufferView, vertexData);
    
        for(size_t i = 0; i < vertexData.size(); i++)
        {
            glm::vec3 vertexAttribute = {vertexData[i].x, vertexData[i].y, 0.0f};
            buffer.push_back(vertexAttribute);
        };
    };
};

template<typename T> void AssetLoader::populateVectorFromMemory(glbAccessorData accessor, glbBufferViewData bufferView, std::vector<T> &vertexData)
{
    /*
        The bufferView.byteOffset defines the stride up-to 
        the beginning of the vertex attributes. An additional 
        offset wil be defined in the accessor if the data is 
        interleaved for striding to a specific attribute.
    */

    uint32_t offset = accessor.byteOffset != -1 
    ? accessor.byteOffset + bufferView.byteOffset 
    : bufferView.byteOffset;

    /*
        Note the spec enforces a 4-byte alignment. This means that 
        usage of sizeof(type) * accessor.count is preferable to 
        bufferView.byteLength which is merely an indication of the raw
        size and may be inclusive of padding.

        The reason it's used here is because the buffers contents may
        be interleaved, i.e. different byte-sizes.

        E.g. 
        {Pos | Norm | Uv | Indices},{...}
         ^     ^      ^    ^
         12    12     8    4 or 2
    */

    uint32_t bufferPadding = bufferView.byteLength % 4;
    uint32_t bufferSize = bufferPadding == 0
    ? bufferView.byteLength
    : bufferView.byteLength - bufferPadding;

    if(bufferView.byteStride > 0)
    {
        for(size_t i = 0; i < (bufferSize / bufferView.byteStride); i++)
        {
            T data = {};
            std::memcpy(&data, &this->binaryData[offset + (i * bufferView.byteStride)], sizeof(T));
            vertexData.push_back(data);
        };
    }
    else
    {
        vertexData.resize(accessor.count);
        std::memcpy(vertexData.data(), &this->binaryData[offset], sizeof(T) * accessor.count);    
    }
}

lazarus_result AssetLoader::loadGlbChunks(const char *filepath)
{
    file.open(filepath, std::ios::in | std::ios::binary);

    if(!file.is_open())
    {
        LOG_ERROR("Filesystem Error:", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_FILE_UNREADABLE;
    }
    else
    {
        uint32_t chunkSize = 0;

        /*
            Read first 20 bytes (header + first 8 bytes of chunk_0)
            to retrieve total size of chunk_0 and to align the 
            readers cursor to the start of the json chunkData[].
        */

        std::string headerBuffer;
        file.read(headerBuffer.data(), sizeof(char) * 20);
        std::memcpy(&chunkSize, &headerBuffer[12], sizeof(uint32_t));
        
        /*
            Read and store JSON chunkData[] up to the beginning of 
            the next chunk. This data describes how to interpret the
            bytes from the next chunk.
        */

        jsonData.resize(chunkSize);
        file.read(jsonData.data(), chunkSize);

        /*
            Read first 8 bytes of final chunk and extract the byte
            length of it's chunkData[] from the first 4. 
        */        

        std::string binaryChunkDetails;
        file.read(binaryChunkDetails.data(), sizeof(char) * 8);
        std::memcpy(&chunkSize, &binaryChunkDetails[0], sizeof(uint32_t));
        
        /*
            Read final chunkData[] from final chunk. This data may
            contain the data of several 'buffers' identifiers, split by
            buffers.byteLength
        */

        binaryData.resize(chunkSize);
        file.read(binaryData.data(), chunkSize);
        file.close();

        return lazarus_result::LAZARUS_OK;
    }
}

int32_t AssetLoader::extractAttributeIndex(std::string bounds, std::string target)
{
    int32_t out = -1;
    size_t attributeNameLocation = bounds.find(target);

    /*
        Pick out all occurances of an integer value that 
        appear following the target key / property name.
    */

    if(attributeNameLocation != std::string::npos)
    {
        std::string start = bounds.substr(attributeNameLocation + target.size());

        for(size_t i = 0; i < start.size(); i++)
        {
            if(start[i] == ',' || start[i] == '}')
            {
                out = std::stoi(start.substr(0, i));
                break;
            };
        };
    };
    
    return out;
};

void AssetLoader::constructIndexBuffer(std::vector<glm::vec3> &outAttributes, std::vector<glm::vec4> &outMovements, std::vector<uint32_t> &outIndexes, std::vector<glm::vec3> outDiffuse, uint32_t numOfAttributes)
{
    std::unordered_set<uint64_t> hashes = {};
    std::map<uint64_t, uint32_t> entries = {};

    uint32_t count = 0;

    /*
        Construct each of the mesh's vertexes. Lookup 
        attribute values from temporary buffers.
    */

    for(size_t i = 0; i < numOfAttributes; i++)
    {
        /*
            These lookups are faster as vectors for some reason unlike 
            the attribute values themselves, which are definitely 
            faster as maps.
        */

        uint32_t vertexIndex = vertexIndices[i];
        uint32_t normalIndex = normalIndices[i];
        uint32_t uvIndex     = uvIndices[i];
        uint32_t jointIndex  = jointIndices[i];
        uint32_t weightIndex = weightIndices[i];
        
        uint32_t layer              = layers.size() ? layers[i] : 0; // stupid hack, should fix
        glm::vec3 position          = this->tempVertexPositions.at(vertexIndex - 1);
        glm::vec3 diffuseColor      = outDiffuse[i];
        glm::vec3 normalCoordinates = this->tempNormals.at(normalIndex - 1);
        glm::vec3 uv                = this->tempUvs.at(uvIndex - 1);
        glm::vec4 joint             = this->tempJoints.at(jointIndex - 1);
        glm::vec4 weight            = this->tempWeights.at(weightIndex - 1);
        
        /*
            It's implicit that we are using a 
            TextureLoader::StorageType::ARRAY which means that 
            when a texture is present, we need to store it's 
            array-layer number. If there's no texture present, 
            use a uv padded with a zero.
        */

        glm::vec3 uvCoordinates = (diffuseColor.r + diffuseColor.g + diffuseColor.b) < -0.1f
        ? glm::vec3(uv.x, uv.y, static_cast<float>(layer))
        : uv;

        /*
            https://docs.vulkan.org/tutorial/latest/08_Loading_models.html#_vertex_deduplication:~:text=cppreference.com%20recommends

            Generate a hash value from the vertex attributes.
        */

        uint64_t positionHash = std::hash<glm::vec3>()(position);
        uint64_t normalHash = std::hash<glm::vec3>()(normalCoordinates);
        uint64_t uvHash = std::hash<glm::vec3>()(uvCoordinates);
        uint64_t jointHash = std::hash<glm::vec4>()(joint);

        uint64_t hash = positionHash ^ ((jointHash << 1) >> 1) ^ ((normalHash << 1) >> 1) ^ (uvHash << 1); 

        if(outAttributes.size() == 0)
        {
            outAttributes.push_back(position);
            outAttributes.push_back(diffuseColor);
            outAttributes.push_back(normalCoordinates);
            outAttributes.push_back(uvCoordinates);

            outMovements.push_back(joint);
            outMovements.push_back(weight);

            outIndexes.push_back(count);
            entries.emplace(hash, count);
        }
        else
        {
            /*
                Perform deduplication of vertex attributes.

                Check for duplicates by attempting to store the 
                hash in a set (unique values only). If it fails 
                we know it's already been inserted somewhere.
            */
            
            std::pair<std::unordered_set<uint64_t>::iterator, bool> result = hashes.insert(hash);

            if(result.second)
            {
                count += 1;

                /*
                    Interleave bufferdata in order expected 
                    by MeshManager::initialiseMesh

                    I.e.
                    [ Pos | Norm | color | uv ]
                */

                outAttributes.push_back(position);
                outAttributes.push_back(diffuseColor);
                outAttributes.push_back(normalCoordinates);
                outAttributes.push_back(uvCoordinates);

                outMovements.push_back(joint);
                outMovements.push_back(weight);

                outIndexes.push_back(count);
                entries.emplace(hash, count);
            }
            else
            {
                /*
                    Identify / retreive the location of a 
                    duplicate hash value.
                */

                uint32_t location = entries.at(hash);
                outIndexes.push_back(location);
            }
        }
    }

    return;
}

lazarus_result AssetLoader::constructTriangle()
{
    /*
        The faces of a wavefront mesh will be treated as 
        primitives. I.e. they should have 3 points, each with 
        3 different vertex attributes. If the face data 
        contains any more than 9 vertex attribute indexes we 
        know this mesh hasn't been triangulated and isn't
        supported.
    */

    if ( this->attributeIndexes.size() !=  9)
    {
        LOG_ERROR("Asset Error:", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_ASSET_LOAD_ERROR;    
    }

    this->vertexIndices.push_back(std::stoi(this->attributeIndexes[0]));
    this->vertexIndices.push_back(std::stoi(this->attributeIndexes[3]));
    this->vertexIndices.push_back(std::stoi(this->attributeIndexes[6]));
    this->uvIndices    .push_back(std::stoi(this->attributeIndexes[1]));
    this->uvIndices    .push_back(std::stoi(this->attributeIndexes[4]));
    this->uvIndices    .push_back(std::stoi(this->attributeIndexes[7]));
    this->normalIndices.push_back(std::stoi(this->attributeIndexes[2]));
    this->normalIndices.push_back(std::stoi(this->attributeIndexes[5]));
    this->normalIndices.push_back(std::stoi(this->attributeIndexes[8]));

    /*
        This function is currently only used for loading
        wavefront which doesn't support animation. So the
        values pushed back here are arbitrary.
    */

    this->jointIndices.push_back(1);
    this->jointIndices.push_back(1);
    this->jointIndices.push_back(1);
    this->weightIndices.push_back(1);
    this->weightIndices.push_back(1);
    this->weightIndices.push_back(1);

    attributeIndexes.clear();

    return lazarus_result::LAZARUS_OK;
}

void AssetLoader::resetMembers()
{
    /*
        Glb
    */

    this->nodes.clear();
    this->mesh.clear();
    this->meshes.clear();
    this->skins.clear();
    this->materials.clear();
    this->textures.clear();
    this->images.clear();
    this->accessors.clear();
    this->bufferViews.clear();
    this->buffers.clear();
    this->jsonData.clear();
    this->binaryData.clear();
    this->animations.clear();

    /*
        Obj / Mtl
    */

    this->wavefrontCoordinates.clear();
    this->materialBuffer.clear();
    this->materialData.clear();
    this->materialIdentifierIndex = 0;
    this->triangleCount = 0;
    
    /*
        Shared
    */
    
    this->vertexIndices.clear();
    this->normalIndices.clear();
    this->uvIndices.clear();
    this->jointIndices.clear();
    this->weightIndices.clear();
    this->attributeIndexes.clear();
    this->tempVertexPositions.clear();
    this->tempNormals.clear();
    this->tempUvs.clear();
    this->tempJoints.clear();
    this->tempWeights.clear();
    this->tempDiffuse.clear();
    this->tempImages.clear();
    this->layers.clear();

    return;
};

AssetLoader::~AssetLoader()
{
    LOG_DEBUG("Destroying Lazarus::AssetLoader");

    if( file.is_open() )
    {
        file.close();
    };
};