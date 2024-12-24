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

#ifndef LAZARUS_GLOBALS_MANAGER_H
    #include "lazarus_globals_manager.h"
#endif

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>

#include "lazarus_file_reader.h"
#include "lazarus_texture_loader.h"

using std::unique_ptr;
using std::vector;
using std::string;
using glm::vec3;
using glm::vec2;
using std::ifstream;
using std::stringstream;

#ifndef MATERIAL_LOADER_H
#define MATERIAL_LOADER_H

class MaterialLoader
{
    public:        
        MaterialLoader();
        bool loadMaterial(vector<vec3> &out, vector<vector<int>> data, string materialPath);
        virtual ~MaterialLoader();

    private:
        unique_ptr<FileReader> fileReader;
    	unique_ptr<TextureLoader> textureLoader;
        
        vec3 diffuse;                                           //  Diffuse colour, the main / dominant colour of a face
        ifstream file;
        char currentLine[256];
        int diffuseCount;                                    //  The number of times an instance of `char[]="Kd"`(diffuse color) has appeared since the last invocation
        int texCount;

        GlobalsManager globals;
};

#endif