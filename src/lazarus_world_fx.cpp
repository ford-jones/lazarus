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

#include "../include/lazarus_world_fx.h"

ParticleLoader::ParticleLoader(Shader &shader) 
    : ModelManager(shader, TextureLoader::StorageType::ARRAY)
{
    LOG_DEBUG("Constructing Lazarus::WorldFX::ParticleLoader");
};

ParticleLoader::~ParticleLoader()
{
    LOG_DEBUG("Destroying Lazarus::WorldFX::ParticleLoader");
};

SkyboxLoader::SkyboxLoader(Shader &shader)
    : ModelManager(shader, TextureLoader::StorageType::CUBEMAP)
{
    LOG_DEBUG("Constructing Lazarus::WorldFX::SkyboxLoader");
};

SkyboxLoader::~SkyboxLoader()
{
    LOG_DEBUG("Destroying Lazarus::WorldFX::SkyboxLoader");
};

WorldFX::WorldFX(Shader &shader) 
    : ParticleLoader(shader), SkyboxLoader(shader)
{
    LOG_DEBUG("Constructing Lazarus::WorldFX");

    this->error = 0;
    this->activeShaderID = 0;
    this->shader = &shader;
    this->transformer = Transform();
    
    shader.getActiveShader(this->activeShaderID);
    this->updateUniformLocations();

    this->fileLoader = std::make_unique<FileLoader>();
    this->particleSystem = {};
    this->skybox         = {};
    this->fog            = {};
};

lazarus_result WorldFX::createSkyBox(WorldFX::Skybox &out, std::string rightPath, std::string leftPath, std::string downPath, std::string upPath, std::string frontPath, std::string backPath)
{
    LOG_DEBUG("Creating skybox");

    this->skybox = {};
    ModelManager::AssetConfig config = {};
    config.scale = glm::vec3(10.0f);

    lazarus_result status = WorldFX::SkyboxLoader::createCube(this->skybox.cube, config);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };
    this->skybox.paths = {rightPath, leftPath, downPath, upPath, frontPath, backPath};

    status = this->loadSkyMap();
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    out = this->skybox;
    return lazarus_result::LAZARUS_OK;
};

