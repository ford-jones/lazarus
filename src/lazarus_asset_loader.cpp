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
    this->texCount                  = 0;

    this->fileLoader                = std::make_unique<FileLoader>();

    this->tempVertexPositions = {};
    this->tempNormals = {};
    this->tempUvs = {};
};

bool AssetLoader::parseWavefrontObj(std::vector<glm::vec3> &outAttributes, std::vector<glm::vec3> &outDiffuse, std::vector<uint32_t> &outIndexes, const char *meshPath, const char *materialPath) 
{
    this->resetMembers();

    std::string path = fileLoader->relativePathToAbsolute(meshPath);
    file.open(path);

    if( !file.is_open() )
    {
        LOG_ERROR("Filesystem Error:", __FILE__, __LINE__);
        globals.setExecutionState(StatusCode::LAZARUS_FILE_UNREADABLE);
        
        return false;
    }

    uint32_t positionCount = 0;
    uint32_t uvCount = 0;
    uint32_t normalCount = 0;
    
    while(file.getline(currentLine, UINT8_MAX))
    {
        switch (currentLine[0])
        {
            case 'v':
                /* =============================================
                    v = Vertex Position Coordinates (location)
                ================================================ */
                if ( currentLine[1] == ' ' )
                {
                    wavefrontCoordinates = splitTokensFromLine(currentLine, ' ');

                    this->vertex.x = stof(wavefrontCoordinates[1]);
                    this->vertex.y = stof(wavefrontCoordinates[2]);
                    this->vertex.z = stof(wavefrontCoordinates[3]);

                    this->tempVertexPositions.insert(std::pair(positionCount, this->vertex));
                    positionCount += 1;
                } 
                /* =============================================
                    vt = Vertex Texture Coordinates (UV / ST)
                ================================================ */
                else if ( currentLine[1] == 't' )
                {
                    wavefrontCoordinates = splitTokensFromLine(currentLine, ' ');

                    /* =========================================
                        uv is extended from its generic xy components
                        to include a z value here to meet the expected
                        stride range for attributes in the vertex buffer.
                
                        i.e: (4 * sizeof(vec3)) = 12

                        Once in the shaders it is disregarded. 
                    ============================================ */
                    this->uv.x = stof(wavefrontCoordinates[1]);
                    this->uv.y = stof(wavefrontCoordinates[2]);
                    this->uv.z = 0.0f;

                    this->tempUvs.insert(std::pair(uvCount, this->uv));
                    uvCount += 1;
                }
                /* ==============================================
                    vn = Vertex Normal coordinates (direction)
                ================================================= */
                else if ( currentLine[1] == 'n' )
                {
                    wavefrontCoordinates = splitTokensFromLine(currentLine, ' ');

                    this->normal.x = stof(wavefrontCoordinates[1]);
                    this->normal.y = stof(wavefrontCoordinates[2]);
                    this->normal.z = stof(wavefrontCoordinates[3]);

                    this->tempNormals.insert(std::pair(normalCount, this->normal));
                    normalCount += 1;
                }
                break;
            /* ==============================================
                f = Face
            ================================================= */
            case 'f':
                this->triangleCount += 1;

                wavefrontCoordinates = splitTokensFromLine(currentLine, ' ');

                for(auto i: wavefrontCoordinates) 
                {
                    stringstream ssJ(i);
                    string tokenJ;

                    /* ============================================
                        Unlike the other identifiers on the current
                        line which are folliowed by xyz coordinates; 
                        values following a face identifier contain 
                        the indexes describing which v, vt and vn
                        lines define the properties of *this* face.:

                        Note / TODO:
                        Some editors deliminate face data with a 
                        dash character '-', others use whitespace
                        ' '. Blender uses a forward-slash '/'.
                    =============================================== */
                    while(getline(ssJ, tokenJ, '/')) 
                    {
                        if (tokenJ != "f") 
                        {
                            attributeIndexes.push_back(tokenJ);
                        }
                    }
                }            

                this->constructTriangle();

                break;
            /* ===============================================
                usemtl = Use material identifier
            ================================================== */
            case 'u':
                this->materialData = {materialIdentifierIndex, triangleCount};
		    	this->materialBuffer.push_back(this->materialData);
            
                this->materialIdentifierIndex += 1;
                this->triangleCount = 0;
                break;

            default:
                break;
        }
    }

    if (file.eof())
    {
        file.close();
        this->materialData = {materialIdentifierIndex, triangleCount};
		this->materialBuffer.push_back(this->materialData);
        
        this->parseWavefrontMtl(materialPath, materialBuffer, outDiffuse);
    }

    this->constructIndexBuffer(outAttributes, outIndexes, outDiffuse, this->vertexIndices.size());

    return true;
};

