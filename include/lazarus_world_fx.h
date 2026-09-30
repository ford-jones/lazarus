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

#include <iostream>
#include <memory>
#include <string>

#include "lazarus_file_loader.h"
#include "lazarus_mesh.h"
#include "lazarus_camera.h"
#include "lazarus_shader.h"
#include "lazarus_transforms.h"

#include <glm/gtc/random.hpp>

#ifndef LAZARUS_WORLD_FX_H
#define LAZARUS_WORLD_FX_H
class ParticleLoader : protected ModelManager
{
    public:
        ParticleLoader(Shader &shader);
        virtual ~ParticleLoader();
};

class SkyboxLoader : protected ModelManager
{
    public:
        SkyboxLoader(Shader &shader);
        virtual ~SkyboxLoader();
};

class WorldFX : public ParticleLoader, SkyboxLoader
{
    public:
        WorldFX(Shader &shader);
        struct Skybox
        {
            std::vector<string> paths;
            std::vector<FileLoader::Image> cubeMap;

            ModelManager::Model cube;
        };

        struct Fog
        {
            glm::vec3 color;
            glm::vec3 viewpoint;

            float minDistance;
            float maxDistance;
            float density;
        };

        struct ParticleSystemConfig
        {
            std::string baseAsset;
            glm::vec3 epicenter;
            glm::vec3 minVelocity;
            glm::vec3 maxVelocity;
            uint32_t particleCount;
            uint32_t spawnRate;
            float maxOffset;
            float decay;
            float particleSize;
            float dispersion;
        };

        struct ParticleSystem
        {
            struct Particle
            {
                uint32_t id;
                uint32_t releaseTimer;
                glm::vec3 velocity;
                float targetDelta;
        };

            uint32_t spawnRate;
            float maxOffset;
            glm::vec3 position;
            float dispersion;
            std::vector<Particle> particles = {};
            ModelManager::Model base;
        };

        lazarus_result createSkyBox(
            Skybox &out,
            std::string rightPath,
            std::string leftPath,
            std::string downPath,
            std::string upPath,
            std::string frontPath,
            std::string backPath
        );
        lazarus_result drawSkyBox(Skybox &skyboxIn, CameraManager::Camera camera);

        lazarus_result createFog(
            Fog &out,
            float minDistance,
            float maxDistance,
            float thickness,
            glm::vec3 color,
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f)
        );
        lazarus_result loadFog(Fog fogIn);

        // lazarus_result createParticleSystem(
        //     ParticleSystem &out,        //  refernce out variable
        //     std::string baseAsset,      //  if obj/glb do controlled load of model, otherwise assume is texture that will go on cubes
        //     uint32_t particleCount,     //  num of instances
        //     float maxDistance,          //  Max distance from epicenter
        //     float fadeRate,             //  Translucence influence (goal: 0.0=solid, 1.0=threshold-distance)
        //     float size,                 //  By how much the base asset should scale
        //     glm::vec3 position,         //  Starting location
        //     glm::vec3 maxVelocity,      //  Directional magnitude of particle travel
        //     glm::vec3 minVelocity = glm::vec3(0.0f)
        // );
        // lazarus_result drawParticles(ParticleSystem &particleSystem);

        lazarus_result createParticleSystem(ParticleSystem &out, ParticleSystemConfig config);
        lazarus_result drawParticles(ParticleSystem &particleSystemIn);

        virtual ~WorldFX();

    private:
        lazarus_result loadSkyMap();
        lazarus_result updateUniformLocations();
        void updateParticle(ParticleSystem::Particle &particle);

        GLuint activeShaderID;
        
        GLint viewMatrixLocation;
        GLint fogColorUniformLocation;
        GLint fogViewpointUniformLocation;
        GLint fogMaxDistUniformLocation;
        GLint fogMinDistUniformLocation;
        GLint fogDensityUniformLocation;

        Skybox skybox;
        ParticleSystem particleSystem;
        Fog fog;

        Shader *shader;
        std::unique_ptr<FileLoader> fileLoader;
        Transform transformer;

        int32_t error;
        uint32_t previousUptime;
};

#endif