lazarus_result WorldFX::drawSkyBox(WorldFX::Skybox &skyboxIn, CameraManager::Camera camera)
{
    /*
        For the illusion of infinite depth of the skybox  tp work, 
        the translation transform needs to be culled from the
        viewing matrix (the result of the glm::lookAt() operation 
        called by CameraManager::create*Cam(...)). 
        
        This decomposition done by converting the 4x4 matrix to a 3x3 and 
        back again. This truncates the row from the matrix which
        describes the vertex position from the origin and replaces
        it with 0's (essentially back at the origin). It's then
        changed back again afterwards.
    */
    glDisable           (GL_CULL_FACE);

    glm::mat4 viewFromOrigin = glm::mat4(glm::mat3(camera.viewMatrix)); 
    
    this->error = glGetError();
    if(this->error != GL_NO_ERROR)
    {
        LOG_ERROR("OpenGL Error:", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_UNIFORM_NOT_FOUND;
    };

    glUniformMatrix4fv(this->viewMatrixLocation, 1, GL_FALSE, &viewFromOrigin[0][0]);
    this->error = glGetError();
    if(this->error != GL_NO_ERROR)
    {
        LOG_ERROR("OpenGL Error:", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_MATRIX_LOCATION_ERROR;
    };

    /**
     * Temporarily toggle depth-testing during rendering
     */
    glDepthMask(GL_FALSE);

    lazarus_result engineStatus = WorldFX::SkyboxLoader::loadModel(skyboxIn.cube);
    if(!engineStatus)
    {
        engineStatus = WorldFX::SkyboxLoader::drawModel(skyboxIn.cube);
    }
    
    glDepthMask(GL_TRUE);

    glUniformMatrix4fv(this->viewMatrixLocation, 1, GL_FALSE, &camera.viewMatrix[0][0]);
    this->error = glGetError();
    if(this->error != GL_NO_ERROR)
    {
        LOG_ERROR("OpenGL Error:", __FILE__, __LINE__);
        engineStatus = lazarus_result::LAZARUS_MATRIX_LOCATION_ERROR;
    };

    return engineStatus;
};

lazarus_result WorldFX::createFog(WorldFX::Fog &out, float minDistance, float maxDistance, float thickness, glm::vec3 color, glm::vec3 position)
{
    LOG_DEBUG("Creating world fog");
    
    this->fog = {};
    this->fog.color = color;
    this->fog.viewpoint = position;
    this->fog.density = thickness;
    this->fog.maxDistance = maxDistance;
    this->fog.minDistance = minDistance;

    out = this->fog;
    return lazarus_result::LAZARUS_OK;
};

lazarus_result WorldFX::loadFog(WorldFX::Fog fogIn)
{
    if(fogIn.density < 0.0f)
    {
        LOG_ERROR("World Error:", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_INVALID_INTENSITY;
    };
    
    uint32_t program = 0;
    shader->getActiveShader(program);
    if(this->activeShaderID != program)
    {
        this->activeShaderID = program;
        this->updateUniformLocations();
    };
    
    glUniform3fv(this->fogColorUniformLocation, 1, &fogIn.color[0]);
    glUniform3fv(this->fogViewpointUniformLocation, 1, &fogIn.viewpoint[0]);
    glUniform1f(this->fogMaxDistUniformLocation, fogIn.maxDistance);
    glUniform1f(this->fogMinDistUniformLocation, fogIn.minDistance);
    glUniform1f(this->fogDensityUniformLocation, fogIn.density);

    this->error = glGetError();
    if(this->error != GL_NO_ERROR)
    {
        LOG_ERROR("OpenGL Error:", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_UNIFORM_NOT_FOUND;
    };

    return lazarus_result::LAZARUS_OK;
};

lazarus_result WorldFX::createParticleSystem(WorldFX::ParticleSystem &out, std::string baseAsset, uint32_t particleCount, float maxDistance, float fadeRate, float size, glm::vec3 position, glm::vec3 velocity)
{
    LOG_DEBUG("Creating particle system");

    //  Validate inputs
    if(std::empty(baseAsset) || !particleCount)
    {
        LOG_ERROR("World Error: ", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_ASSET_LOAD_ERROR;
    }

    lazarus_result engineCode = lazarus_result::LAZARUS_OK;

    std::string filetype = "";
    engineCode = fileLoader->checkFileType(filetype, baseAsset);
    if(!engineCode)
    {
        this->particleSystem = {};
        this->particleSystem.epicenter = position;
        this->particleSystem.particleSize = size;
        this->particleSystem.velocity = velocity;
        this->particleSystem.decayFactor = fadeRate;
        this->particleSystem.maxOffset = maxDistance;
    
        ModelManager::AssetConfig baseParticle = {};
        baseParticle.name = std::string("PARTICLE_").append(baseAsset);
        baseParticle.instanceCount = particleCount;
        baseParticle.translation = this->particleSystem.epicenter;
        baseParticle.scale = glm::vec3(this->particleSystem.particleSize);
        baseParticle.textureTransparency = true;
        
        //  TODO:
        //  Handle obj (need material)
        if(strcmp(filetype.c_str(), "glb") == 0)
        {
            baseParticle.meshPath = baseAsset;
            engineCode = WorldFX::ParticleLoader::create3DAsset(this->particleSystem.base, baseParticle);
        }
        else
        {
            baseParticle.materialPath = baseAsset;
            engineCode = WorldFX::ParticleLoader::createCube(this->particleSystem.base, baseParticle);
        }

        for(size_t i = 0; i < this->particleSystem.base.instances.size(); i++)
        {
            //  Pick a random location somewhere inbetween maxOffset and epicenter 
            srand(LAZARUS_UPTIME + (i * 1000));
            float rand_distance = rand() % static_cast<uint32_t>(this->particleSystem.maxOffset);
            glm::vec3 offset = this->particleSystem.velocity * rand_distance;

            //  Move to the new location
            transformer.translateModel(this->particleSystem.base, offset.x, offset.y, offset.z, i);

            //  Store the current particle progress/distance
            float particle_delta = glm::length(this->particleSystem.velocity * this->particleSystem.maxOffset) - glm::length(this->particleSystem.base.instances[i].position);
            this->particleSystem.particles.push_back(particle_delta);
        }

        out = this->particleSystem;
    }

    return engineCode;
};

lazarus_result WorldFX::drawParticles(WorldFX::ParticleSystem &particleSystem)
{
    lazarus_result engineCode = lazarus_result::LAZARUS_OK;

    for(size_t i = 0; i < particleSystem.particles.size(); i++)
    {
        float &distanceFromSource = particleSystem.particles[i];
        if(distanceFromSource < particleSystem.maxOffset)
        {
            //  Move particle away from source in direction specified by the user
            transformer.translateModel(particleSystem.base, particleSystem.velocity.x, particleSystem.velocity.y, particleSystem.velocity.z, i);
            ModelManager::Model::Instance &instance = particleSystem.base.instances[i];
    
            float particle_delta = glm::length(particleSystem.epicenter) - glm::length(instance.position);
            distanceFromSource = particle_delta;
        }
        else
        {
            //  Return to center
            transformer.translateModel(particleSystem.base, particleSystem.epicenter.x, particleSystem.epicenter.y, particleSystem.epicenter.z, i);
            distanceFromSource = 0.0f;
        }
    }
    
    engineCode = WorldFX::ParticleLoader::loadModel(particleSystem.base);
    if(!engineCode)
    {
        engineCode = WorldFX::ParticleLoader::drawModel(particleSystem.base);
    }

    return engineCode;
};

lazarus_result WorldFX::loadSkyMap()
{
    lazarus_result status = lazarus_result::LAZARUS_OK;

    for(auto path: this->skybox.paths)
    {
        LOG_DEBUG(std::string("Loading skybox texture: " + path).c_str());
        std::string absolutePath = "";
        FileLoader::Image image = {};

        status = fileLoader->relativePathToAbsolute(path, absolutePath);
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };
        
        status = fileLoader->loadImage(image, absolutePath.c_str());
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };
        
        /*
            Validate that the image inputs for the cubemap are 
            each of the same size. 

            (OpenGL actually already does this and will surface a 
            GL_INVALID_ENUM error if the check returns false this just 
            helps narrow down an exact cause if the gl code occurs). 
        */
        if(
            !std::empty(this->skybox.cubeMap) && 
            (image.width != image.height || image.width != this->skybox.cubeMap[0].width)
        )
        {
            LOG_ERROR("World Error:", __FILE__, __LINE__);
            
            return lazarus_result::LAZARUS_INVALID_CUBEMAP;
        };
        
        this->skybox.cubeMap.push_back(image);
    };

    /*
        Access the same texture ID values used by the skybox's mesh.
        Do so by using the ModelManager's TextureManager inherited 
        members to perform texture operations for this skybox.
    */
    
    status = WorldFX::SkyboxLoader::storeCubeMap(this->skybox.cubeMap[0].width, this->skybox.cubeMap[0].height);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    }
    else
    {
        return WorldFX::SkyboxLoader::loadCubeMap(this->skybox.cubeMap);
    }
};

lazarus_result WorldFX::updateUniformLocations()
{
    LOG_DEBUG("Setting updated world uniform locations");
    /**
     * TODO:
     * Check errors
     */
    this->viewMatrixLocation            = glGetUniformLocation(this->activeShaderID, "viewMatrix");
    this->fogColorUniformLocation       = glGetUniformLocation(this->activeShaderID, "fogColor");
    this->fogViewpointUniformLocation   = glGetUniformLocation(this->activeShaderID, "fogViewpoint");
    this->fogMaxDistUniformLocation     = glGetUniformLocation(this->activeShaderID, "fogMaxDist");
    this->fogMinDistUniformLocation     = glGetUniformLocation(this->activeShaderID, "fogMinDist");
    this->fogDensityUniformLocation     = glGetUniformLocation(this->activeShaderID, "fogDensity");

    return lazarus_result::LAZARUS_OK;
};

WorldFX::~WorldFX()
{
    LOG_DEBUG("Destroying Lazarus::WorldFX");
};