bool AssetLoader::parseWavefrontMtl(const char *materialPath, vector<vector<uint32_t>> data, vector<vec3> &out)
{
    diffuseCount = 0;
    texCount = 0;

    
    if(file.is_open())
    {
        file.close();
    };
    
    std::string path = fileLoader->relativePathToAbsolute(materialPath);
    file.open(path);
    
    if( !file.is_open() )
    {
        LOG_ERROR("Filesystem Error:", __FILE__, __LINE__);
        globals.setExecutionState(StatusCode::LAZARUS_FILE_UNREADABLE);
        return false;
    }   
    
    while(file.getline(currentLine, UINT8_MAX)) 
    {        
        /* =============================================
            Kd = diffuse colors
        ================================================ */
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
                    /* ====================================================
                        Push the current diffuse object into the out
                        out parameter N times.

                        N = The number of vertices which use this color.
                        (faceCount * 3)
                    ======================================================= */
    	            for(size_t j = 0; j < faceCount * 3; j++)
    	            {
                        out.push_back(diffuse);
    	            };
    	        };        
            };
        }
        /* ==========================================
            map_Kd = Image texture
        ============================================= */
        if( (currentLine[0] == 'm') && 
            (currentLine[1] == 'a') && 
            (currentLine[2] == 'p')
        )
        {
            texCount += 1;
            if( diffuseCount == 0 )
            {
                for(auto i: data)
                {
                    uint32_t faceCount = i[1];
                    for(size_t j = 0; j < faceCount * 3; j++)
                    {
                        /* ===========================================
                            Negative values passed here are an indicator
                            to the fragment shader that it should instead 
                            interpret the desired frag color of this face
                            from the current layer of the sampler array 
                            (an image) instead of a diffuse texture.

                            i.e: 
                            positiveDiffuseValues
                            ? fragColor(positiveDiffuseValues.xyz) 
                            : fragColor(images[layer].xyz)
                        ============================================== */
                        out.push_back(vec3(-0.1f, -0.1f, -0.1f));
                    }
                }
            }
        }
    };
        
    if (file.eof())
    {
        file.close();
    }
        
    return true;
};
    
