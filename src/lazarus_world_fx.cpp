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

    this->previousUptime = 0;
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

lazarus_result WorldFX::createParticleSystem(WorldFX::ParticleSystem &out, WorldFX::ParticleSystemConfig config)
{
    LOG_DEBUG("Creating particle system");

    //  Validate inputs
    if(std::empty(config.baseAsset) || !config.particleCount)
    {
        LOG_ERROR("World Error: ", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_ASSET_LOAD_ERROR;
    }

    lazarus_result engineCode = lazarus_result::LAZARUS_OK;

    std::string filetype = "";
    engineCode = fileLoader->checkFileType(filetype, config.baseAsset);

    if(!engineCode)
    {
        //  Load particle base asset either from a model or an image
        ModelManager::AssetConfig baseParticle = {};
        baseParticle.name = std::string("PARTICLE_").append(config.baseAsset);
        baseParticle.instanceCount = config.particleCount;
        baseParticle.translation = config.epicenter;
        baseParticle.scale = glm::vec3(config.particleSize);
        baseParticle.textureTransparency = true;
        
        this->particleSystem = {};
        this->particleSystem.spawnRate = config.spawnRate;
        this->particleSystem.maxOffset = config.maxOffset;
        this->particleSystem.position = config.epicenter;
        this->particleSystem.dispersion = config.dispersion;
        //  TODO:
        //  Handle obj (need material)
        //  Use sprites instead of cubes, cubes look weird
        if(strcmp(filetype.c_str(), "glb") == 0)
        {
            baseParticle.meshPath = config.baseAsset;
            engineCode = WorldFX::ParticleLoader::create3DAsset(this->particleSystem.base, baseParticle);
        }
        else
        {
            baseParticle.materialPath = config.baseAsset;
            engineCode = WorldFX::ParticleLoader::createCube(this->particleSystem.base, baseParticle);
        }

        //  Initialise particles
        for(size_t i = 0; i < this->particleSystem.base.instances.size(); i++)
        {
            WorldFX::ParticleSystem::Particle particle = {};
            particle.id = i;
            particle.releaseTimer = config.spawnRate;
            
            //  Determine a semi-random speed this particle will move at between min/max velocity
            glm::vec3 range = (config.maxVelocity - config.minVelocity) * 100.0f;   //  Bring first 2 decimal places into integer range
            srand(LAZARUS_UPTIME + ((particle.id + 1) * 1000));
            float randY = range.y != 0.0f ? (rand() % static_cast<uint32_t>(range.y)) * 0.01f : 0.0f;        //  Convert back to floating point
            float randX = range.x != 0.0f ? (rand() % static_cast<uint32_t>(range.x)) * 0.01f : 0.0f;        //  Convert back to floating point
            float randZ = range.z != 0.0f ? (rand() % static_cast<uint32_t>(range.z)) * 0.01f : 0.0f;        //  Convert back to floating point
            particle.velocity = config.minVelocity + glm::vec3(randX, randY, randZ);

            //  Move particle to semi-random location and then determine how far away it is from maxOffset
            glm::vec3 start = this->particleSystem.dispersion > 0.0f 
                ? this->particleSystem.position + glm::ballRand(this->particleSystem.dispersion)
                : this->particleSystem.position;

            transformer.translateModel(this->particleSystem.base, start.x, start.y, start.z, particle.id);
            particle.targetDelta = glm::length(this->particleSystem.position - this->particleSystem.base.instances[i].position);
            
            //  Check that the particle hasn't advanced beyond the limit
            if(particle.targetDelta >= this->particleSystem.maxOffset)
            {
                //  Reset and start the respawn countdown
                transformer.translateModel(particleSystem.base, -this->particleSystem.base.instances[i].position.x, -this->particleSystem.base.instances[i].position.y, -this->particleSystem.base.instances[i].position.z, i);
                particle.targetDelta = 0.0f;
                particle.releaseTimer = 0;
                this->particleSystem.base.instances[i].isVisible = false;
            }

            this->particleSystem.particles.push_back(particle);
        }
        out = this->particleSystem;
    }
    
    return engineCode;
};

lazarus_result WorldFX::drawParticles(WorldFX::ParticleSystem &particleSystemIn)
{
    lazarus_result engineCode = lazarus_result::LAZARUS_OK;

    for(size_t i = 0; i < particleSystemIn.particles.size(); i++)
    {
        WorldFX::ParticleSystem::Particle &particle = particleSystemIn.particles[i];
        WorldFX::ParticleLoader::Model::Instance &meshInstance = particleSystemIn.base.instances[i];

        //  Determine whether particle has been released
        if(particle.releaseTimer >= particleSystemIn.spawnRate)
        {
            //  Ensure the particle can be seen upon release
            if(!meshInstance.isVisible)
            {
                meshInstance.isVisible = true;
            }
            
            //  Advance the particle further towards the target
            if(particle.targetDelta < particleSystemIn.maxOffset)
            {
                transformer.translateModel(particleSystemIn.base, particle.velocity.x, particle.velocity.y, particle.velocity.z, particle.id);
                particle.targetDelta = glm::length(particleSystemIn.position - meshInstance.position);
            }
            //  Reset back to epicenter and await release
            else
            {
                glm::vec3 start = particleSystemIn.dispersion > 0.0f 
                ? particleSystemIn.position + glm::ballRand(particleSystemIn.dispersion)
                : particleSystemIn.position;

                transformer.translateModel(particleSystemIn.base, -meshInstance.position.x, -meshInstance.position.y, -meshInstance.position.z, particle.id);
                transformer.translateModel(particleSystemIn.base, start.x, start.y, start.z, particle.id);

                particle.targetDelta = 0.0f;
                particle.releaseTimer = 0;
                meshInstance.isVisible = false;
            }
        }
        else
        {
            //  Increment countdown towards respawn
            uint32_t diff = LAZARUS_UPTIME - previousUptime;
            particle.releaseTimer += diff;
            previousUptime = LAZARUS_UPTIME;
        }
    };

    engineCode = WorldFX::ParticleLoader::loadModel(particleSystemIn.base);
    if(!engineCode)
    {
        engineCode = WorldFX::ParticleLoader::drawModel(particleSystemIn.base);
    }

    return engineCode;
};

void WorldFX::updateParticle(WorldFX::ParticleSystem::Particle &particle)
{
    //         if(distanceFromSource < particleSystem.maxOffset)
//         {
//             //  Move particle away from source in direction specified by the user
//             // glm::vec3 position = particleSystem.epicenter * particleSystem.velocity.x; 
//             transformer.translateModel(particleSystem.base, particleSystem.velocity.x, particleSystem.velocity.y, particleSystem.velocity.z, i);
//             distanceFromSource = glm::length(particleSystem.epicenter - instance.position);
//         }
//         else
//         {
//             //  Return to center
//             transformer.translateModel(particleSystem.base, -instance.position.x, -instance.position.y, -instance.position.z, i);
//             distanceFromSource = 0.0f;
//         }
    return;
}

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