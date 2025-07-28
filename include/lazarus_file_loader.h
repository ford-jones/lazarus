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

class FileLoader 
{
    public:
		FileLoader();
        
        struct Image 
        {
            unsigned char *pixelData;

            uint32_t height;
            uint32_t width;
        };

		string relativePathToAbsolute(string filepath);
        Image loadImage(const char *filepath = NULL, const unsigned char *raw = NULL, uint32_t size = 0);
        const char *loadText(string filepath);
        
		int32_t imageWidth;
        int32_t imageHeight;
        int32_t channelCount;

        virtual ~FileLoader();
        
	private:
		unsigned char *imageData;
        unsigned char *outResize;
        
		const char *textData;

        int32_t resizeStatus;
		
        std::stringstream stringstream;
        std::filesystem::path path;
        
        string contents;
        string absolutePath;
        
        string filenameString;

        Image outImage;

        bool enforceResize;
        uint32_t maxWidth;
        uint32_t maxHeight;

        GlobalsManager globals;
};

#endif
