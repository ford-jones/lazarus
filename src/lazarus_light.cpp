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

#include "../include/lazarus_light.h"

LightManager::LightManager(Shader &shader)
{
    LOG_DEBUG("Constructing Lazarus::LightManager");
    this->shader = &shader;
    this->activeShaderID = shader.activeProgram;
    this->updateUniformLocations();

    this->lightOut = {};
    this->lightData = {};
    this->lightStore = {};

    this->lightCountLocation = glGetUniformLocation(shader.activeProgram, "lightCount");
    this->lightCount = GlobalsManager::getNumberOfActiveLights();

    this->errorCode = 0;
}

lazarus_result LightManager::createLightSource(LightManager::Light &out, LightManager::LightConfig options)
{	
    LOG_DEBUG("Creating new light...");

    this->lightOut = {};
    this->lightData = {};
    
    this->clearErrors();

    lightOut.id = lightStore.size();
    lightOut.config = options;
    if(lightOut.config.name == "LIGHT_")
    {
        try
        {
            lightOut.config.name.append(std::to_string(lightOut.id));
        }
        catch(const std::exception& e)
        {
            LOG_ERROR(e.what(), __FILE__, __LINE__);
            return lazarus_result::LAZARUS_CAUGHT_EXCEPTION;
        }
        
    };
    
    this->lightCount += 1;
    lightData.uniformIndex                   =   (this->lightCount - 1);
    lightData.lightTypeUniformLocation       =   glGetUniformLocation(shader->activeProgram, (std::string("lightTypes[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());
    lightData.lightPositionUniformLocation   =   glGetUniformLocation(shader->activeProgram, (std::string("lightPositions[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());
    lightData.lightColorUniformLocation      =   glGetUniformLocation(shader->activeProgram, (std::string("lightColors[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());
    lightData.brightnessUniformLocation      =   glGetUniformLocation(shader->activeProgram, (std::string("lightBrightness[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());
    lightData.lightDirectionUniformLocation  =   glGetUniformLocation(shader->activeProgram, (std::string("lightDirections[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());

    lazarus_result status = this->checkErrors(__FILE__, __LINE__);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    try
    {
        lightStore.insert(std::pair<uint32_t, LightManager::LightData>(lightOut.id, lightData));
        if(GlobalsManager::getNumberOfActiveLights() < UINT8_MAX)
        {
            GlobalsManager::setNumberOfActiveLights(this->lightCount);
        }
        else
        {
            LOG_ERROR("Light Error: ", __FILE__, __LINE__);
            return lazarus_result::LAZARUS_LIMIT_REACHED;
        };
        out = lightOut;
        return lazarus_result::LAZARUS_OK;
    }
    catch(const std::exception& e)
    {
        LOG_ERROR(e.what(), __FILE__, __LINE__);
        return lazarus_result::LAZARUS_CAUGHT_EXCEPTION;
    }
};

lazarus_result LightManager::loadLightSource(LightManager::Light &lightIn)
{
    LOG_DEBUG(std::string("Loading lightsource [" + lightIn.config.name + "]").c_str());

    /*
        Rediscover GPU uniform location(s) for every light in the store if the active
        shader program has been changed since this was last called.
    */
    if(this->activeShaderID != shader->activeProgram)
    {
        this->updateUniformLocations();
        this->activeShaderID = shader->activeProgram;
    };

    /*
        Ensure that the user hasn't set a negative brightness value 
    */
    if(lightIn.config.brightness < 0.0f)
    {
        LOG_ERROR("Light Error: ", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_INVALID_INTENSITY;
    }

    /*
        Perform light store lookup and upload uniforms
    */
    try
    {
        this->lightData = lightStore.at(lightIn.id);

        glUniform1i         (this->lightCountLocation, this->lightCount);
        glUniform1i         (lightData.lightTypeUniformLocation, lightIn.config.type);
        glUniform1f         (lightData.brightnessUniformLocation, lightIn.config.brightness);
        glUniform3fv        (lightData.lightPositionUniformLocation, 1, &lightIn.config.position[0]);
        glUniform3fv        (lightData.lightColorUniformLocation, 1, &lightIn.config.color[0]);
        glUniform3fv        (lightData.lightDirectionUniformLocation, 1, &lightIn.config.direction[0]);
    
        return this->checkErrors(__FILE__, __LINE__);
    }
    catch(const std::out_of_range& e)
    {
        LOG_ERROR(e.what(), __FILE__, __LINE__);
        return lazarus_result::LAZARUS_CAUGHT_EXCEPTION;
    }
};

lazarus_result LightManager::updateUniformLocations()
{
    this->clearErrors();
    this->lightCountLocation = glGetUniformLocation(shader->activeProgram, "lightCount");

    for(auto &light : lightStore)
    {
        this->lightData = light.second;
        
        lightData.lightTypeUniformLocation       =   glGetUniformLocation(shader->activeProgram, (std::string("lightTypes[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());
        lightData.lightPositionUniformLocation   =   glGetUniformLocation(shader->activeProgram, (std::string("lightPositions[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());
        lightData.lightColorUniformLocation      =   glGetUniformLocation(shader->activeProgram, (std::string("lightColors[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());
        lightData.brightnessUniformLocation      =   glGetUniformLocation(shader->activeProgram, (std::string("lightBrightness[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());
        lightData.lightDirectionUniformLocation  =   glGetUniformLocation(shader->activeProgram, (std::string("lightDirections[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());    
    }

    return this->checkErrors(__FILE__, __LINE__);
};

lazarus_result LightManager::checkErrors(const char *file, uint32_t line)
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

void LightManager::clearErrors()
{
    this->errorCode = glGetError();
    
    while(this->errorCode != GL_NO_ERROR)
    {
        this->errorCode = glGetError();
    };

    return;
};

LightManager::~LightManager()
{
    LOG_DEBUG("Destroying Lazarus::LightManager");
}