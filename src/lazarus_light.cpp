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
    this->lightCount = 0;
}

LightManager::Light LightManager::createLightSource(float x, float y, float z, float r, float g, float b)
{	
    this->lightCount += 1;
    light.id             =   (this->lightCount - 1);
    
    light.locationX = x;
    light.locationY = y;
    light.locationZ = z;

    light.lightPosition  =   glm::vec3(light.locationX, light.locationY, light.locationZ);
    light.lightColor     =   glm::vec3(r, g, b);
    
    light.lightPositionUniformLocation   =   glGetUniformLocation(shaderProgram, (std::string("lightPositions[").append(std::to_string(light.id)) + "]").c_str());
    light.lightColorUniformLocation      =   glGetUniformLocation(shaderProgram, (std::string("lightColors[").append(std::to_string(light.id)) + "]").c_str());

    return light;
};

void LightManager::loadLightSource(LightManager::Light &lightData)
{
    glUniform1i         (this->lightCountLocation, this->lightCount);
    glUniform3fv        (lightData.lightPositionUniformLocation, 1, &lightData.lightPosition[0]);
    glUniform3fv        (lightData.lightColorUniformLocation, 1, &lightData.lightColor[0]);
};

LightManager::~LightManager()
{
    std::cout << GREEN_TEXT << "Calling destructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
}