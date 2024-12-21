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
//                                        
/*  LAZARUS ENGINE */

#ifndef LAZARUS_CONSTANTS_H
	#include "lazarus_constants.h"
#endif

#ifndef LAZARUS_GLOBALS_MANAGER_H
    #include "lazarus_globals_manager.h"
#endif

#include <iostream>
#include <ft2build.h>
#include <vector>
#include <string>
#include <memory>
#include <math.h>

#include "lazarus_file_reader.h"

#include FT_FREETYPE_H

#ifndef LAZARUS_FONT_LOADER_H
#define LAZARUS_FONT_LOADER_H

class FontLoader
{
    public:
        FontLoader();

        void initialise();
        int loadFont(std::string filepath, int charHeight, int charWidth);
        FileReader::Image loadCharacter(char character, int fontIndex);

        virtual ~FontLoader();

    private:
        void createBitmap();
        void flipGlyph();
        void setImageData(int width, int height, unsigned char *data);

        GlobalsManager globals;
        FileReader::Image image;
        std::vector<FT_Face> fontStack;

        int keyCode;

        FT_Matrix transformationMatrix;
        FT_Library lib;
        FT_Face fontFace;

        unsigned int glyphIndex;

        FT_Error status;

        std::unique_ptr<FileReader> fileReader;
        std::string absolutePath;
};

#endif