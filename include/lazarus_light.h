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

#ifndef LAZARUS_COMMON_H
    #include "lazarus_common.h"
#endif

#include <iostream>

#ifndef LAZARUS_LIGHT_H
#define LAZARUS_LIGHT_H

//	TODO:
//	Create a directional light

//	TODO:
//	Create a funciton which allows users to functionally change the light color

class LightManager
{
    public:
        struct Light
        {
            int id;
            
            float locationX;
            float locationY;
            float locationZ;
            
            float brightness;

            glm::vec3 lightPosition;                     //  The (x,y,z) location of the light source
            glm::vec3 lightColor;                        //  The (r,g,b) color of the light

            GLint lightPositionUniformLocation;    //  The location / index of the light position uniform inside the frag shader
            GLint lightColorUniformLocation;       //  The location / index of the light color uniform inside the frag shader
            GLint brightnessUniformLocation;
        };
        
        LightManager(GLuint shader);
        virtual ~LightManager();

        Light createLightSource(float x, float y, float z, float r, float g, float b, float brightness = 1.0f);
        void loadLightSource(Light &lightData);

    private:
        int lightCount;
        GLint lightCountLocation;
    	GLint shaderProgram;

        Light light;
        GlobalsManager globals;

};

#endif