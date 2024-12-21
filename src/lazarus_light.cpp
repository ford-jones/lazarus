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
    std::cout << GREEN_TEXT << "Calling constructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;
	this->shaderProgram = shader;
}

LightManager::Light LightManager::createLightSource(double x, double y, double z, double r, double g, double b)
{	
    srand(time((0)));                                                                                                             // Seed a random number based on current time
    light.id             =   1 + (rand() % 2147483647);
    
    light.locationX = x;
    light.locationY = y;
    light.locationZ = z;

    light.lightPosition  =   vec3(light.locationX, light.locationY, light.locationZ);
    light.lightColor     =   vec3(r, g, b);
    
    light.lightPositionUniformLocation   =   glGetUniformLocation(shaderProgram, "lightPosition");
    light.lightColorUniformLocation      =   glGetUniformLocation(shaderProgram, "lightColor");

    return light;
};

void LightManager::loadLightSource(LightManager::Light &lightData)
{
    glUniform3fv        (lightData.lightPositionUniformLocation, 1, &lightData.lightPosition[0]);
    glUniform3fv        (lightData.lightColorUniformLocation, 1, &lightData.lightColor[0]);
};

LightManager::~LightManager()
{
    std::cout << GREEN_TEXT << "Calling destructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;
}