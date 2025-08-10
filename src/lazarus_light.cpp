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
    std::cout << GREEN_TEXT << "Calling constructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
	this->shaderProgram = shader;

    this->lightOut = {};
    this->lightData = {};
    this->lightStore = {};

    this->lightCountLocation = glGetUniformLocation(this->shaderProgram, "lightCount");
    this->lightCount = globals.getNumberOfActiveLights();
}

LightManager::Light LightManager::createLightSource(float x, float y, float z, float r, float g, float b, float brightness)
{	
    this->lightOut = {};
    this->lightData = {};
    
    lightOut.id             = lightStore.size();
    lightOut.brightness     = brightness;
    lightOut.position  = glm::vec3(x, y, z);
    lightOut.color     = glm::vec3(r, g, b);
    
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

    if(lightIn.brightness < 0.0f) globals.setExecutionState(LAZARUS_INVALID_INTENSITY);
    
    if(
        lightData.brightnessUniformLocation     >= 0 &&
        lightData.lightColorUniformLocation     >= 0 &&
        lightData.lightPositionUniformLocation  >= 0 
    )
    {
        glUniform1i         (this->lightCountLocation, this->lightCount);
        glUniform1f         (lightData.brightnessUniformLocation, lightIn.brightness);
        glUniform3fv        (lightData.lightPositionUniformLocation, 1, &lightIn.position[0]);
        glUniform3fv        (lightData.lightColorUniformLocation, 1, &lightIn.color[0]);
    }
    else
    {
        globals.setExecutionState(LAZARUS_UNIFORM_NOT_FOUND);
    };

    return;
};

LightManager::~LightManager()
{
    std::cout << GREEN_TEXT << "Calling destructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
}