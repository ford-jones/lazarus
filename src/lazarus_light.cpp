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

LightManager::LightManager(GLuint shader)
{
    LOG_DEBUG("Constructing Lazarus::LightManager");
	this->shaderProgram = shader;

    this->lightOut = {};
    this->lightData = {};
    this->lightStore = {};

    this->lightCountLocation = glGetUniformLocation(this->shaderProgram, "lightCount");
    this->lightCount = globals.getNumberOfActiveLights();

    this->errorCode = 0;
}

LightManager::Light LightManager::createLightSource(glm::vec3 location, glm::vec3 color, float brightness)
{	
    this->lightOut = {};
    this->lightData = {};
    
    lightOut.id             = lightStore.size();
    lightOut.brightness     = brightness;
    lightOut.position       = location;
    lightOut.color          = color;
    
    this->lightCount += 1;
    lightData.uniformIndex                   =   (this->lightCount - 1);
    lightData.lightPositionUniformLocation   =   glGetUniformLocation(this->shaderProgram, (std::string("lightPositions[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());
    lightData.lightColorUniformLocation      =   glGetUniformLocation(this->shaderProgram, (std::string("lightColors[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());
    lightData.brightnessUniformLocation      =   glGetUniformLocation(this->shaderProgram, (std::string("lightBrightness[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());

    lightStore.insert(std::pair<uint32_t, LightManager::LightData>(lightOut.id, lightData));

    globals.setNumberOfActiveLights(this->lightCount);

    return lightOut;
};

void LightManager::loadLightSource(LightManager::Light &lightIn, int32_t shader)
{
    this->lightData = lightStore.at(lightIn.id);

    if(lightIn.brightness < 0.0f) globals.setExecutionState(StatusCode::LAZARUS_INVALID_INTENSITY);
    
    if(shader == 0)
    {
        if(
            lightData.brightnessUniformLocation     >= 0 &&
            lightData.lightColorUniformLocation     >= 0 &&
            lightData.lightPositionUniformLocation  >= 0 
        )
        {
            this->clearErrors();
    
            glUniform1i         (this->lightCountLocation, this->lightCount);
            glUniform1f         (lightData.brightnessUniformLocation, lightIn.brightness);
            glUniform3fv        (lightData.lightPositionUniformLocation, 1, &lightIn.position[0]);
            glUniform3fv        (lightData.lightColorUniformLocation, 1, &lightIn.color[0]);
    
            this->checkErrors(__FILE__, __LINE__);
        }
        else
        {
            globals.setExecutionState(StatusCode::LAZARUS_UNIFORM_NOT_FOUND);
        };
    }
    else
    {
        /* ==============================================================
            When using a shader other than that which this light manager
            was initialised with, we need to rediscover and reset the 
            values of the uniform arrays. This is an expensive opperation
            which could be better handled in userspace by creating a 
            manager instance for each shader which needs the lights.

            Another noteworthy performance reduction is that regardless
            of how many lights have been loaded with 'shader', the entire
            lightCount is sent and iterated over.
        ================================================================= */

        this->clearErrors();

        GLuint countLocation        = glGetUniformLocation(shader, "lightCount");
        GLuint positionLocation     = glGetUniformLocation(shader, (std::string("lightPositions[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());
        GLuint colorLocation        = glGetUniformLocation(shader, (std::string("lightColors[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());
        GLuint brightnessLocation   = glGetUniformLocation(shader, (std::string("lightBrightness[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());

        glUniform1i         (countLocation, this->lightCount);
        glUniform1f         (brightnessLocation, lightIn.brightness);
        glUniform3fv        (positionLocation, 1, &lightIn.position[0]);
        glUniform3fv        (colorLocation, 1, &lightIn.color[0]);
    
        this->checkErrors(__FILE__, __LINE__);
    };

    return;
};

void LightManager::checkErrors(const char *file, uint32_t line)
{
    this->errorCode = glGetError();
    
    if(this->errorCode != GL_NO_ERROR)
    {
        std::string message = std::string("OpenGL Error: ").append(std::to_string(this->errorCode));
        LOG_ERROR(message.c_str(), file, line);

        globals.setExecutionState(StatusCode::LAZARUS_OPENGL_ERROR);
    }

    return;
};

void LightManager::clearErrors()
{
    this->errorCode = glGetError();
    
    while(this->errorCode != GL_NO_ERROR)
    {
        this->errorCode = glGetError();
    };
};

LightManager::~LightManager()
{
    LOG_DEBUG("Destroying Lazarus::LightManager");
}