bool AssetLoader::parseGlBinary(vector<vec3> &outAttributes, vector<vec3> &outDiffuse, vector<uint32_t> &outIndexes, FileLoader::Image &outImage, const char* meshPath)
{
    //  TODO:
    //  Consider a threading implentation to speed up loads and not block the main-thread/renderer
    //  Get the size of this function down, too many lines... (-_-*)
    //  Extract animation data

    this->resetMembers();

    std::string PRIMITIVES = "\"primitives\":[";
    std::string MATERIALS = "\"materials\":[";
    std::string MATERIALID = "\"material\":";
    std::string TEXTURES = "\"textures\":[";
    std::string TEXTUREID = "\"baseColorTexture\":";
    std::string DIFFUSE = "\"baseColorFactor\":";
    std::string INDEX = "\"index\":";
    std::string ACCESSORS = "\"accessors\":[";
    std::string ATTRIBUTES = "\"attributes\":";
    std::string INDICES = "\"indices\":";
    std::string SAMPLERID = "\"sampler\":";
    std::string IMAGES = "\"images\":[";
    std::string IMAGEID = "\"source\":";
    std::string BUFFERVIEW = "\"bufferViews\":[";
    std::string BUFFERVIEWID = "\"bufferView\":";
    std::string BUFFERS = "\"buffers\":[{";
    std::string BUFFERID = "\"buffer\":";
    std::string COUNT = "\"count\":";
    std::string COMPONENTTYPE = "\"componentType\":";
    std::string BYTEOFFSET = "\"byteOffset\":";
    std::string BYTELENGTH = "\"byteLength\":";
    std::string BYTESTRIDE = "\"byteStride\":";
    
    std::string path = fileLoader->relativePathToAbsolute(meshPath);
    this->loadGlbChunks(path.c_str());

    std::vector<std::string> propertyIdentifiers = {ACCESSORS,PRIMITIVES,MATERIALS,TEXTURES,IMAGES,BUFFERVIEW,BUFFERS};
    std::vector<std::string> propertyStrings = {};
    std::vector<uint32_t> propertyIndexes = {};

    /* ====================================================
        Read top-level glb json properties, find their
        locations and then remove those that aren't present.
    ======================================================= */
    for(size_t i = 0; i < propertyIdentifiers.size(); i++)
    {
        std::string property = propertyIdentifiers[i];   
        int32_t propertyLocation = this->jsonData.find(property);

        if(propertyLocation > 0)
        {
            propertyIndexes.push_back(propertyLocation);
        };
    };
    /* ========================================================
        Sort in order of appearance and split to avoid 
        duplicate / red-herring identifiers. 
        E.g. "buffer" vs "bufferView" vs "bufferViews"
    =========================================================== */
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

    /* =====================================================
        Extract variables required for reading the next 
        chunk from the json contents between each 
        propertyIndex.
    ======================================================== */
    for(size_t i = 0; i < propertyStrings.size(); i++)
    {
        std::string json = propertyStrings[i];

        if(json.find(MATERIALS) == 0)
        {
            /* ==================================================
                Check whether the mesh uses an image texture or 
                is diffuse-colored.
            ===================================================== */
            int32_t diffuse = json.find(DIFFUSE);
            int32_t textureIndex = json.find(TEXTUREID);
            if(diffuse > 0)
            {
                std::vector<std::string> colors = extractContainedContents(json, DIFFUSE.append("["), "]");
                for(size_t j = 0; j < colors.size(); j++)
                {
                    std::vector<std::string> color = this->splitTokensFromLine(colors[j].c_str(), ',');
                    glbMaterialData colorMaterial = {};
                    colorMaterial.diffuse = {std::stof(color[0]), std::stof(color[1]), std::stof(color[2])};
                    colorMaterial.textureIndex = -1;
                    materials.push_back(colorMaterial);
                };

            }
            else if(textureIndex > 0)
            {
                /* =============================================
                    Identify texture index.
                ================================================ */
                std::string texIdxIdentifier = std::string(INDEX);
                int32_t id = json.find(texIdxIdentifier);
                std::string tex = json.substr(id + texIdxIdentifier.size());
                
                int32_t objEnd = tex.find("}");
                std::string objContents = tex.substr(0, objEnd);
            
                glbMaterialData texturedMaterial = {};
                texturedMaterial.diffuse = glm::vec3(-1.0f, -1.0f, -1.0f);
                texturedMaterial.textureIndex = std::stoi(objContents);
                materials.push_back(texturedMaterial);
            }
            else
            {
                LOG_ERROR("Asset Error:", __FILE__, __LINE__);
                globals.setExecutionState(StatusCode::LAZARUS_FILE_UNREADABLE);
            };
        }
        else if(json.find(PRIMITIVES) == 0)
        {
            std::string meshData = json;
            std::vector<std::string> attributes = extractContainedContents(meshData, ATTRIBUTES.append("{"), "}");
            for(size_t i = 0; i < attributes.size(); i++)
            {           
                glbMeshData mesh = {};
                std::vector<std::string> attributeProperties = splitTokensFromLine(attributes[i].c_str(), ',');
                /* ========================================================
                    Mark uvAccessor as not-present by default.
                =========================================================== */
                mesh.uvAccessor = -1;
                for(size_t j = 0; j < attributeProperties.size(); j++)
                {
                    std::string property = attributeProperties[j];
                    int32_t index = property.find(":");
                    if(index < 0)
                    {
                        LOG_ERROR("Asset Error:", __FILE__, __LINE__);
                        globals.setExecutionState(StatusCode::LAZARUS_FILE_UNREADABLE);
                    };


                    int32_t value = std::stoi(property.substr(index + 1));
                    switch (property[1])
                    {
                        //  POSITION
                        case 'P':
                            mesh.positionAccessor = value;
                            break;
                        //  NORMAL
                        case 'N':
                            mesh.normalsAccessor = value;
                            break;
                        //  TEXCOORD
                        case 'T':
                            mesh.uvAccessor = value;
                            break;

                        default:
                            break;
                    };
                };

                mesh.indicesAccessor = this->extractAttributeIndex(meshData, INDICES);
                mesh.materialIndex = this->extractAttributeIndex(meshData, MATERIALID);
                meshes.push_back(mesh);

                std::string nextObject = "},{";
                int32_t location = meshData.find(nextObject);
                meshData = meshData.substr(location + nextObject.size());
            }
        }
        else if(json.find(TEXTURES) == 0)
        {
            glbTextureData texture = {};

            texture.samplerIndex = this->extractAttributeIndex(json, SAMPLERID);
            texture.imageIndex = this->extractAttributeIndex(json, IMAGEID);
            textures.push_back(texture);
        }
        else if(json.find(IMAGES) == 0)
        {
            glbImageData image = {}; 

            image.bufferViewIndex = this->extractAttributeIndex(json, BUFFERVIEWID);
            images.push_back(image);
        }
        else if(json.find(ACCESSORS) == 0)
        {
            std::vector<std::string> data = this->extractContainedContents(json, "{", "}");

            std::vector<std::string> types;
            types = this->extractContainedContents(json, "\"type\":\"", ",");

            for(size_t j = 0; j < data.size(); j++)
            {
                std::string type = types[j].substr(0, types[j].find("\""));
                std::string accessorData = data[j];

                glbAccessorData accessor = {};
                accessor.bufferViewIndex = this->extractAttributeIndex(accessorData, BUFFERVIEWID);
                accessor.componentType = this->extractAttributeIndex(accessorData, COMPONENTTYPE);
                accessor.byteOffset = this->extractAttributeIndex(accessorData, BYTEOFFSET);
                accessor.count = this->extractAttributeIndex(accessorData, COUNT);
                accessor.type = type;

                accessors.push_back(accessor);
            };

        }
        else if(json.find(BUFFERVIEW) == 0)
        {
            /* ============================================================================
                Note that the bufferView's 'target' property (responsible for describing 
                what kind of buffer object this data should be written to) is skipped. 
                
                This is because we already know that an accessor with componentType 5123
                (GL_UNSIGNED_SHORT) or 5124 (GL_UNSIGNED_INT) will be written to a buffer of 
                type GL_ELEMENT_ARRAY_BUFFER. Anything other than that should be of 
                componentType 5126 (GL_FLOAT), which will be written to GL_ARRAY_BUFFER. If 
                it isn't then the mesh is not supported.
            =============================================================================== */
            std::vector<std::string> data = this->extractContainedContents(json, "{", "}");
            
            for(size_t j = 0; j < data.size(); j++)
            {
                std::string bvData = data[j];

                glbBufferViewData bufferView = {};

                bufferView.bufferIndex  = this->extractAttributeIndex(bvData, BUFFERID);
                bufferView.byteOffset   = this->extractAttributeIndex(bvData, BYTEOFFSET);
                bufferView.byteLength   = this->extractAttributeIndex(bvData, BYTELENGTH);
                bufferView.byteStride   = this->extractAttributeIndex(bvData, BYTESTRIDE);

                bufferViews.push_back(bufferView);
            };
        }
        else if(json.find(BUFFERS) == 0)
        {
            std::vector<std::string> data = this->extractContainedContents(json, "{", "}");
            uint32_t offset = 0;
            for(size_t j = 0; j < data.size(); j++)
            {
                std::string bufferData = data[j];

                glbBufferData buffer = {};
                buffer.offset = offset;
                buffer.stride = this->extractAttributeIndex(bufferData, BYTELENGTH);
                offset += buffer.stride;

                buffers.push_back(buffer);
            };
        };
    };

    uint32_t indicesCount = 0;
    /* ===========================================
        Load values from this->binaryData
        ...
    ============================================== */
    for(size_t i = 0; i < meshes.size(); i++)
    {
        std::vector<glm::vec3> vertexPositions;
        std::vector<glm::vec3> vertexNormals;
        std::vector<glm::vec3> vertexUvs;
        
        glbMeshData mesh = meshes[i];

        /* ==================================================================
            Load face data / primitives. Unlike wavefront, this format can
            support n'gons due to it's serialisation of indices per-face.
            Note Uvs may not be present.
        ===================================================================== */

        glbAccessorData posiitonAccessor = accessors[mesh.positionAccessor];
        glbAccessorData normalAccessor = accessors[mesh.normalsAccessor];

        if(mesh.uvAccessor >= 0)
        {
            glbAccessorData uvAccessor = accessors[mesh.uvAccessor];
            this->populateBufferFromAccessor(uvAccessor, vertexUvs);
        };
        this->populateBufferFromAccessor(posiitonAccessor, vertexPositions);
        this->populateBufferFromAccessor(normalAccessor, vertexNormals);
        
        /* =========================================================
            Load materials. Load the image from memory if the mesh
            uses an image texture. If an image is loaded, the 
            diffuse portion of the attributes vector is zero'd.
        ============================================================ */

        glbMaterialData material = materials[mesh.materialIndex];
        
        if(material.textureIndex >= 0)
        {
            glbTextureData texture = textures[material.textureIndex];
            glbImageData image = images[texture.imageIndex];

            glbBufferViewData bufferView = bufferViews[image.bufferViewIndex];

            /* ==============================================================
                Allocate the image buffer on the heap. Even when the texture 
                image is compressed, it's raw size can be in the MBs and in
                the worst case can cause stack overflows (and has).
            ================================================================== */

            unsigned char *buffer = new unsigned char[bufferView.byteLength];
            std::memset(buffer, 0, sizeof(unsigned char) * bufferView.byteLength);
            std::memcpy(buffer, &this->binaryData[bufferView.byteOffset], sizeof(unsigned char) * bufferView.byteLength);

            outImage = fileLoader->loadImage(nullptr, buffer, bufferView.byteLength);
            delete[] buffer;
        };
        
        /* =================================================
            Load indices data and perform lookups.
        ==================================================== */

        glbAccessorData indicesAccessor = accessors[mesh.indicesAccessor];
        glbBufferViewData indicesBufferView = bufferViews[indicesAccessor.bufferViewIndex];
        
        indicesCount += indicesAccessor.count;

        uint32_t indicesOffset = indicesAccessor.byteOffset != -1 
        ? indicesBufferView.byteOffset + indicesAccessor.byteOffset
        : indicesBufferView.byteOffset;

        /* ===============================================================
            Ensure that the correct size is being used as indices values may 
            be expressed as either 16 OR 32 bit. 
        ================================================================== */
        std::vector<uint16_t> indicesShort(indicesAccessor.count);
        std::vector<uint32_t> indices(indicesAccessor.count);
        
        indicesAccessor.componentType == GL_UNSIGNED_SHORT
        ? std::memcpy(indicesShort.data(), &this->binaryData[indicesOffset], sizeof(uint16_t) * indicesAccessor.count)
        : std::memcpy(indices.data(), &this->binaryData[indicesOffset], sizeof(uint32_t) * indicesAccessor.count);
        
        /* ========================================================================================
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
        ============================================================================================= */

        for(size_t j = 0; j < indicesAccessor.count; j++)
        {
            uint32_t index = indicesAccessor.componentType == GL_UNSIGNED_SHORT
            ? indicesShort[j]
            : indices[j];

            uint32_t serial = (indicesCount - indicesAccessor.count) + j;

            tempVertexPositions.emplace(serial, vertexPositions[index]);
            tempNormals.emplace(serial, vertexNormals[index]);

            /* ===================================================================
                mesh.uvAccessor is optional. I.e. it doesn't nessecarily exist. 
                In the case that it's not present in the json chunk, the VBO will 
                still need to be populated so push back zeroes.
            ====================================================================== */

            mesh.uvAccessor >= 0 
            ? tempUvs.emplace(serial, vertexUvs[index]) 
            : tempUvs.emplace(serial, glm::vec3(0.0f, 0.0f, 0.0f));
            
            vertexIndices.push_back(serial + 1);
            normalIndices.push_back(serial + 1);
            uvIndices.push_back(serial + 1);

            outDiffuse.push_back(material.diffuse);
        };
    };

    this->constructIndexBuffer(outAttributes, outIndexes, outDiffuse, tempVertexPositions.size());

    return true;
};

