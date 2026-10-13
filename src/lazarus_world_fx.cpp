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

WorldFX::WorldFX(Shader &shader, CameraManager::Camera &camera)
    : ParticleLoader(shader), SkyboxLoader(shader)
{
    LOG_DEBUG("Constructing Lazarus::WorldFX");

    this->error = 0;
    this->activeShaderID = 0;
    this->shader = &shader;
    this->camera = &camera;
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

lazarus_result WorldFX::drawSkyBox(WorldFX::Skybox &skyboxIn)
{
    /**
     * Because the camera is inside of a cube looking at a texture
     * from the perspective of its' interior; backface culling needs to
     * be disabled.
     */
    glDisable(GL_CULL_FACE);
    
    /**
     * For the illusion of infinite depth of the skybox to work, 
        the position data needs to be culled / decomposed out from the
        viewing matrix.
     *  This decomposition is performed by converting the 4x4 matrix to a 3x3 and 
        back again. This truncates the row from the matrix which describes the vertex 
        position from the origin and replaces it with 0's (essentially back at the origin).
        It's then changed back again afterwards.
     */
    glm::mat4 viewFromOrigin = glm::mat4(glm::mat3(this->camera->viewMatrix)); 
    
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
     * Temporarily disable depth-testing during rendering
     * Because the skybox is really just a cube sitting at the origin; this ensures that 
       draw results from anything else occupying the same space will be overwritten.
     * The implication is that skybox draw calls should always be performed first. Any draw
       call that follows prior to calling WindowManager::presentNextFrame will be rendered 
       on top of the skybox.
     */
    glDepthMask(GL_FALSE);

    lazarus_result engineStatus = WorldFX::SkyboxLoader::loadModel(skyboxIn.cube);
    if(!engineStatus)
    {
        engineStatus = WorldFX::SkyboxLoader::drawModel(skyboxIn.cube);
    }
    
    glDepthMask(GL_TRUE);

    glUniformMatrix4fv(this->viewMatrixLocation, 1, GL_FALSE, &this->camera->viewMatrix[0][0]);
    this->error = glGetError();
    if(this->error != GL_NO_ERROR)
    {
        LOG_ERROR("OpenGL Error:", __FILE__, __LINE__);
        engineStatus = lazarus_result::LAZARUS_MATRIX_LOCATION_ERROR;
    };

    /**
     * TODO:
     * This should only happen if backface culling was enabled during window
       creation
     */

    //  Turn backface culling back on
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

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

    //  Input validation
    if(this->fog.minDistance > this->fog.maxDistance)
    {
        LOG_ERROR("World Error", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_INVALID_DIMENSIONS;
    }
    else if(this->fog.density < 0.0f)
    {
        LOG_ERROR("World Error", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_INVALID_INTENSITY;
    }
    else
    {
        out = this->fog;
        return lazarus_result::LAZARUS_OK;
    }
};

lazarus_result WorldFX::drawFog(WorldFX::Fog fogIn)
{
    if(fogIn.density < 0.0f)
    {
        LOG_ERROR("World Error", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_INVALID_INTENSITY;
    };
    
    /**
     * Check whether the active shader program has changed and adjust uniforms accordingly
     * Note that this or similar should be performed by every manager at the site of setting any per-draw uniform upload data
     */
    uint32_t program = 0;
    shader->getActiveShader(program);
    if(this->activeShaderID != program)
    {
        this->activeShaderID = program;
        this->updateUniformLocations();
    };
    
    //  Set uniform values
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

/**
 * TODO:
 * Handle obj assets (i.e. need the material path)
 */

lazarus_result WorldFX::createParticles(WorldFX::ParticleSystem &out, WorldFX::ParticleSystemConfig config)
{
    LOG_DEBUG("Creating particle system");

    //  Validate inputs
    if(std::empty(config.baseAsset))
    {
        LOG_ERROR("World Error ", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_ASSET_LOAD_ERROR;
    }
    else if(
        !config.particleCount                       ||
        config.dispersion    < 0.0f                 ||
        config.decay         < 0.0f                 ||
        config.spawnRate     < 0.0f                 ||
        config.minVelocity.x > config.maxVelocity.x || 
        config.minVelocity.y > config.maxVelocity.y || 
        config.minVelocity.z > config.maxVelocity.z
    )
    {
        LOG_ERROR("World Error ", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_INVALID_INTENSITY;
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
        
        
        /**
         * TODO:
         * Make these ID's unique
         * This naive implementation will duplicate ID's when psystems are created
           from different world managers
         */
        this->particleSystem = {};
        this->particleSystem.id = this->particleSystems.size();
        this->particleSystem.spawnRate = config.spawnRate;
        this->particleSystem.maxOffset = config.maxOffset;
        this->particleSystem.position = config.epicenter;
        this->particleSystem.dispersion = config.dispersion;
        this->particleSystem.decayFactor = config.decay;
        
        this->particleSystemData = {};
        this->particleSystemData.id = this->particleSystem.id;
        this->particleSystemData.previousUptime = 0;

        if(strcmp(filetype.c_str(), "glb") == 0)
        {
            baseParticle.meshPath = config.baseAsset;
            engineCode = WorldFX::ParticleLoader::create3DAsset(this->particleSystemData.baseMesh, baseParticle);
        }
        else
        {
            baseParticle.materialPath = config.baseAsset;
            engineCode = WorldFX::ParticleLoader::createQuad(this->particleSystemData.baseMesh, baseParticle);
        }

        //  Initialise particles
        for(size_t i = 0; i < this->particleSystemData.baseMesh.instances.size(); i++)
        {
            WorldFX::ParticleSystemData::Particle particle = {};
            particle.id = i;
            particle.releaseTimer = config.spawnRate;
            
            //  Determine a semi-random speed this particle will move at between min/max velocity
            glm::vec3 range = (config.maxVelocity - config.minVelocity) * 100.0f;   //  Bring first 2 decimal places into integer range
            srand(UINT16_MAX + ((particle.id + 1) * 1000));
            float randX = range.x != 0.0f ? (rand() % static_cast<uint32_t>(range.x)) * 0.01f : 0.0f;        //  Convert back to floating point
            float randY = range.y != 0.0f ? (rand() % static_cast<uint32_t>(range.y)) * 0.01f : 0.0f;        //  Convert back to floating point
            float randZ = range.z != 0.0f ? (rand() % static_cast<uint32_t>(range.z)) * 0.01f : 0.0f;        //  Convert back to floating point
            particle.velocity = config.minVelocity + glm::vec3(randX, randY, randZ);

            //  Move particle to semi-random location and then determine how far away it is from maxOffset
            glm::vec3 start = this->particleSystem.dispersion > 0.0f 
                ? this->particleSystem.position + glm::ballRand(this->particleSystem.dispersion)
                : this->particleSystem.position;

            transformer.translateModel(
                this->particleSystemData.baseMesh, 
                start.x, start.y, start.z, 
                particle.id
            );
            particle.offset = glm::length(this->particleSystem.position - this->particleSystemData.baseMesh.instances[i].position);
            
            //  Check that the particle hasn't advanced beyond the limit
            if(particle.offset >= this->particleSystem.maxOffset)
            {
                //  Reset and start the respawn countdown
                transformer.translateModel(
                    particleSystemData.baseMesh, 
                    -this->particleSystemData.baseMesh.instances[i].position.x, 
                    -this->particleSystemData.baseMesh.instances[i].position.y, 
                    -this->particleSystemData.baseMesh.instances[i].position.z, 
                    particle.id
                );
                particle.offset = 0.0f;
                particle.releaseTimer = 0;
                this->particleSystemData.baseMesh.instances[i].opacity = 0.0f;
            }

            this->particleSystemData.particles.push_back(particle);
        }

        this->particleSystems.emplace(this->particleSystemData.id, this->particleSystemData);
        out = this->particleSystem;
    }
    
    return engineCode;
};

lazarus_result WorldFX::drawParticles(WorldFX::ParticleSystem &particleSystemIn)
{
    lazarus_result engineCode = lazarus_result::LAZARUS_OK;
    WorldFX::ParticleSystemData &systemData = this->particleSystems[particleSystemIn.id];

    for(size_t i = 0; i < systemData.particles.size(); i++)
    {
        WorldFX::ParticleSystemData::Particle &particle = systemData.particles[i];
        WorldFX::ParticleLoader::Model::Instance &meshInstance = systemData.baseMesh.instances[particle.id];

        //  Determine whether particle has been released
        if(particle.releaseTimer >= particleSystemIn.spawnRate)
        {
            //  Ensure the particle can be seen upon release
            if(meshInstance.opacity == 0.0f)
            {
                meshInstance.opacity = 1.0f;
            }
            
            if(particle.offset < particleSystemIn.maxOffset)
            {
                //  If it's a sprite then clear all previously applied rotations
                //  i.e. reset to an identity matrix but preserve the original position
                if(systemData.baseMesh.type == ParticleLoader::ModelType::PLANE)
                {
                    meshInstance.modelMatrix = glm::mat4(1.0f);
                    meshInstance.modelMatrix[3] = glm::vec4(meshInstance.position, 1.0f);
                }
                
                //  Advance the particle further towards the desired offset
                transformer.translateModel(systemData.baseMesh, particle.velocity.x, particle.velocity.y, particle.velocity.z, particle.id);
                particle.offset = glm::length(particleSystemIn.position - meshInstance.position);
                
                //  If it's a billboard / sprite rotate towards the viewport
                if(systemData.baseMesh.type == ParticleLoader::ModelType::PLANE)
                {
                    transformer.rotateModel(systemData.baseMesh, 0.0f, -this->camera->rotation.y + 90.0f, 0.0f, particle.id);
                    transformer.rotateModel(systemData.baseMesh, this->camera->rotation.x, 0.0f, 0.0f, particle.id);
                }

                /*
                    Another possible way I imagine doing this is by using the
                    inverse of the (already calculated) camera viewing matrix.
                    By switching the cameras position with the particles instead.
                    E.g.
                    
                    glm::mat4 viewInverse = glm::inverse(this->camera->viewMatrix);
                    glm::mat4 matrix = glm::mat4(
                        viewInverse[0],
                        viewInverse[1],
                        viewInverse[2],
                        meshInstance.modelMatrix[3]
                    );
                    meshInstance.modelMatrix = matrix;
                */
                
                //  Apply transparency
                meshInstance.opacity = particleSystemIn.decayFactor > 0.0f
                ? (1.0f - (particle.offset / particleSystemIn.maxOffset)) / particleSystemIn.decayFactor
                : 1.0f;
            }
            else
            {
                //  locate a spawn position
                glm::vec3 start = particleSystemIn.dispersion > 0.0f 
                ? particleSystemIn.position + glm::ballRand(particleSystemIn.dispersion)
                : particleSystemIn.position;

                //  Reset back to epicenter and begin awaiting release
                meshInstance.modelMatrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                meshInstance.position = glm::vec3(0.0f);
                transformer.translateModel(
                    systemData.baseMesh, 
                    start.x, start.y, start.z, 
                    particle.id
                );

                particle.offset = 0.0f;
                particle.releaseTimer = 0;
                meshInstance.opacity = 0.0f;
            }
        }
        else
        {
            //  Increment countdown towards respawn
            uint32_t diff = LAZARUS_UPTIME - systemData.previousUptime;
            particle.releaseTimer += diff;
            systemData.previousUptime = LAZARUS_UPTIME;
        }
    };

    engineCode = WorldFX::ParticleLoader::loadModel(systemData.baseMesh);
    if(!engineCode)
    {
        engineCode = WorldFX::ParticleLoader::drawModel(systemData.baseMesh);
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
            LOG_ERROR("World Error", __FILE__, __LINE__);
            
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