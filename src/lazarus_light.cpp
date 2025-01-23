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

    this->lightCountLocation = glGetUniformLocation(this->shaderProgram, "lightCount");
    this->lightCount = globals.getNumberOfActiveLights();
}

LightManager::Light LightManager::createLightSource(float x, float y, float z, float r, float g, float b, float brightness)
{	
    this->lightCount += 1;
    light.id             =   (this->lightCount - 1);
    
    light.locationX = x;
    light.locationY = y;
    light.locationZ = z;
    light.brightness = brightness;

    light.lightPosition  =   glm::vec3(light.locationX, light.locationY, light.locationZ);
    light.lightColor     =   glm::vec3(r, g, b);
    
    light.lightPositionUniformLocation   =   glGetUniformLocation(shaderProgram, (std::string("lightPositions[").append(std::to_string(light.id)) + "]").c_str());
    light.lightColorUniformLocation      =   glGetUniformLocation(shaderProgram, (std::string("lightColors[").append(std::to_string(light.id)) + "]").c_str());
    light.brightnessUniformLocation      =   glGetUniformLocation(shaderProgram, (std::string("lightBrightness[").append(std::to_string(light.id)) + "]").c_str());

    globals.setNumberOfActiveLights(this->lightCount);
    return light;
};

void LightManager::loadLightSource(LightManager::Light &lightData)
{
    if(
        lightData.brightnessUniformLocation >= 0    &&
        lightData.lightColorUniformLocation >= 0    &&
        lightData.lightPositionUniformLocation >= 0 
    )
    {
        glUniform1i         (this->lightCountLocation, this->lightCount);
        glUniform1f         (lightData.brightnessUniformLocation, lightData.brightness);
        glUniform3fv        (lightData.lightPositionUniformLocation, 1, &lightData.lightPosition[0]);
        glUniform3fv        (lightData.lightColorUniformLocation, 1, &lightData.lightColor[0]);
    }
    else
    {
        globals.setExecutionState(LAZARUS_UNIFORM_NOT_FOUND);
    };
};

LightManager::~LightManager()
{
    std::cout << GREEN_TEXT << "Calling destructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
}