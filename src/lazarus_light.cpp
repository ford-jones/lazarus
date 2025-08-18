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
    lightData.lightPositionUniformLocation   =   glGetUniformLocation(shaderProgram, (std::string("lightPositions[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());
    lightData.lightColorUniformLocation      =   glGetUniformLocation(shaderProgram, (std::string("lightColors[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());
    lightData.brightnessUniformLocation      =   glGetUniformLocation(shaderProgram, (std::string("lightBrightness[").append(std::to_string(lightData.uniformIndex)) + "]").c_str());

    lightStore.push_back(lightData);

    globals.setNumberOfActiveLights(this->lightCount);

    return lightOut;
};

void LightManager::loadLightSource(LightManager::Light &lightIn)
{
    this->lightData = lightStore[lightIn.id];

    if(lightIn.brightness < 0.0f) globals.setExecutionState(StatusCode::LAZARUS_INVALID_INTENSITY);
    
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