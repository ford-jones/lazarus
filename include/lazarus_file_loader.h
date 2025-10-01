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
#include <memory>

#include <stb_image.h>
#include <stb_image_resize.h>

using std::ifstream;

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

		lazarus_result relativePathToAbsolute(std::string filepath, std::string &out);
        lazarus_result loadImage(Image &out, const char *filepath = NULL, const unsigned char *raw = NULL, uint32_t size = 0);
        lazarus_result loadText(std::string filepath, std::string &out);
        
        virtual ~FileLoader();
        
    private:
		unsigned char *imageData;
        unsigned char *outResize;
        
		int32_t imageWidth;
        int32_t imageHeight;
        int32_t channelCount;

        int32_t resizeStatus;
		
        std::stringstream stringstream;
        std::filesystem::path path;

        Image outImage;

        bool enforceResize;
        uint32_t maxWidth;
        uint32_t maxHeight;
};

#endif
