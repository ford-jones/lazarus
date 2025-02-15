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

#ifndef LAZARUS_COMMON_H
	#include "lazarus_common.h"
#endif

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

#include <stb_image.h>
#include <stb_image_resize.h>

using std::ifstream;
using std::string;
using std::vector;

#ifndef LAZARUS_FILE_READER_H
#define LAZARUS_FILE_READER_H

class FileReader 
{
    public:
		FileReader();
        
        struct Image 
        {
            unsigned char *pixelData;

            int height;
            int width;
        };

		string relativePathToAbsolute(string filepath);
        Image readFromImage(string filepath);
        const char *readFromText(string filepath);
        
		int x, y, n;
        virtual ~FileReader();
        
	private:
		unsigned char *imageData;
        unsigned char *outResize;
        
		const char *textData;
        const char *img;

        int resizeStatus;
		
        std::stringstream stringstream;
        std::filesystem::path path;
        
        string contents;
        string absolutePath;
        
        string filenameString;

        Image outImage;

        bool enforceResize;
        int maxWidth;
        int maxHeight;

        GlobalsManager globals;
};

#endif