void AssetLoader::populateBufferFromAccessor(glbAccessorData accessor, std::vector<glm::vec3> &buffer)
{
    /* ==================================================================
        Accessor optionally defines an additional byteOffset. Used to
        define stride in the case that multiple accessors use the same
        bufferView.
    ===================================================================== */
    
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
    /* =====================================================
        The bufferView.byteOffset defines the stride up-to 
        the beginning of the vertex attributes. An additional 
        offset wil be defined in the accessor if the data is 
        interleaved for striding to a specific attribute.
    ======================================================== */

    uint32_t offset = accessor.byteOffset != -1 
    ? accessor.byteOffset + bufferView.byteOffset 
    : bufferView.byteOffset;

    /* ================================================================
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
    =================================================================== */

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

void AssetLoader::loadGlbChunks(const char *filepath)
{
    file.open(filepath, std::ios::in | std::ios::binary);

    if(!file.is_open())
    {
        LOG_ERROR("Filesystem Error:", __FILE__, __LINE__);
        globals.setExecutionState(StatusCode::LAZARUS_FILE_UNREADABLE);

        return;
    }
    else
    {
        uint32_t chunkSize = 0;

        /* ========================================================
            Read first 20 bytes (header + first 8 bytes of chunk_0)
            to retrieve total size of chunk_0 and to align the 
            readers cursor to the start of the json chunkData[].
        =========================================================== */

        std::string headerBuffer;
        file.read(headerBuffer.data(), sizeof(char) * 20);
        std::memcpy(&chunkSize, &headerBuffer[12], sizeof(uint32_t));
        
        /* =========================================================
            Read and store JSON chunkData[] up to the beginning of 
            the next chunk. This data describes how to interpret the
            bytes from the next chunk.
        ============================================================ */

        jsonData.resize(chunkSize);
        file.read(jsonData.data(), chunkSize);

        /* ==========================================================
            Read first 8 bytes of final chunk and extract the byte
            length of it's chunkData[] from the first 4. 
        ============================================================= */        

        std::string binaryChunkDetails;
        file.read(binaryChunkDetails.data(), sizeof(char) * 8);
        std::memcpy(&chunkSize, &binaryChunkDetails[0], sizeof(uint32_t));
        
        /* =============================================================
            Read final chunkData[] from final chunk. This data may
            contain the data of several 'buffers' identifiers, split by
            buffers.byteLength
        ================================================================ */

        binaryData.resize(chunkSize);
        file.read(binaryData.data(), chunkSize);
        file.close();

        return;
    }
}

vector<string> AssetLoader::extractContainedContents(std::string bounds, std::string containerStart, std::string containerEnd)
{
    std::vector<std::string> outContents = {};
    std::string buffer = bounds;
    bool moreToUnpack = true;
    int32_t start = 0;
    int32_t end = 0;

    /* ==============================================
        Identify target containers and split-out their
        contents.
    ================================================= */

    while(moreToUnpack)
    {
        start = buffer.find(containerStart);
        if(start < 0)
        {
            moreToUnpack = false;
            break;
        };
        buffer = buffer.substr(start + 1);
      
        end = buffer.find(containerEnd);

        std::string arrayContents = buffer.substr(containerStart.size() - 1, end - (containerStart.size() - 2));
        outContents.push_back(arrayContents);
    };

    return outContents;
}

int32_t AssetLoader::extractAttributeIndex(std::string bounds, std::string target)
{
    int32_t out = 0;
    int32_t attributeNameLocation = bounds.find(target);
    out = attributeNameLocation;

    /* ====================================================
        Pick out all occurances of an integer value that 
        appear following the target key / property name.
    ======================================================= */

    if(attributeNameLocation >= 0)
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

vector<string> AssetLoader::splitTokensFromLine(const char *wavefrontData, char delim)
{
    string token;
    string currentString = wavefrontData;
    stringstream ss(currentString);

    vector<string> tokenStore;

    while(getline(ss, token, delim)) 
    {
        tokenStore.push_back(token);
    }

    return tokenStore;
}

void AssetLoader::constructIndexBuffer(vector<vec3> &outAttributes, vector<uint32_t> &outIndexes, vector<vec3> outDiffuse, uint32_t numOfAttributes)
{
    std::unordered_set<uint64_t> hashes = {};
    std::map<uint64_t, uint32_t> entries = {};

    uint32_t count = 0;

    /* ===================================================
        Construct each of the mesh's vertexes. Lookup 
        attribute values from temporary buffers.
    ====================================================== */

    for(size_t i = 0; i < numOfAttributes; i++)
    {
        /* ===========================================================
            These lookups are faster as vectors for some reason unlike 
            the attribute values themselves, which are definitely 
            faster as maps.
        ============================================================== */
        uint32_t vertexIndex = vertexIndices[i];
        uint32_t normalIndex = normalIndices[i];
        uint32_t uvIndex     = uvIndices[i];
        
        glm::vec3 position          = this->tempVertexPositions.at(vertexIndex - 1);
        glm::vec3 normalCoordinates = this->tempNormals.at(normalIndex - 1);
        glm::vec3 uvCoordinates     = this->tempUvs.at(uvIndex - 1);
        glm::vec3 diffuseColor      = outDiffuse[i];

        /* =========================================================
            https://docs.vulkan.org/tutorial/latest/08_Loading_models.html#_vertex_deduplication:~:text=cppreference.com%20recommends

            Generate a hash value from the vertex attributes.
        ============================================================ */

        uint64_t positionHash = std::hash<glm::vec3>()(position);
        uint64_t normalHash = std::hash<glm::vec3>()(normalCoordinates);
        uint64_t uvHash = std::hash<glm::vec3>()(uvCoordinates);

        uint64_t hash = positionHash ^ ((normalHash << 1) >> 1) ^ (uvHash << 1); 

        if(outAttributes.size() == 0)
        {
            outAttributes.push_back(position);
            outAttributes.push_back(diffuseColor);
            outAttributes.push_back(normalCoordinates);
            outAttributes.push_back(uvCoordinates);

            outIndexes.push_back(count);
            entries.emplace(hash, count);
        }
        else
        {
            /* ===================================================
                Perform deduplication of vertex attributes.

                Check for duplicates by attempting to store the 
                hash in a set (unique values only). If it fails 
                we know it's already been inserted somewhere.
            ====================================================== */
            
            std::pair<std::unordered_set<uint64_t>::iterator, bool> result = hashes.insert(hash);

            if(result.second)
            {
                count += 1;

                /* =========================================
                    Interleave bufferdata in order expected 
                    by MeshManager::initialiseMesh

                    I.e.
                    [ Pos | Norm | color | uv ]
                ============================================ */

                outAttributes.push_back(position);
                outAttributes.push_back(diffuseColor);
                outAttributes.push_back(normalCoordinates);
                outAttributes.push_back(uvCoordinates);

                outIndexes.push_back(count);
                entries.emplace(hash, count);
            }
            else
            {
                /* ===========================================
                    Identify / retreive the location of a 
                    duplicate hash value.
                ============================================== */

                uint32_t location = entries.at(hash);
                outIndexes.push_back(location);
            }
        }
    }

    return;
}

void AssetLoader::constructTriangle()
{
    /* =======================================================
        The faces of a wavefront mesh will be treated as 
        primitives. I.e. they should have 3 points, each with 
        3 different vertex attributes. If the face data 
        contains any more than 9 vertex attribute indexes we 
        know this mesh hasn't been triangulated and isn't
        supported.
    ========================================================== */

    if ( this->attributeIndexes.size() !=  9)
    {
        LOG_ERROR("Asset Error:", __FILE__, __LINE__);

        globals.setExecutionState(StatusCode::LAZARUS_FILE_UNREADABLE);

        return;
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

    attributeIndexes.clear();

    return;
}

void AssetLoader::resetMembers()
{
    /* =============================
        Glb
    ================================ */

    this->meshes.clear();
    this->materials.clear();
    this->textures.clear();
    this->images.clear();
    this->accessors.clear();
    this->bufferViews.clear();
    this->buffers.clear();
    this->jsonData.clear();
    this->binaryData.clear();

    /* =============================
        Obj / Mtl
    ================================ */

    this->wavefrontCoordinates.clear();
    this->materialBuffer.clear();
    this->materialData.clear();
    this->materialIdentifierIndex = 0;
    this->triangleCount = 0;
    
    /* =============================
        Shared
    ================================ */
    
    this->vertexIndices.clear();
    this->normalIndices.clear();
    this->uvIndices.clear();
    this->attributeIndexes.clear();
    this->tempVertexPositions.clear();
    this->tempNormals.clear();
    this->tempUvs.clear();
};

AssetLoader::~AssetLoader()
{
    LOG_DEBUG("Destroying Lazarus::AssetLoader");

    if( file.is_open() )
    {
        file.close();
    };
};