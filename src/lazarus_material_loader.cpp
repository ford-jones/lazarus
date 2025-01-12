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

#include "../include/lazarus_material_loader.h"

MaterialLoader::MaterialLoader()
{
	std::cout << GREEN_TEXT << "Calling constructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
	
	textureLoader = nullptr;
	diffuseCount = 0;
    texCount = 0;
};

bool MaterialLoader::loadMaterial(vector<vec3> &out, vector<vector<int>> data ,string materialPath)
{
    diffuseCount = 0;
    texCount = 0;

    file.open(materialPath.c_str());
    
    if( !file.is_open() )
    {
        globals.setExecutionState(LAZARUS_FILE_UNREADABLE);
        return false;
    }   

    while( file.getline(currentLine, 256) ) 
    {        
        /* =============================================
            Kd = diffuse colors
        ================================================ */
        if( (currentLine[0] == 'K') && (currentLine[1] == 'd') )
        {
            diffuseCount += 1;
            for(auto i: data)
            {
            	int index = i[0];
            	int faceCount = i[1];
            	
	            if(diffuseCount == index) {
                    string currentString = currentLine;
                    stringstream ss(currentString);
                    string token;

                    vector<string> tokenStore;
                    while(getline(ss, token, ' ')) 
                    {
                        tokenStore.push_back(token);
                    }

                    diffuse.r = stof(tokenStore[1]);
                    diffuse.g = stof(tokenStore[2]);
                    diffuse.b = stof(tokenStore[3]);
                    /* ====================================================
                        Push the current diffuse object into the out
                        out parameter N times.

                        N = The number of vertices which use this color.
                        (faceCount * 3)
                    ======================================================= */
    	            for(int j = 0; j < faceCount * 3; j++)
    	            {
    	                out.push_back(diffuse);
    	            };
    	        };        
            };
        }
        /* ==========================================
            map_Kd = Image texture
        ============================================= */
        if( ((currentLine[0] == 'm') && (currentLine[1] == 'a') && (currentLine[2] == 'p')))
        {
            texCount += 1;
            if( diffuseCount == 0 )
            {
                for(auto i: data)
                {
                    int faceCount = i[1];
                    for(int j = 0; j < faceCount * 3; j++)
                    {
                        /* ===========================================
                            Negative values passed here are an indicator
                            to the fragment shader that it should instead 
                            interpret the desired frag color of this face
                            from the current layer of the sampler array 
                            (an image) instead of a diffuse texture.

                            i.e: 
                                positiveDiffuseValues
                                ? fragColor(positiveDiffuseValues.xyz) 
                                : fragColor(images[layer].xyz)
                        ============================================== */
                        out.push_back(vec3(-0.1f, -0.1f, -0.1f));
                    }
                }
            }
        }
    };

    if (file.eof())
    {
        file.close();
    }

    return true;
};

MaterialLoader::~MaterialLoader()
{
    if( file.is_open() )
    {
        file.close();
    }
    std::cout << GREEN_TEXT << "Calling destructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
};