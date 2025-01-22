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

#ifndef LAZARUS_CONSTANTS_H
    #include "lazarus_constants.h"
#endif

#include <iostream>
#include <stdlib.h>

using glm::vec3;

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

            vec3 lightPosition;                     //  The (x,y,z) location of the light source
            vec3 lightColor;                        //  The (r,g,b) color of the light

            GLuint lightPositionUniformLocation;    //  The location / index of the light position uniform inside the frag shader
            GLuint lightColorUniformLocation;       //  The location / index of the light color uniform inside the frag shader
        };
        
        LightManager(GLuint shader);
        virtual ~LightManager();

        Light createLightSource(float x, float y, float z, float r, float g, float b);
        void loadLightSource(Light &lightData);

    private:
    	GLuint shaderProgram;
        Light light;
        
};

#endif
